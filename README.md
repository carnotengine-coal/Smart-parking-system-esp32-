# Smart Parking Detection System

## IoT Lab Project — Batch J2

### Team Members
| Name | Enrollment Number |
|---|---|
| S Shavanth | BL.EN.U4ECE25230 |
| Shyam Krishnan | BL.EN.U4ECE25231 |
| Sivala Akaash Madhav | BL.EN.U4ECE25232 |

---

## Project Description
A smart parking monitoring system that detects real-time vehicle 
occupancy using IR and ultrasonic sensors. Slot status is displayed 
on a MIT App Inventor mobile application via ESP32 WiFi.

## Hardware Used
- ESP32 Dev Board
- 3x IR Sensor Module
- 3x HC-SR04 Ultrasonic Sensor
- 3x Red LED + 3x Green LED
- 220Ω Resistors
- Breadboard + Jumper Wires

## Pin Connections
| Component | ESP32 GPIO |
|---|---|
| IR Sensor A | GPIO 4 |
| IR Sensor B | GPIO 13 |
| IR Sensor C | GPIO 14 |
| Ultrasonic A TRIG/ECHO | GPIO 5 / GPIO 18 |
| Ultrasonic B TRIG/ECHO | GPIO 19 / GPIO 21 |
| Ultrasonic C TRIG/ECHO | GPIO 22 / GPIO 23 |
| Slot A Red/Green LED | GPIO 25 / GPIO 26 |
| Slot B Red/Green LED | GPIO 27 / GPIO 32 |
| Slot C Red/Green LED | GPIO 33 / GPIO 15 |

## How It Works
1. ESP32 reads IR and ultrasonic sensors every 300ms
2. Updates LED indicators (Red = Occupied, Green = Empty)
3. Hosts HTTP server returning: SLOTA:X,SLOTB:X,SLOTC:X
4. MIT App fetches data every 3 seconds
5. User can book a slot, track parking time, and get cost at Rs.50/hr

## WiFi Setup
Change these lines in the code before uploading:
```cpp
const char* ssid     = "YourWiFiName";
const char* password = "YourPassword";
```

## Files
- `SmartParking.ino` — ESP32 firmware
- `SmartParking.aia` — MIT App Inventor project
- `images/` — Prototype and circuit photos
