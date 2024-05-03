#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// WiFi credentials
#define WIFI_SSID "Kfirs Phone"
#define WIFI_PASSWORD "Kfir@326158813"

// Firebase credentials
#define API_KEY "AIzaSyCVEjwukELHMRqnj6yjIvmQcDVdPtR5zD4"
#define DATABASE_URL "https://finalproj155-default-rtdb.firebaseio.com/"
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
int intValue = 0;
int maxNumber = 0;

MPU6050 mpu(0x68);   // Assuming default I2C address of the first MPU6050
MPU6050 mpu2(0x69);  // Custom I2C address for the second MPU6050 if needed

int16_t ax, ay, az;
int16_t gx, gy, gz;

int16_t ax2, ay2, az2;
int16_t gx2, gy2, gz2;

void setup() {


  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  mpu2.initialize();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning I2C bus...");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found\n");
  else Serial.println("done\n");

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 300 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    maxNumber = 0;

    if (Firebase.RTDB.getInt(&fbdo, "/isStart")) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();
        if (intValue == 1) {
          delay(2000);

          // Read data from MPU1
          mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
          int x1 = map(ax, -17000, 17000, 0, 255);  // Mapping accelerometer x-axis data for MPU1
          Serial.print("Axis X 1 = ");
          Serial.println(x1);  // Print mapped X-axis value for MPU1

          // Read data from MPU2
          mpu2.getMotion6(&ax2, &ay2, &az2, &gx2, &gy2, &gz2);
          int x2 = 180;
          Serial.print("Axis X 2 = ");
          Serial.println(x2);  // Print mapped X-axis value for MPU2
          int diffX = x1 - x2;
         Serial.print("Difference of X values: ");
          Serial.println(diffX);

          // Check if the difference is positive
          if (diffX > 0) {
            // Update the maximum difference found, if current diffX is greater
            if (diffX > maxNumber) {
              maxNumber = diffX;  // Update maxNumber with the new maximum
              // Attempt to store this new maximum in Firebase
              if (Firebase.RTDB.setInt(&fbdo, "/angle", maxNumber)) {
                Serial.print("Stored new max difference of X values: ");
                Serial.println(maxNumber);
              }
            }
          } else {
            // Handle non-positive diffX values
            Serial.print("Error: Difference is not positive, diffX = ");
            Serial.println(diffX);
          }
        }
      }

    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
