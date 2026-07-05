# 🕴️ Handsome-Sentinel (Yakışıklı Güvenlik Cam)

An ESP32-CAM and Arduino Uno based Telegram security camera with a twist of Turkish meme culture! 

Whenever motion is detected, this camera not only sends you a photo of the intruder but also serves them some fierce lines and GIFs from the famous "Yakışıklı Güvenlik" (Handsome Security) meme.

## 🌟 Features
- **Dual-MCU Architecture (Stability Upgrade)**: Uses an Arduino Uno dedicated solely to monitoring the PIR sensor, filtering out false positives, and sending clean triggers to the ESP32-CAM.
- **PIR Motion Detection**: Uses an HC-SR501 PIR sensor to detect motion.
- **Smart Wi-Fi Reconnect**: ESP32-CAM constantly checks its connection before sending a photo to ensure 100% reliability even after network drops.
- **Spam Protection**: The Arduino Uno enforces a strict 10-second cooldown (with a serial countdown timer) between captures so your Telegram inbox doesn't explode.
- **GC2145 Sensor Support**: Specifically adapted for the RHYX M21-45 (GC2145) camera module (which lacks hardware JPEG compression) by capturing raw RGB565 and software-encoding it to JPEG.
- **Meme Responses & GIFs**: Replies to intrusions with randomly selected lyrics and GIFs from "Yakışıklı Güvenlik"!

## 🛠️ Hardware Requirements
- ESP32-CAM Board (AI-Thinker footprint)
- Arduino Uno (or Nano/Mini)
- RHYX M21-45 (GC2145) Camera Module (or standard OV2640)
- HC-SR501 PIR Motion Sensor
- FTDI Programmer or ESP32-CAM-MB Shield (for flashing ESP32)

## ⚡ Pinout & Wiring (IMPORTANT)

**1. PIR Sensor to Arduino Uno:**
| Component | Arduino Uno Pin |
|---|---|
| PIR VCC | 5V |
| PIR GND | GND |
| PIR OUT (Data) | Pin 2 |

**2. Arduino Uno to ESP32-CAM:**
| Arduino Uno Pin | ESP32-CAM Pin |
|---|---|
| Pin 3 | GPIO 13 |
| GND | GND (Crucial for communication!) |

> [!CAUTION]
> **Voltage Warning:** The ESP32 operates at 3.3V, while the Arduino Uno operates at 5V. To prevent damage to the ESP32, the Arduino code uses an **Active LOW (Open-Drain)** logic. The Arduino leaves the trigger pin floating (high impedance) normally, and pulls it to `GND` only when motion is detected. This allows direct connection without needing a voltage divider!

## 🔧 The Problem We Faced & How We Solved It
**The Issue:** Initially, the PIR sensor was connected directly to the ESP32-CAM. However, whenever the ESP32 turned on its Flash LED or transmitted data via Wi-Fi to Telegram, it drew a massive amount of current. This sudden voltage drop caused the PIR sensor to trigger falsely, leading to an infinite loop of the camera taking photos of an empty room.

**The Solution:** We decoupled the sensor logic by introducing an **Arduino Uno**. The Uno is now solely responsible for reading the PIR sensor and filtering out voltage spikes (debouncing). When a valid motion is detected, the Uno sends a clean, isolated 200ms `LOW` signal to the ESP32-CAM. The ESP32-CAM now acts purely as a "dumb camera" that wakes up, takes a photo, sends it, and goes back to sleep, completely immune to PIR noise!

## 🚀 Setup
1. Clone the repository.
2. Open `Arduino_Uno_PIR/Arduino_Uno_PIR.ino` and upload it to your **Arduino Uno**.
3. Open `SecurityCam/SecurityCam.ino` in the Arduino IDE.
4. Install the **UniversalTelegramBot** and **ArduinoJson** libraries.
5. Replace the credentials in `SecurityCam.ino`:
   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *password = "YOUR_WIFI_PASSWORD";
   const char *BOT_TOKEN = "YOUR_TELEGRAM_BOT_TOKEN";
   const char *CHAT_ID = "YOUR_TELEGRAM_CHAT_ID";
   ```
6. Select **AI Thinker ESP32-CAM** in the Arduino Board Manager.
7. Upload the code to your **ESP32-CAM**.
8. Connect the wires as shown in the Pinout section and enjoy your handsome security!

## 📸 Example
*Motion Detected!*
📸 `[Intruder Photo]`
🚨 *"Dert değil anlasana yetti bunlar bana, Değişmem demiştin haline baksana 💅"*
*(Followed by a random Yakışıklı Güvenlik GIF)*
