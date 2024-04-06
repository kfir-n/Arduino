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
#define WIFI_SSID     "Kfirs Phone"
#define WIFI_PASSWORD "Kfir@326158813"

// Firebase credentials
#define API_KEY       "AIzaSyCVEjwukELHMRqnj6yjIvmQcDVdPtR5zD4"
#define DATABASE_URL  "https://finalproj155-default-rtdb.firebaseio.com/"
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
int intValue = 0;
int maxNumber = 1000;

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
void setup(){
  Serial.begin(115200);
   Wire.begin();
  mpu.initialize();
  mpu2.initialize();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 300 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();


    if (Firebase.RTDB.getInt(&fbdo, "/isStart")) {
      if(fbdo.dataType()=="int"){
        intValue = fbdo.intData();
        if(intValue == 1){
       delay(2000); // Delay for 2 seconds for readability
  
  // Read data from MPU1
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.X = map(ax, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255); 
  data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data
  
  // Print data from MPU1
  Serial.print("Axis X 1 = ");
  Serial.println(data.X);

  // Read data from MPU2
  mpu2.getMotion6(&ax2, &ay2, &az2, &gx2, &gy2, &gz2);
  data.X = map(ax2, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay2, -17000, 17000, 0, 255); 
  data.Z = map(az2, -17000, 17000, 0, 255);  // Y axis data

  // Print data from MPU2
  Serial.print("Axis X 2 = ");
  Serial.println(data.X);

  // Calculate the difference of X values
  int16_t diffX = ax2 - ax; 
  data.X = map(diffX, -34000, 34000, 0, 255); 

  // Print the difference of X values
  Serial.print("Difference of X values: ");
  Serial.println(data.X);
if(maxNumber > data.X){
  maxNumber = data.X;

if(Firebase.RTDB.setInt(&fbdo,"/angle" , data.X)){
  Serial.print("Difference of X values: ");
  Serial.println(data.X);
}
}

        }
      }
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());





  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  }
}


