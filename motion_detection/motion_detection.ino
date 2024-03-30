#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"    

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

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
  mpu2.initialize();
  //pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.X = map(ax, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255); 
  data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data
  delay(500);
  Serial.print("Axis X = ");
  Serial.print(data.X);
  Serial.print("  ");
  Serial.print("Axis Y = ");
  Serial.print(data.Y);
  Serial.print("  ");
  Serial.print("Axis Z  = ");
  Serial.println(data.Z);


  mpu2.getMotion6(&ax2, &ay2, &az2, &gx2, &gy2, &gz2);
  data.X = map(ax2, -17000, 17000, 0, 255 ); // X axis data
  data.Y = map(ay2, -17000, 17000, 0, 255); 
  data.Z = map(az2, -17000, 17000, 0, 255);  // Y axis data
  delay(500);
  Serial.print("Axis X 2= ");
  Serial.print(data.X);
  Serial.print("  ");
  Serial.print("Axis Y 2= ");
  Serial.print(data.Y);
  Serial.print("  ");
  Serial.print("Axis Z 2 = ");
  Serial.println(data.Z);
}
