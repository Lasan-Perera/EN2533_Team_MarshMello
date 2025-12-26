#ifndef ARM_CONTROL_H
#define ARM_CONTROL_H

#include "RobotConfig.h"
#include "SensorsTask.h"
#include <Servo.h>

class ArmControlTask {
public:
  ArmControlTask();
  void init();
  void enable();
  void disable();
  void pickupBox();
  void preparePickup();
  void dropBox();
  void home();
  void run(); // Wait for completion

private:
  struct ServoMotion {
    Servo *s;
    int target;
    int speed; // ms per step
    unsigned long lastStep = 0;
    bool active = false;
  };

  Servo servoBig;    // Arm
  Servo servoSmall1; // Gripper 1
  Servo servoSmall2; // Gripper 2

  ServoMotion armMotion;
  ServoMotion grab1Motion;
  ServoMotion grab2Motion;

  void startMove(ServoMotion &m, int target, int speed);
  void updateMove(ServoMotion &m);

  void grabberAngle(int angle);
  void armAngle(int angle);
};

#endif // ARM_CONTROL_H
