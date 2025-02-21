#include <Wire.h>
#include <Servo.h>

Servo XServo;
Servo YServo;

int ADXL345 = 0x53;

const int length = 5;
int ReadingsX[length] = { 0 };
int currentIndexX = 0;
int sumX = 0;     

int ReadingsY[length] = { 0 };
int currentIndexY = 0;
int sumY = 0;     

float X_out, Y_out, Z_out;
float roll,pitch,rollF,pitchF=0;

void setup() {
  Serial.begin(9600);
 
  XServo.attach(6);
  YServo.attach(7);

  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);
  Wire.write(1);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F);
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(ADXL345);
  Wire.write(0x20);
  Wire.write(-9);
  Wire.endTransmission();
  delay(10);
}

void loop() {
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  X_out = ( Wire.read() | Wire.read() << 8);
  X_out = X_out / 256;
  Y_out = ( Wire.read() | Wire.read() << 8);
  Y_out = Y_out / 256;
  Z_out = ( Wire.read() | Wire.read() << 8);
  Z_out = Z_out / 256;

  roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
  pitch = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;

  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;

  ;

  Serial.print(rollF);
  Serial.print("/");
  Serial.println(pitchF);

  XServo.write(updateMeanX());
  YServo.write(updateMeanY());
}

int updateMeanX() {
  sumX -= ReadingsX[currentIndexX];

  ReadingsX[currentIndexX] = roll;
  sumX += ReadingsX[currentIndexX];

  currentIndexX = (currentIndexX + 1) % length;

  return sumX / length + 90;
}

int updateMeanY() {
  sumY -= ReadingsY[currentIndexY];

  ReadingsY[currentIndexY] = pitch;
  sumY += ReadingsY[currentIndexY];

  currentIndexY = (currentIndexY + 1) % length;

  return 180-(sumY / length + 90);
}