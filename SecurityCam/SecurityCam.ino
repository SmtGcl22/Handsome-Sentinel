#include <WiFi.h>
#include "esp_camera.h"
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

    // =========================================================================
    // KULLANICI AYARLARI
    // =========================================================================
    const char *ssid = "Arçura";     // WiFi Adınız
const char *password = "SMt22092003."; // WiFi Şifreniz
const char *BOT_TOKEN = "8640490663:AAGHrGjDuRA19GfvtgRUQ43b4rUVp9jb5D8";    // Telegram Bot Token'ınız
const char *CHAT_ID =
    "6377884612"; // Telegram Chat ID'niz (Size ait veya grubun ID'si)

#define PIR_PIN 13 // PIR Sensörünün bağlı olduğu GPIO pini
#define FLASH_PIN 4 // ESP32-CAM üzerindeki dahili flaş LED'inin GPIO pini

// =========================================================================
// AI-THINKER ESP32-CAM PIN TANIMLAMALARI
// =========================================================================
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// =========================================================================
// GLOBAL NESNELER
// =========================================================================
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Fotoğraf verisi gönderimi için kullanılan global değişkenler
camera_fb_t *fb = NULL;
uint8_t *jpeg_buf = NULL;
size_t jpeg_len = 0;
int currentByte = 0;

// UniversalTelegramBot'a veri akışının devam edip etmediğini bildirir
bool isMoreDataAvailable() {
  if (jpeg_buf == NULL)
    return false;
  return (currentByte < jpeg_len);
}

// UniversalTelegramBot'a gönderilecek bir sonraki byte'ı iletir
byte getNextByte() {
  byte b = jpeg_buf[currentByte];
  currentByte++;
  return b;
}

// =========================================================================
// KURULUM (SETUP)
// =========================================================================
void setup() {
  Serial.begin(115200);
  Serial.println();

  // PIR Sensör Pini ve Flaş Pini
  pinMode(PIR_PIN, INPUT_PULLDOWN); // Sensör takılı değilken veya boşta 'floating' (kararsız) olmasını engeller
  pinMode(FLASH_PIN, OUTPUT);
  digitalWrite(FLASH_PIN, LOW); // Başlangıçta flaşı kapalı tut

  // WiFi Bağlantısı
  Serial.print("WiFi'ye bağlanıyor: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(
      TELEGRAM_CERTIFICATE_ROOT); // Telegram SSL sertifikasını ayarla

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi bağlandı!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());

  // Kamera Konfigürasyonu
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  // Prompt gereksinimlerine göre ayarlar
  // RHYX M21-45 (GC2145) sensöründe donanımsal JPEG sıkıştırması yoktur.
  // Bu yüzden RAW (RGB565) formatında görüntü alıp yazılımsal olarak dönüştüreceğiz.
  config.pixel_format = PIXFORMAT_RGB565; 
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12; // Yazılımsal dönüştürme sırasında kullanılacak
  config.fb_count = 1;

  // Kamerayı başlat
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Kamera başlatılamadı, hata kodu: 0x%x\n", err);
    return;
  }

  Serial.println("Sistem hazır. Hareket bekleniyor...");
  bot.sendMessage(CHAT_ID,
                  "Güvenlik kamerası aktif edildi. Hareket bekleniyor...", "");
}

// =========================================================================
// YAKIŞIKLI GÜVENLİK SÖZLERİ (ŞİMDİLİK SADECE METİN)
// =========================================================================
const char* yakisikliSozler[] = {
  "Bitti bahaneler tükendi cümleler\nGüldüğüme bakma neler çektim ben neler 😎",
  "Kalbimde kırılmadık kalmadı ki bir yer\nHepsi hikaye sildim teker teker 💔",
  "Dert değil anlasana yetti bunlar bana\nDeğişmem demiştin haline baksana 💅",
  "Ah sana vah sana\nArada yazar bana\nİsmini kalbimden silmiştim oysa 🥀",
  "Ah sana yazık sana\nKıyamazdım sana ama\nFarkın yok senin de\nYürü yol bitmez sana 🚶‍♂️",
  "Gereksiz çabalar artık ne fark edеr\nŞimdi oldum erken\nBitmiyor ki dertlеr 🚬",
  "Kapandı perdeler gelmez o günler\nBizi bizden aldı yalan yanlış vaatler 🎭",
  "Hareket algılandı! Ama unutma:\nDert değil anlasana yetti bunlar bana! 🎶",
  "Yakışıklı Güvenlik Görevde! 😎🔫"
};
const int SOZ_SAYISI = 9;

// =========================================================================
// ANA DÖNGÜ (LOOP)
// =========================================================================
unsigned long lastPhotoTime = 0;
const unsigned long PHOTO_COOLDOWN = 30000; // 30 saniye bekleme süresi (Spam koruması)
const unsigned long CALIBRATION_TIME = 20000; // Başlangıçta PIR sensörünün kalibre olması için 20 sn bekleme

void loop() {
  unsigned long currentTime = millis();
  
  // Sensör kalibrasyonu bitmediyse (ilk açılışta sahte tetiklemeleri önler)
  if (currentTime < CALIBRATION_TIME) {
    return;
  }

  int pirState = digitalRead(PIR_PIN);

  // Hareket algılandıysa, ufak bir doğrulama yapalım (kablo teması/gürültü önlemi - debounce)
  if (pirState == HIGH) {
    delay(100); // 100ms bekle
    if (digitalRead(PIR_PIN) == LOW) {
      return; // Anlık bir gürültüyse (kablo teması vs.) yoksay
    }

    // Gerçek hareket var ve spam süresi dolmuşsa
    if (currentTime - lastPhotoTime >= PHOTO_COOLDOWN || lastPhotoTime == 0) {
      Serial.println("Gerçek hareket algılandı! Fotoğraf çekiliyor...");

    // Flaş LED'i yak
    digitalWrite(FLASH_PIN, HIGH);
    delay(500); // Kameranın ışığa adapte olması için kısa bir bekleme (Auto-Exposure)

    // 1) Fotoğrafı çek
    fb = esp_camera_fb_get();
    
    // Çekimden hemen sonra flaş LED'i kapat
    digitalWrite(FLASH_PIN, LOW);

    if (!fb) {
      Serial.println("Fotoğraf çekimi başarısız oldu!");
      return;
    }

    // GC2145 Modülü donanımsal JPEG desteklemediğinden, resmi yazılımsal olarak JPEG'e çeviriyoruz
    bool converted = false;
    if (fb->format != PIXFORMAT_JPEG) {
      Serial.println("RGB565 formatından JPEG'e dönüştürülüyor...");
      converted = frame2jpg(fb, 12, &jpeg_buf, &jpeg_len);
      if (!converted) {
        Serial.println("JPEG dönüştürme işlemi başarısız!");
        esp_camera_fb_return(fb);
        return;
      }
    } else {
      jpeg_buf = fb->buf;
      jpeg_len = fb->len;
    }

    // 2) Gönderim için global sayacı sıfırla
    currentByte = 0;

    // 3) Telegram'a fotoğrafı binary formatta gönder
    Serial.println("Telegram'a fotoğraf gönderiliyor...");
    String response = bot.sendPhotoByBinary(CHAT_ID, "image/jpeg", jpeg_len,
                                            isMoreDataAvailable, getNextByte,
                                            nullptr, nullptr);

    if (response == "") {
      Serial.println("Fotoğraf gönderimi başarısız.");
    } else {
      Serial.println("Fotoğraf başarıyla gönderildi!");
      
      // Yakışıklı Güvenlik konsepti - Rastgele söz veya link gönder
      int randomIndex = random(0, SOZ_SAYISI);
      bot.sendMessage(CHAT_ID, String("🚨 ") + yakisikliSozler[randomIndex], "");
      Serial.println("Yakışıklı Güvenlik mesajı gönderildi!");
      
      lastPhotoTime = currentTime; // Başarılı gönderimde son çekim zamanını kaydet
    }

    // 4) Buffer'ı temizle, bellek sızıntısını önle
    if (converted && jpeg_buf != NULL) {
      free(jpeg_buf);
      jpeg_buf = NULL;
    }
    esp_camera_fb_return(fb);
    fb = NULL; // Güvenlik için pointerı boşa al

    Serial.println("Spam koruması aktif. 30 saniye boyunca yeni fotoğraf gönderilmeyecek...");
    } // End of if (currentTime - lastPhotoTime >= PHOTO_COOLDOWN || lastPhotoTime == 0)
  } // End of if (pirState == HIGH)
} // End of loop()
