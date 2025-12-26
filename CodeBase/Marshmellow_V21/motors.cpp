#include "motors.h"

AccelStepper stepperL(AccelStepper::DRIVER, StepPinL, DirPinL); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepperR(AccelStepper::DRIVER, StepPinR, DirPinR); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 54
MultiStepper steppers;


Servo servoBig;      
Servo servoSmall1;  
Servo servoSmall2;

int baseSpeed = 1000;
int maxSpeed  = 1500;

void setupMotors() {
  pinMode(L0, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(DirPinL, OUTPUT);
  pinMode(EnPinL, OUTPUT);
  digitalWrite(EnPinL, 1);  //disable
  digitalWrite(L0, 0);//  1/4 
  digitalWrite(L1, 1);
  digitalWrite(L2, 0);

  pinMode(R0, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(DirPinR, OUTPUT);
  pinMode(EnPinR, OUTPUT);
  digitalWrite(EnPinR, 1);  //disable
  digitalWrite(R0, 0);//  1/4 
  digitalWrite(R1, 1);
  digitalWrite(R2, 0);
  stepperL.setPinsInverted(true);

  stepperL.setMaxSpeed(maxSpeed);
  stepperR.setMaxSpeed(maxSpeed);

  steppers.addStepper(stepperL);
  steppers.addStepper(stepperR);  

  digitalWrite(EnPinL, 0);
  digitalWrite(EnPinR, 0);
}

void setupServos() {
  servoBig.attach(7);
  servoSmall1.attach(8);
  servoSmall2.attach(5);

  servoBig.write(60);
  servoSmall1.write(80);
  servoSmall2.write(110);
}

void right90() {
  stepperL.setAcceleration(1000.0);  
  stepperR.setAcceleration(1000.0);
  stepperL.move(400);
  stepperR.move(-400);
  while(stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
}

void left90() {
  stepperL.setAcceleration(1000.0);  
  stepperR.setAcceleration(1000.0);
  stepperL.move(-400);
  stepperR.move(+400);
  while(stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
}

void turnAngle(int steps, int dir = 1) {
  stepperL.setAcceleration(1000.0);  
  stepperR.setAcceleration(1000.0);

  if (dir == 1){
    stepperL.move(0);
    stepperR.move(steps);
  } else {
    stepperL.move(steps);
    stepperR.move(0);
  }

  while(stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
}

void forward(int steps) {
  stepperL.setAcceleration(1000.0);  
  stepperR.setAcceleration(1000.0);
  digitalWrite(DirPinL, HIGH);
  digitalWrite(DirPinR, HIGH);    
  stepperL.moveTo(stepperL.currentPosition ()+steps);
  stepperR.moveTo(stepperR.currentPosition ()+steps);
  // stepperL.move(steps);
  // stepperR.move(steps);
  while(stepperL.isRunning() ==  true || stepperR.isRunning() ==  true) {
    stepperL.run();
    stepperR.run();
  }
}

void applyMotorSpeed(float correction) {
  int leftSpeed  = baseSpeed - correction;
  int rightSpeed = baseSpeed + correction;

  leftSpeed  = constrain(leftSpeed,  -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  stepperL.setSpeed(leftSpeed);
  stepperR.setSpeed(rightSpeed);

  stepperL.runSpeed();
  stepperR.runSpeed();
}

void applyMotorSpeed_slow(float correction) {
  int leftSpeed  = baseSpeed/2 - correction;
  int rightSpeed = baseSpeed/2 + correction;

  leftSpeed  = constrain(leftSpeed,  -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  stepperL.setSpeed(leftSpeed);
  stepperR.setSpeed(rightSpeed);

  stepperL.runSpeed();
  stepperR.runSpeed();
}


void stopRobot() {
  stepperL.stop();
  stepperR.stop();
  stepperL.runToPosition(); 
  stepperR.runToPosition();
}


void sweepServo(Servo &s, int startAngle, int endAngle, int speed = 10) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      s.write(pos);
      delay(speed);
    }
  } else {
    for (int pos = startAngle; pos >= endAngle; pos--) {
      s.write(pos);
      delay(speed);
    }
  }
}

void sweepGrabber(int start1, int end1, int start2, int end2, int steps = 1, int speed = 10) {
  int pos1 = start1;
  int pos2 = start2;

  int dir1 = (end1 > start1) ? 1 : -1;
  int dir2 = (end2 > start2) ? 1 : -1;

  while (pos1 != end1 || pos2 != end2) {
    if (pos1 != end1) pos1 += dir1;
    if (pos2 != end2) pos2 += dir2;

    servoSmall1.write(pos1);
    servoSmall2.write(pos2);
    delay(speed);
  }
}

void armDown() {
  sweepServo(servoBig, servoBig.read(), 5);
}

void armUp() {
  sweepServo(servoBig, servoBig.read(), 180);
}

void grabberOpen() {
  sweepGrabber(servoSmall1.read(), 20, servoSmall2.read(), 155, 1, 10);
}

void grabberClose() {
  sweepGrabber(servoSmall1.read(), 130, servoSmall2.read(), 45, 1, 10);
}

void leftTurn(int turnSteps = 267) {
  stepperL.setAcceleration(1000.0);
  stepperR.setAcceleration(1000.0);

  stepperL.move(-turnSteps);
  stepperR.move(+turnSteps);

  while (stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
}

void right60() {
  stepperL.setAcceleration(1000.0);
  stepperR.setAcceleration(1000.0);

  int turnSteps = 220;  // approx 60 degrees based on 400 = 90°

  stepperL.move(+turnSteps);
  stepperR.move(-turnSteps);

  while (stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
}