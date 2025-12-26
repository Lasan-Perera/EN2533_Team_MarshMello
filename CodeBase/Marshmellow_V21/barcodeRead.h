#pragma once
#include <Arduino.h>

// Barcode detection constants
#define MIN_LINE_DURATION_STEPS 10

// Barcode variables
extern long lineDurations[5];
extern int linesDetected;
extern int sphereCount;

// Function declarations
bool isIntersection(int *sensorValues_);
void barcodeUpdate(int *sensorValues_, long currentSteps);
bool barcodeFinished();
int decodeBarcode();
int getSphereCount();
void lineFollowBarcode();
