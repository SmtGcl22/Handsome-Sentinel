// =========================================================================
// ARDUINO UNO - PIR SENSÖR KONTROLCÜSÜ
// =========================================================================c:\Users\samet\OneDrive\Desktop\23.png
// Bu kod PIR sensörünü okur, sahte parazitleri engeller ve 
// geçerli bir hareket algılandığında ESP32-CAM'e temiz bir tetik sinyali gönderir.

#define PIR_PIN 2         // PIR Sensörünün Data (OUT) pini
#define TRIGGER_PIN 3     // ESP32-CAM'e gidecek sinyal pini (ESP32 GPIO 13'e bağlanacak)

const unsigned long COOLDOWN_TIME = 10000; // 10 saniye spam koruması (bekleme süresi)
unsigned long lastTriggerTime = 0;
unsigned long lastPrintTime = 0;
int lastPirState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  
  // ÖNEMLİ: ESP32 3.3V ile çalıştığı için Arduino'nun doğrudan 5V vermesini engellemeliyiz!
  // Bunun için "Active LOW" (Açık Drenaj) mantığını kullanıyoruz.
  // Pin normalde INPUT (yüksek empedans) kalacak. Tetikleme anında OUTPUT ve LOW yapılacak.
  pinMode(TRIGGER_PIN, INPUT); 
  
  Serial.println("Arduino PIR Kontrolcusu Basladi.");
  Serial.println("Sensorun isinmasi bekleniyor...");
  
  // PIR sensörünün ilk açılışta kararlı hale gelmesi için bekle
  for (int i = 15; i > 0; i--) {
    Serial.print(i);
    Serial.println(" saniye kaldi...");
    delay(1000);
  }
  Serial.println("Sistem Hazir! Hareket bekleniyor.");
}

void loop() {
  unsigned long currentTime = millis();
  int pirState = digitalRead(PIR_PIN);

  // --- SAYAÇ YAZDIRMA MANTIĞI ---
  // Eğer bekleme (cooldown) süresindeysek, her saniye geriye sayımı yazdır
  if (lastTriggerTime != 0 && (currentTime - lastTriggerTime) < COOLDOWN_TIME) {
    if (currentTime - lastPrintTime >= 1000) {
      int remaining = (COOLDOWN_TIME - (currentTime - lastTriggerTime)) / 1000;
      Serial.print("Bekleme suresi... Kalan: ");
      Serial.print(remaining);
      Serial.println(" sn");
      lastPrintTime = currentTime;
    }
  } else if (lastTriggerTime != 0 && lastPrintTime != 0) {
    // Bekleme süresi bittiğinde ekranı temizle (30 satır boşluk)
    for(int i=0; i<30; i++) {
      Serial.println();
    }
    Serial.println("====================================================");
    Serial.println(">>> Bekleme suresi doldu! Yeni hareketlere acik. <<<");
    Serial.println("====================================================");
    lastPrintTime = 0; // Bir daha yazdırmaması için sıfırla
  }

  // --- HAREKET ALGILAMA MANTIĞI ---
  // PIR sensörü LOW'dan HIGH'a geçtiğinde (yeni bir hareket başlangıcı)
  if (pirState == HIGH && lastPirState == LOW) {
    
    // Ufak bir gecikme ile sinyalin parazit olup olmadığını teyit et (Debounce)
    delay(100);
    if (digitalRead(PIR_PIN) == HIGH) {
      
      // Bekleme (cooldown) süresi dolmuş mu kontrol et
      if (currentTime - lastTriggerTime >= COOLDOWN_TIME || lastTriggerTime == 0) {
        Serial.println("\n[!] Gercek hareket algilandi!");
        Serial.println("ESP32'ye tetik sinyali gonderiliyor...");
        
        // ESP32'yi tetikle (Pini OUTPUT yapıp LOW'a çekiyoruz - Güvenli 3.3V iletişimi)
        pinMode(TRIGGER_PIN, OUTPUT);
        digitalWrite(TRIGGER_PIN, LOW);
        
        delay(200); // 200 milisaniye boyunca LOW sinyali gönder
        
        // Tetikleme bitti, pini tekrar serbest bırak (INPUT yapınca ESP32 iç pull-up ile HIGH okuyacak)
        pinMode(TRIGGER_PIN, INPUT);
        
        lastTriggerTime = millis();
        lastPrintTime = millis(); // Sayacı başlatmak için referans zamanı
      }
    }
  }

  // (Gereksiz spam olmaması için LOW'a düşme mesajı kaldırıldı)

  lastPirState = pirState;
  delay(50); // Döngü dengeleyici
}
