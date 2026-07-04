# 🕴️ Handsome-Sentinel (Yakışıklı Güvenlik Cam)

An ESP32-CAM based Telegram security camera with a twist of Turkish meme culture! 

Whenever motion is detected, this camera not only sends you a photo of the intruder but also serves them some fierce lines and GIFs from the famous "Yakışıklı Güvenlik" (Handsome Security) meme.

## 🌟 Features
- **PIR Motion Detection**: Uses an HC-SR501 PIR sensor to detect motion.
- **Edge Detection (Kenar Algılama)**: Only triggers on the initial motion edge (LOW to HIGH), completely preventing spam triggers caused by the PIR sensor holding a HIGH state.
- **Smart Calibration**: Ignores the sensor's unstable boot-up signals for the first 15-20 seconds.
- **Spam Protection**: Enforces a strict 5-10 second cooldown between captures so your Telegram inbox doesn't explode, even if network errors occur.
- **GC2145 Sensor Support**: Specifically adapted for the RHYX M21-45 (GC2145) camera module (which lacks hardware JPEG compression) by capturing raw RGB565 and software-encoding it to JPEG.
- **Meme Responses & GIFs**: Replies to intrusions with randomly selected lyrics and GIFs from "Yakışıklı Güvenlik"!

## 🛠️ Hardware Requirements
- ESP32-CAM Board (AI-Thinker footprint)
- RHYX M21-45 (GC2145) Camera Module (or standard OV2640)
- HC-SR501 PIR Motion Sensor
- FTDI Programmer or ESP32-CAM-MB Shield (for flashing)

## ⚡ Pinout
| ESP32-CAM Pin | Component |
|---|---|
| GPIO 3 (U0R) | PIR Sensor Data Pin |
| 5V / GND | PIR Sensor Power |

## 🚀 Setup
1. Clone the repository.
2. Open `SecurityCam/SecurityCam.ino` in the Arduino IDE.
3. Install the **UniversalTelegramBot** and **ArduinoJson** libraries.
4. Replace the credentials:
   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *password = "YOUR_WIFI_PASSWORD";
   const char *BOT_TOKEN = "YOUR_TELEGRAM_BOT_TOKEN";
   const char *CHAT_ID = "YOUR_TELEGRAM_CHAT_ID";
   ```
5. Select **AI Thinker ESP32-CAM** in the Arduino Board Manager.
6. Upload the code and enjoy your handsome security!

## 📸 Example
*Motion Detected!*
📸 `[Intruder Photo]`
🚨 *"Dert değil anlasana yetti bunlar bana, Değişmem demiştin haline baksana 💅"*
*(Followed by a random Yakışıklı Güvenlik GIF)*
