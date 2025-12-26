#include "MotorControl.h"

MotorControl::MotorControl()
    : leftStepper(AccelStepper::DRIVER, StepPinL, DirPinL),
      rightStepper(AccelStepper::DRIVER, StepPinR, DirPinR) {}

void MotorControl::init() {
  // Initialize Control Pins
  pinMode(L0, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(DirPinL, OUTPUT);
  pinMode(EnPinL, OUTPUT);
  digitalWrite(EnPinL, 0); // Enable (Active LOW)
  digitalWrite(L0, 0);     // 1/4
  digitalWrite(L1, 1);
  digitalWrite(L2, 0);

  pinMode(R0, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(DirPinR, OUTPUT);
  pinMode(EnPinR, OUTPUT);
  digitalWrite(EnPinR, 0); // Enable (Active LOW)
  digitalWrite(R0, 0);     // 1/4
  digitalWrite(R1, 1);
  digitalWrite(R2, 0);

  Serial.println("MotorControl Initialized. Motors Enabled.");
  leftStepper.setPinsInverted(true);

  leftStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  leftStepper.setAcceleration(STEPPER_ACCELERATION);
  rightStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  rightStepper.setAcceleration(STEPPER_ACCELERATION);
}

void MotorControl::moveForward(long steps) {
  leftStepper.setAcceleration(STEPPER_ACCELERATION);
  rightStepper.setAcceleration(STEPPER_ACCELERATION);
  digitalWrite(DirPinL, HIGH);
  digitalWrite(DirPinR, HIGH);
  leftStepper.moveTo(leftStepper.currentPosition() + steps);
  rightStepper.moveTo(rightStepper.currentPosition() + steps);
  // stepperL.move(steps);
  // stepperR.move(steps);
  while (leftStepper.isRunning() == true || rightStepper.isRunning() == true) {
    leftStepper.run();
    rightStepper.run();
  }
}

void MotorControl::moveBackward(long steps) {
  MotorControl::moveForward(-steps);
}

void MotorControl::turnLeft(float degrees) {
  leftStepper.setAcceleration(STEPPER_ACCELERATION);
  rightStepper.setAcceleration(STEPPER_ACCELERATION);
  leftStepper.move(-400);
  rightStepper.move(+400);
  while (leftStepper.distanceToGo() != 0 || rightStepper.distanceToGo() != 0) {
    leftStepper.run();
    rightStepper.run();
  }
}

void MotorControl::turnRight(float degrees) {
  leftStepper.setAcceleration(STEPPER_ACCELERATION);
  rightStepper.setAcceleration(STEPPER_ACCELERATION);
  leftStepper.move(400);
  rightStepper.move(-400);
  while (leftStepper.distanceToGo() != 0 || rightStepper.distanceToGo() != 0) {
    leftStepper.run();
    rightStepper.run();
  }
}

void MotorControl::stop() {
  leftStepper.stop();
  rightStepper.stop();
}

void MotorControl::run() {
  leftStepper.run();
  rightStepper.run();
}

void MotorControl::runSpeed() {
  leftStepper.runSpeed();
  rightStepper.runSpeed();
}

void MotorControl::setLeftSpeed(float speed) { leftStepper.setSpeed(speed); }

void MotorControl::setRightSpeed(float speed) { rightStepper.setSpeed(speed); }

void MotorControl::applyMotorSpeed(float correction, int speed) {
  //  int baseSpeed = 1000; // Too fast for startup?
  int baseSpeed = speed; // Default was 1000, now passed in. Check Caller.//
                         // User defined base speed
  int maxSpeed = 1400;   // Increased to 1200 (was 800) for faster response

  int leftSpeed = baseSpeed - correction;
  int rightSpeed = baseSpeed + correction;

  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  leftStepper.setSpeed(leftSpeed);
  rightStepper.setSpeed(rightSpeed);

  leftStepper.runSpeed();
  rightStepper.runSpeed();
}

bool MotorControl::isRunning() {
  return leftStepper.isRunning() || rightStepper.isRunning();
}

long MotorControl::cmToSteps(float cm) { return cm * STEPS_PER_CM; }

long MotorControl::degreesToSteps(float degrees) {
  // Arc length for one wheel to turn 'degrees'
  // Arc = (degrees/360) * (PI * WheelBase)
  float turningCircumference = WHEEL_BASE_CM * PI;
  float arcLength = (degrees / 360.0) * turningCircumference;
  return cmToSteps(arcLength);
}

long MotorControl::getCurrentPosition() {
  return leftStepper.currentPosition();
}

void MotorControl::enableMotors() {
  digitalWrite(EnPinL, LOW);
  digitalWrite(EnPinR, LOW);
}

void MotorControl::disableMotors() {
  digitalWrite(EnPinL, HIGH);
  digitalWrite(EnPinR, HIGH);
}
