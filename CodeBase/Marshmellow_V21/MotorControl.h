#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "RobotConfig.h"
#include <AccelStepper.h>

class MotorControl {
public:
  MotorControl();
  void init();
  void moveForward(long steps);
  void moveBackward(long steps);
  void turnLeft(float degrees);
  void turnRight(float degrees);
  void stop();
  void run();      // Must be called in loop
  void runSpeed(); // For PID speed control
  void setLeftSpeed(float speed);
  void setRightSpeed(float speed);
  void applyMotorSpeed(float correction, int speed); // New method for PID
  bool isRunning();
  void moveForwardVertex(int intersections);
  long getCurrentPosition();
  void enableMotors();
  void disableMotors();
  long cmToSteps(float cm);

private:
  const float STEPS_PER_CM = STEPS_PER_REV / (WHEEL_DIAMETER_CM * PI);
  AccelStepper leftStepper;
  AccelStepper rightStepper;
  long degreesToSteps(float degrees);
};

#endif // MOTOR_CONTROL_H
