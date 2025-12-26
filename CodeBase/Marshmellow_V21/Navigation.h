#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "MotorControl.h"
#include "SensorsTask.h"
#include <Arduino.h>

class Navigation {
public:
  Navigation(MotorControl &mc, SensorsTask &s);
  void update();
  void followLine(int speed);
  bool moveCells(int numCells);
  void moveForwardPID(long steps);
  void moveBackwardPID(long steps);
  void turnIntersection(char direction); // 'L', 'R', 'F'

  bool isAtIntersection();

private:
  MotorControl &motorControl;
  SensorsTask &sensors;

  // // PID Constants
  // float Kp = 0.5;
  // float Ki = 0.0;
  // float Kd = 0.1;

  float lastError;
  float integral;
};

#endif // NAVIGATION_H
