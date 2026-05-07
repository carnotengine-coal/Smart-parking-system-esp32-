#include <WiFi.h>

// ── WIFI CREDENTIALS ──────────────────────────────
const char* ssid     = "Your wifi";
const char* password = "wifi password";

// ── SERVER ────────────────────────────────────────
WiFiServer server(80);

// ── IR SENSOR PINS ────────────────────────────────
#define IR_A    4
#define IR_B    13
#define IR_C    14

// ── ULTRASONIC SENSOR PINS ────────────────────────
#define TRIG_A  5
#define ECHO_A  18
#define TRIG_B  19
#define ECHO_B  21
#define TRIG_C  22
#define ECHO_C  23

// ── LED PINS ──────────────────────────────────────
#define RED_A   25
#define GREEN_A 26
#define RED_B   27
#define GREEN_B 32
#define RED_C   33
#define GREEN_C 15

// ── THRESHOLD ─────────────────────────────────────
#define THRESHOLD 15

// ── SLOT STATUS ───────────────────────────────────
bool occupied_A = false;
bool occupied_B = false;
bool occupied_C = false;

// ── READ ULTRASONIC DISTANCE ──────────────────────
int readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long dur = pulseIn(echoPin, HIGH, 30000);
  if (dur == 0) return 999;
  return dur * 0.034 / 2;
}

// ── CHECK IF SLOT IS OCCUPIED ─────────────────────
bool isOccupied(int irPin, int trigPin, int echoPin) {
  int irState  = digitalRead(irPin);
  int distance = readDistance(trigPin, echoPin);
  return (irState == LOW || distance < THRESHOLD);
}

// ── UPDATE LED ────────────────────────────────────
void updateLED(bool occ, int redPin, int greenPin) {
  digitalWrite(redPin,   occ ? HIGH : LOW);
  digitalWrite(greenPin, occ ? LOW  : HIGH);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // IR inputs with PULLUP so floating pin reads HIGH (EMPTY)
  pinMode(IR_A, INPUT_PULLUP);
  pinMode(IR_B, INPUT_PULLUP);
  pinMode(IR_C, INPUT_PULLUP);

  // Ultrasonic
  pinMode(TRIG_A, OUTPUT); pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT); pinMode(ECHO_B, INPUT);
  pinMode(TRIG_C, OUTPUT); pinMode(ECHO_C, INPUT);

  // LEDs
  pinMode(RED_A,   OUTPUT); pinMode(GREEN_A, OUTPUT);
  pinMode(RED_B,   OUTPUT); pinMode(GREEN_B, OUTPUT);
  pinMode(RED_C,   OUTPUT); pinMode(GREEN_C, OUTPUT);

 
  updateLED(false, RED_A, GREEN_A);
  updateLED(false, RED_B, GREEN_B);
  updateLED(false, RED_C, GREEN_C);

  // Connect WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 40) {
      Serial.println("\nFailed to connect. Check WiFi name and password.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    Serial.println("Server started. Open browser and go to:");
    Serial.print("http://");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(3000);
    return;
  }

  // Read all 3 slots
  occupied_A = isOccupied(IR_A, TRIG_A, ECHO_A);
  occupied_B = isOccupied(IR_B, TRIG_B, ECHO_B);
  occupied_C = isOccupied(IR_C, TRIG_C, ECHO_C);


  updateLED(occupied_A, RED_A, GREEN_A);
  updateLED(occupied_B, RED_B, GREEN_B);
  updateLED(occupied_C, RED_C, GREEN_C);

  // Print to Serial Monitor
  Serial.print("A: "); Serial.print(occupied_A ? "OCC" : "EMP");
  Serial.print(" | B: "); Serial.print(occupied_B ? "OCC" : "EMP");
  Serial.print(" | C: "); Serial.println(occupied_C ? "OCC" : "EMP");

  // Handle app HTTP request
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    String sA = occupied_A ? "OCCUPIED" : "EMPTY";
    String sB = occupied_B ? "OCCUPIED" : "EMPTY";
    String sC = occupied_C ? "OCCUPIED" : "EMPTY";

    String response = "SLOTA:" + sA + ",SLOTB:" + sB + ",SLOTC:" + sC;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");
    client.println();
    client.println(response);
    client.stop();

    Serial.println("Sent to App: " + response);
  }

  delay(300);
}