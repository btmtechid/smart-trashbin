#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Servo.h>

// WiFi dan Telegram
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* botToken = "TOKEN_BOT_TELEGRAM";
const char* chatID = "CHAT_ID_KAMU";

// HC-SR04
#define trigPin1 D1
#define echoPin1 D2
#define trigPin2 D5
#define echoPin2 D6

// LED
#define led1 D7
#define led2 D8

// Servo
#define servoPin D3
Servo servo;

// Sensor variabel
long duration1, distance1;
long duration2, distance2;

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  servo.attach(servoPin);
  servo.write(0); // posisi tutup

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setInsecure(); // untuk HTTPS
  Serial.println("WiFi connected");
}

void loop() {
  // Sensor Depan
  distance1 = readDistance(trigPin1, echoPin1);
  if (distance1 <= 10) {
    digitalWrite(led1, HIGH);
    servo.write(90); // buka tutup
    delay(3000);
    servo.write(0); // tutup lagi
  } else {
    digitalWrite(led1, LOW);
  }

  // Sensor Dalam
  distance2 = readDistance(trigPin2, echoPin2);
  if (distance2 <= 30) {
    digitalWrite(led2, HIGH);
    sendTelegram("Sampah dalam tong sudah penuh!");
    delay(10000); // Tunggu 10 detik agar tidak spam
  } else {
    digitalWrite(led2, LOW);
  }

  delay(1000);
}

long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

void sendTelegram(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;
    String url = "https://api.telegram.org/bot" + String(botToken) +
                 "/sendMessage?chat_id=" + String(chatID) +
                 "&text=" + message;
    https.begin(client, url);
    int httpCode = https.GET();
    if (httpCode > 0) {
      Serial.println("Telegram sent");
    } else {
      Serial.println("Error sending Telegram");
    }
    https.end();
  }
}