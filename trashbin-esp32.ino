#include <WiFi.h>
#include <HTTPClient.h>
#include <Servo.h>

// WiFi dan Telegram
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* botToken = "TOKEN_BOT_TELEGRAM";
const char* chatID = "CHAT_ID_KAMU";

// HC-SR04 pin
#define trigPin1 23
#define echoPin1 22
#define trigPin2 21
#define echoPin2 19

// LED
#define led1 18
#define led2 5

// Servo
#define servoPin 17
Servo servo;

long duration1, distance1;
long duration2, distance2;

void setup() {
  Serial.begin(115200);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  servo.attach(servoPin);
  servo.write(0); // Tutup

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void loop() {
  // Sensor depan
  distance1 = readDistance(trigPin1, echoPin1);
  if (distance1 <= 10) {
    digitalWrite(led1, HIGH);
    servo.write(90); // buka tutup
    delay(3000);
    servo.write(0);  // tutup
  } else {
    digitalWrite(led1, LOW);
  }

  // Sensor dalam
  distance2 = readDistance(trigPin2, echoPin2);
  if (distance2 <= 30) {
    digitalWrite(led2, HIGH);
    sendTelegram("Sampah dalam tong sudah penuh!");
    delay(10000); // anti spam
  } else {
    digitalWrite(led2, LOW);
  }

  delay(1000);
}

long readDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH) * 0.034 / 2;
}

void sendTelegram(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + String(botToken) +
                 "/sendMessage?chat_id=" + String(chatID) +
                 "&text=" + message;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Telegram sent");
    } else {
      Serial.println("Failed to send");
    }
    http.end();
  }
}