#include "sensors.h"

int sensorPins[10] = {S0, S1, S2, S3, S4, S5, S6, S7, S8, S9};
int sensorValues[8];
int sensorDigitalValues[10];
const int DIGITAL_THRESHOLD = 85;

int irValues[8];
bool irBits[8]; 

bool leftCorner = 0;
bool rightCorner = 0;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_4X);

void setupIRSensors() {
  pinMode(S0, INPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(S6, INPUT);
  pinMode(S7, INPUT);
  pinMode(S8, INPUT);
  pinMode(S9, INPUT);
}

void setupDistanceSensors() {
  pinMode(leftAngleIR, INPUT);
  pinMode(leftStraightIR, INPUT);
  pinMode(rightAngleIR, INPUT);
  pinMode(frontIR, INPUT);
  pinMode(rightStraightIR, INPUT);
  pinMode(highProx, INPUT);
}

void setupColorSensor() {
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // Halt operations
  }
}

void readSensors() {
  for (int i = 1; i < 9; i++) {
    sensorValues[i - 1] = analogRead(sensorPins[i]);
  }
}

void readSensorsDigital() {
  for (int i = 1; i < 9; i++) {
    irValues[i-1] = analogRead(sensorPins[i]);
    irBits[i-1] = (irValues[i-1] < 250) ? 1 : 0;  
  }

  // read corner sensors
  leftCorner  = !digitalRead(9);   // ACTIVE LOW
  rightCorner = !digitalRead(11);  // ACTIVE LOW
}

void readArrowSensorsDigital() {
  for (int i = 1; i <= 8; i++) {
    int value = analogRead(sensorPins[i]);
    sensorDigitalValues[i] = (value < DIGITAL_THRESHOLD) ? 0 : 1;
  }
  sensorDigitalValues[0] = (digitalRead(sensorPins[0]) == LOW) ? 0 : 1;
  sensorDigitalValues[9] = (digitalRead(sensorPins[9]) == LOW) ? 0 : 1;
}

int getDistance(int sharpIRPin) {
  int val = analogRead(sharpIRPin);
  if (val < 10)
    return 999; 

  float volts = val * (5.0 / 1023.0);
  return (int)(5.2819 * pow(volts, -1.161));
}

int readColor() {
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  if ( r > g && r > b){
    Serial.println("Red");
    return 0; //red
  }
  else if (g > r && g > b){
    Serial.println("Green");
    return 1; //Green
  }
  else if ( b > g && b > r){
    Serial.println("Blue");
    return 2; //Blue
  }
  delay(100);
}
