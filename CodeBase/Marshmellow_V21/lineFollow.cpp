#include "helperFunctions.h"
#include "sensors.h"
#include "motors.h"

// PID variables initialization
float error = 0;
float previousError = 0;
float integral = 0;
float derivative = 0;
float correction = 0;

void resetPID();

void lineFollow(float Kp, float Ki, float Kd) {
  readSensors();
  float position = getLinePosition();
  error = position;
  integral += error;
  derivative = error - previousError;
  correction = 10*(Kp * error + Ki * integral + Kd * derivative);
  previousError = error;
  applyMotorSpeed(correction);
}


void wallFollow(float Kp, float Ki, float Kd, float targetDistance) {
  int straightReading = analogRead(rightStraightIR);
  int angleReading = analogRead(rightAngleIR);
  
  float distance = (straightReading + angleReading) / 2.0;
  error = -(targetDistance - distance);
  
  integral += error;
  derivative = error - previousError;
  correction = Kp * error + Ki * integral + Kd * derivative;
  previousError = error;
  // Serial.println(correction);
  applyMotorSpeed(correction);
}


// void doubleWallFollow(float Kp, float Ki, float Kd){
//   int straightRightReading = analogRead(rightStraightIR);
//   int angleRightReading = analogRead(rightAngleIR);

//   int straightLeftReading = analogRead(rightStraightIR);
//   int angleLeftReading = analogRead(rightAngleIR);

//   float rightDistance = (straightRightReading + angleRightReading) / 2.0;
//   float leftDistance = (straightLeftReading + angleLeftReadingg) / 2.0;

//   float error = rightDistance - leftDistance;

//   integral += error;
//   float derivative = error - previousError;
//   float correction = Kp * error + Ki * integral + Kd * derivative;
//   previousError = error;

//   applyMotorSpeed(correction);
// }


void lineFollow90Bends(float Kp, float Ki, float Kd) {
  lineFollow(Kp, Ki, Kd);
  bool leftSensors = false;
  bool rightSensors = false;

  for (int i = 0; i < 4; i++) {
    if (sensorValues[i] > 250) leftSensors = true;
  }
  for (int i = 4; i < 8; i++) {
    if (sensorValues[i] > 250) rightSensors = true;
  }
  if (leftSensors && !rightSensors){
    stopRobot();
    delay(50);
    forward(350);
    left90();
    stopRobot();
    delay(50);
    resetPID();
  }
  else if (rightSensors && !leftSensors) {
    stopRobot();
    delay(50);
    forward(350);
    right90();
    stopRobot();
    delay(50);
    resetPID();
  }
}

void resetPID() {
  integral = 0;
  error = 0;
  derivative = 0;
  previousError = 0;
}
