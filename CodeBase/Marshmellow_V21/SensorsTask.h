#ifndef SENSORS_TASK_H
#define SENSORS_TASK_H

#include "Adafruit_TCS34725.h"
#include "RobotConfig.h"
#include <Arduino.h>
#include <Wire.h>

enum Color { COLOR_NONE, COLOR_RED, COLOR_GREEN, COLOR_BLUE };

class SensorsTask {
public:
  SensorsTask();
  void init();

  // Line Sensor
  float getLinePosition(); // Returns weighted position
  bool isFourWay();        // Returns true if intersection detected
  void printDebugValues(); // Print raw sensor values

  // Obstacle SensorsTask
  float getDistance(int sharpIRPin);
  bool isObstacleFront();
  bool isObstacleLeft();
  bool isObstacleRight();
  float getObstacleDistance(); // From height sensor

  // Color Sensor
  Color readColor();

private:
  Adafruit_TCS34725 tcs;
  int readSharpIR(uint8_t pin);

  // Lookup Table for Sharp IR (0-1023 analog value -> cm)
  uint8_t distanceLUT[1024];
};

#endif // SENSORS_H
