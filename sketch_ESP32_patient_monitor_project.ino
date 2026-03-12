#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiMulti.h> // المكتبة الجديدة المسؤولة عن تعدد الشبكات
#include <HTTPClient.h>

WiFiMulti wifiMulti; // إنشاء كائن لإدارة الشبكات

// ================= إعدادات منصة ThingSpeak =================
String apiKey = "JSHDRIHNB9SATCYD"; 

// ================= الإعدادات والمنافذ =================
LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int trigPin = 32;
const int echoPin = 26;
const int gasPin = 36;
const int greenLed = 18;
const int redLed = 19;
const int buzzerPin = 5; // <--- تم إضافة الجرس على المنفذ 5

// ================= حدود الخطر =================
const float TEMP_LIMIT = 35.0;    
const float HUM_LIMIT  = 80.0;    
const int GAS_LIMIT = 2000;       
const int BED_SAFE_DISTANCE = 90; 

// مؤقت السحابة
unsigned long lastTime = 0;
unsigned long timerDelay = 15000; // إرسال البيانات كل 15 ثانية

void setup() {
  Serial.begin(115200);
  Wire.begin(16, 17); 
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT); // <--- تعريف الجرس كمخرج للطاقة

  // الحالة الابتدائية (طبيعي)
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(buzzerPin, LOW); // <--- التأكد من إطفاء الجرس عند بدء التشغيل

  lcd.init();
  lcd.backlight();
  dht.begin();
  
  // ================= 1. إضافة الشبكات هنا =================
  // الشبكة الأولى (مثلاً: شبكة راوتر المنزل)
  wifiMulti.addAP("Tenda_00D370", "MGS20013Z"); 
  
  // الشبكة الثانية (مثلاً: نقطة اتصال الهاتف البث الشخصي)
  wifiMulti.addAP("realme 7 Pro", "200120012001");
  // =========================================================

  lcd.setCursor(0, 0);
  lcd.print("Connecting Wi-Fi");
  
  // النظام سيبحث الآن عن أي شبكة متاحة من القائمة للاتصال بها
  while(wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connected!");
  delay(2000);
  lcd.clear();
}

void loop() {
  // --------- قراءة جميع الحساسات ---------
  float t = dht.readTemperature();
  float h = dht.readHumidity();       
  int gasValue = analogRead(gasPin);

  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); 
  int distance = duration * 0.034 / 2;

  // --------- المنطق الذكي والإنذار ---------
  bool isDanger = false;
  if (t > TEMP_LIMIT || h > HUM_LIMIT || gasValue > GAS_LIMIT || (distance > BED_SAFE_DISTANCE && distance > 0)) {
    isDanger = true;
  }

  // --------- التحكم بالمصابيح (LEDs) والجرس (Buzzer) ---------
  if (isDanger) {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH); 
    digitalWrite(buzzerPin, HIGH); // <--- تشغيل الجرس عند الخطر
  } else {
    digitalWrite(greenLed, HIGH); 
    digitalWrite(redLed, LOW);
    digitalWrite(buzzerPin, LOW);  // <--- إطفاء الجرس في الحالة الطبيعية
  }

  // --------- عرض البيانات على الشاشة (16x2) ---------
  lcd.setCursor(0, 0);
  if (isnan(t) || isnan(h)) {
    lcd.print("Sensor Error!   ");
  } else {
    lcd.print("T:"); lcd.print((int)t);
    lcd.print(" H:"); lcd.print((int)h);
    lcd.print(" G:"); lcd.print(gasValue);
    lcd.print("   "); 
  }

  lcd.setCursor(0, 1);
  lcd.print("D:");
  if (distance == 0 || distance > 400) {
    lcd.print("Err ");
  } else {
    lcd.print(distance); lcd.print("cm ");
  }
  
  lcd.print("S:");
  if (isDanger) lcd.print("ALARM!"); 
  else lcd.print("SAFE  "); 
  
  // --------- إرسال البيانات إلى السحابة (ThingSpeak) ---------
  if ((millis() - lastTime) > timerDelay) {
    // التأكد من أن الإنترنت لا يزال متصلاً (حتى لو تم التبديل بين الشبكتين)
    if(wifiMulti.run() == WL_CONNECTED){
      HTTPClient http;
      String serverPath = "http://api.thingspeak.com/update?api_key=" + apiKey + 
                          "&field1=" + String(t) + 
                          "&field2=" + String(h) + 
                          "&field3=" + String(gasValue) + 
                          "&field4=" + String(distance);
      
      http.begin(serverPath.c_str());
      int httpResponseCode = http.GET();
      
      if (httpResponseCode > 0) {
        Serial.print("Data Sent to Cloud. Code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error sending data: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    lastTime = millis();
  }

  delay(500); 
}