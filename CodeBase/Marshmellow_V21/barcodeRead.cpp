#include "barcodeRead.h"
#include "helperFunctions.h"
#include "sensors.h"
#include "motors.h"
#include "lineFollow.h"

// Barcode variables initialization
long lineDurations[5] = {0};
int linesDetected = 0;
int sphereCount = 0;


bool isIntersection(int *sensorValues_) {
  return (sensorValues_[0] > 250 && sensorValues_[7] > 250);
}

void barcodeUpdate(int *sensorValues_, long currentSteps) {
  static bool onLine = false;  
  static long startSteps = 0;   

  bool hit = isIntersection(sensorValues_);  

  if (!onLine && !hit) {
    Serial.println("a hit");
    onLine = true;
    startSteps = currentSteps;
  }

  else if (onLine && hit) {
    long dur = currentSteps - startSteps;  

    if (dur > MIN_LINE_DURATION_STEPS) {
      lineDurations[linesDetected++] = dur;
    }

    onLine = false;
  }
}


bool barcodeFinished() {
  return (linesDetected >= 5);
}


int decodeBarcode() {
  long minD = 999999, maxD = 0;

  for (int i = 0; i < 5; i++) {
    if (lineDurations[i] < minD) minD = lineDurations[i];
    if (lineDurations[i] > maxD) maxD = lineDurations[i];
  }

  long threshold = (minD + maxD) / 2;  

  int value = 0;

  for (int i = 0; i < 5; i++) {
    int bit = (lineDurations[i] > threshold) ? 1 : 0;
    Serial.print(lineDurations[i]);
    Serial.print(" ");
    value = (value << 1) | bit;       
  }
  Serial.println();

  int r = value % 3;
  return (r == 0) ? 2 : (r == 1 ? 3 : 4);
}


int getSphereCount() {
  if (!barcodeFinished()) {
    Serial.println(0);
    return 0;
  }

  int val = decodeBarcode();
  Serial.println(val);
  return val;
}


void lineFollowBarcode() {
  lineFollow90Bends(3.9, 0, 1.85);
  readSensors();
  if (!barcodeFinished()){
    barcodeUpdate(sensorValues, stepperR.currentPosition());
  }
  else {
    sphereCount = getSphereCount();
  }
}

