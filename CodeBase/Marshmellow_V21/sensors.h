#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

#define S0 9
#define S1 A0
#define S2 A2
#define S3 A4
#define S4 A6
#define S5 A8
#define S6 A10
#define S7 A12
#define S8 A14
#define S9 11

#define leftAngleIR      A3
#define leftStraightIR   A11
#define frontIR          A1
#define rightStraightIR  A5
#define rightAngleIR     A9

#define highProx 26

extern int sensorPins[10];
extern int sensorValues[8];
extern int sensorDigitalValues[10];

extern int irValues[8];
extern bool irBits[8]; 

extern bool leftCorner;
extern bool rightCorner;

extern Adafruit_TCS34725 tcs;

void setupIRSensors();
void setupDistanceSensors();
void setupColorSensor();
void readSensors();
void readArrowSensorsDigital();
int getDistance(int sharpIRPin);
int readColor();
void readSensorsDigital();

