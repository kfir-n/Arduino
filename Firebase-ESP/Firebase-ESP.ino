#include <Arduino.h>
#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"    

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// WiFi credentials
#define WIFI_SSID     "Kfirs Phone"
#define WIFI_PASSWORD "Kfir@326158813"

// Firebase credentials
#define API_KEY       "AIzaSyCVEjwukELHMRqnj6yjIvmQcDVdPtR5zD4"
#define DATABASE_URL  "https://finalproj155-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

MPU6050 mpu2;
int16_t ax2, ay2, az2;
int16_t gx2, gy2, gz2;

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

MyData data;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  mpu.initialize();
  mpu2.initialize();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected with IP: ");
  Serial.println(WiFi.localIP());
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up OK");
    signupOK = true;
  } else {
    Serial.println("Firebase sign-up failed: " + config.signer.signupError.message);
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  delay(2000); // Delay for 2 seconds for readability
  
  // Read and print data from both MPU6050 sensors
  readAndPrintSensorData(mpu, "1");
  readAndPrintSensorData(mpu2, "2");
  
  // Example Firebase data send (adapted from the first script)
  if (signupOK && Firebase.ready() && millis() - sendDataPrevMillis > 300) {
    sendDataPrevMillis = millis();
    // Replace "test/float" with your actual Firebase path
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", data.X)) {
      Serial.println("Data sent to Firebase successfully.");
    } else {
      Serial.println("Failed to send data to Firebase: " + fbdo.errorReason());
    }
  }
}

void readAndPrintSensorData(MPU6050 &sensor, const String &sensorNumber) {
  int16_t ax, ay, az, gx, gy, gz;
  sensor.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  byte x = map(ax, -17000, 17000, 0, 255);
  byte y = map(ay, -17000, 17000, 0, 255);
  byte z = map(az, -17000, 17000, 0, 255);

  Serial.print("Axis X ");
  Serial.print(sensorNumber);
  Serial.print(" = ");
  Serial.println(x);
}