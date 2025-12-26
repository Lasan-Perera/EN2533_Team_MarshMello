// follow the dash line,push the ramp and then come back
// then go up the ramp and then allign to the first cross and to the second cross

#include "dashLine.h"
#include "motors.h"
#include "helperFunctions.h"
#include "lineFollow.h"
#include "sensors.h"

// const int LEFT_IR = 9;
// const int RIGHT_IR = 11;

// bool leftCorner = 0;
// bool rightCorner = 0;
float lastError = 0;


// int irValues[8];
// bool irBits[8];
int weight_[8] = { -7, -4, -2, 0, 0, 2, 4, 7 };
void dashedLineFollow(float kp_, float kd_);

void dashLineXramp() {
  while (!isFourWay()) {
    dashedLineFollow(100, 100);
  }
  stopRobot();
  delay(100);
  gofow(-1);
  delay(100);
  stopRobot();
  forward(1000);
  stopRobot();
  delay(200);
  forward(-10);
  right90();
  right90();
  while (!isFourWay()) {
    applyMotorSpeed(0);
  }
  forward(200);
  while (!isFourWay()) {
     dashedLineFollow(100, 100);
  }
  forward(-600);
  right90();
  while (!isFourWay()) {
    applyMotorSpeed_slow(0);
  }
  stopRobot();
  forward(550);
  stopRobot();
}


void dashedLineFollow(float kp_, float kd_) {
  while (true) {
    readSensorsDigital();
    int sum = 0;
    int total = 0;

    for (int i = 0; i < 8; i++) {
      sum += irBits[i] * weight_[i];
      total += irBits[i];
    }
    float error;
    if (leftCorner) {
      if (rightCorner) {
        return;
      } else {
        error = -5;
      }
    } else if (rightCorner) {
      if (leftCorner) {
        return;
      } else {
        error = 5;
      }
    } else {
      if (total == 0) {
        applyMotorSpeed(0);
      } else {
        error = (float)sum / total;
      }
    }
    float correction_ = (kp_ * error + kd_ * (error - lastError)) * 5;
    lastError = error;
    Serial.println(correction_);
    applyMotorSpeed(correction_);
  }
}

// No need for now but if we need to change the speed and the steps we need this

// void forward_speed(int steps, int speed = 300) {
//   digitalWrite(DirPinL, HIGH);
//   digitalWrite(DirPinR, HIGH);

//   stepperL.setSpeed(speed);
//   stepperR.setSpeed(speed);

//   stepperL.moveTo(stepperL.currentPosition() + steps);
//   stepperR.moveTo(stepperR.currentPosition() + steps);

//   while (stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
//     stepperL.runSpeedToPosition();
//     stepperR.runSpeedToPosition();
//   }
// }
