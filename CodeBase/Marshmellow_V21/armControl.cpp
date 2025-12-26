#include "armControl.h"
#include <Arduino.h>
// Struct/instances defined in header as members

ArmControl::ArmControl() {}

void ArmControl::init() {
  // Initialize Pointers
  armMotion.s = &servoBig;
  grab1Motion.s = &servoSmall1;
  grab2Motion.s = &servoSmall2;

  // Attach servos via enable()
  enable();

  // Initial position: Arm Up, Gripper Open
  armAngle(100);
  grabberAngle(100);
  run(); // Wait for init move to finish
}

void ArmControl::enable() {
  if (!servoBig.attached())
    servoBig.attach(7);
  if (!servoSmall1.attached())
    servoSmall1.attach(5);
  if (!servoSmall2.attached())
    servoSmall2.attach(8);
}

void ArmControl::disable() {
  if (servoBig.attached())
    servoBig.detach();
  if (servoSmall1.attached())
    servoSmall1.detach();
  if (servoSmall2.attached())
    servoSmall2.detach();
}

// Blocks until all active motions are complete
void ArmControl::run() {
  while (armMotion.active || grab1Motion.active || grab2Motion.active) {
    updateMove(armMotion);
    updateMove(grab1Motion);
    updateMove(grab2Motion);
  }
}

void ArmControl::startMove(ServoMotion &m, int target, int speed) {
  m.target = target;
  m.speed = speed;
  m.lastStep = millis();
  m.active = true;
}

void ArmControl::updateMove(ServoMotion &m) {
  if (!m.active)
    return;

  if (millis() - m.lastStep >= (unsigned long)m.speed) {
    m.lastStep = millis();
    int pos = m.s->read();

    if (pos == m.target) {
      m.active = false;
      return;
    }

    pos += (m.target > pos) ? 1 : -1;
    m.s->write(pos);
  }
}

void ArmControl::grabberAngle(int angle) {
  angle = constrain(angle, 0, 110);
  int target1 = map(angle, 0, 110, 23, 133);
  int target2 = map(angle, 0, 110, 155, 45);

  startMove(grab1Motion, target1, 5); // 5ms per degree
  startMove(grab2Motion, target2, 5);
}

void ArmControl::armAngle(int angle) {
  angle = constrain(angle, 0, 100);
  // REDUCED RANGE: 20 to 160
  int target = map(angle, 0, 100, 5, 180);

  startMove(armMotion, target, 5); // 5ms per degree
}

// ===================== HIGH LEVEL SEQUENCES =====================

void ArmControl::home() {
  enable();
  armAngle(0);
  grabberAngle(0);
  run();
}

void ArmControl::pickupBox() {
  Serial.println("Pickup Box (Close & Lift)");
  enable();

  // 1. Close gripper
  grabberAngle(88);
  run(); // Wait for close

  // 2. Lift arm
  armAngle(100);
  run(); // Wait for lift
}



void ArmControl::preparePickup() {
  Serial.println("Prepare Pickup (Open & Lower)");
  enable();

  // 1. Open gripper
  grabberAngle(0);
  // run(); // Optional: Can move arm/grabber simultaneously if preferred?
  // keeping sequential for safety as per plan
  run();

  // 2. Lower arm
  armAngle(0);
  run();
}

void ArmControl::dropBox() {
  Serial.println("Drop Box");

  // 1. Lower arm
  armAngle(5);
  run();

  // 2. Open gripper
  grabberAngle(5);
  run();

  // Power down to prevent overheating while idle
  disable();
}

void ArmControl::prepareBall() {
  Serial.println("Prepare to pickup a ball");
  enable();

  // 1. Open gripper
  grabberAngle(45);
  // run(); // Optional: Can move arm/grabber simultaneously if preferred?
  // keeping sequential for safety as per plan
  run();

  // 2. Lower arm
  armAngle(0);
  run();
}

void ArmControl::pickupBall() {
  Serial.println("Picking up ball");
  enable();

  // 1. Open gripper
  grabberAngle(38);
  // run(); // Optional: Can move arm/grabber simultaneously if preferred?
  // keeping sequential for safety as per plan
  run();

  // 2. Lower arm
  armAngle(0);
  run();

  armAngle(70);
  run();

  grabberAngle(50);
  run();
  grabberAngle(40);
  run();
}

void ArmControl::dropBall() {
  Serial.println("Dropping the ball");
  enable();

  armAngle(100);
  run();
  // 1. Open gripper
  grabberAngle(70);
  // run(); // Optional: Can move arm/grabber simultaneously if preferred?
  // keeping sequential for safety as per plan
  run();

  // 2. Lower arm
}

void ArmControl::dropBallNoneed() {
  Serial.println("Dropping the ball");
  enable();

  armAngle(0);
  run();
  // 1. Open gripper
  grabberAngle(50);
  // run(); // Optional: Can move arm/grabber simultaneously if preferred?
  // keeping sequential for safety as per plan
  run();

  armAngle(100);
  run();

  // 2. Lower arm
}


void ArmControl::indication() {
  // Serial.println("Picking up ball");
  enable();
  // 2. Lower arm
  armAngle(100);
  run();

  grabberAngle(0);
  run();
  grabberAngle(50);
  run();
  grabberAngle(0);
  run();
  grabberAngle(50);
  run();

}

ArmControl arm;

void setupArm() {
  arm.init();
}
