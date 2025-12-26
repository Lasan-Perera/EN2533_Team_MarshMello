#include "helperFunctions.h"
#include "sensors.h"
#include "motors.h"
#include "lineFollow.h"

int weights[8] = {-35, -25, -15, -5, +5, +15, +25, +35};

bool isFourWay() {
  readSensors();
  int countWhite = 0;
  for (int i = 0; i < 8; i++) {
    if (sensorValues[i] > 250) return false;
  }
  return true;  
}

float getLinePosition() {
  long sumWeighted = 0;
  long sumValues = 0;
  for (int i = 0; i < 8; i++) {
    int value = 1023 - sensorValues[i];  // white line Black bg
    sumWeighted += (long)value * weights[i];
    sumValues   += value;
  }
  if (sumValues == 0) return 0; //error handling
  return sumWeighted / (float)sumValues;
}

void gofow(int intersections){
  if (intersections>0){
    int num_intersection =0;
    while (intersections > num_intersection){
      lineFollow(3.9, 0, 1.85);
      // delay(2);
      if (isFourWay()){
        num_intersection++;
        while(isFourWay()){
          lineFollow(3.9, 0, 1.85);
          // delay(2);
        }
      }
    }
  }else{
    forward(-300);
    gofow(1);
  }
}