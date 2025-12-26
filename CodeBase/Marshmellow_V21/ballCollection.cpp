#include "ballCollection.h"
#include "motors.h"
#include "lineFollow.h"
#include "helperFunctions.h"
#include "sensors.h"
#include "armControl.h"

// int needColour = 1;

void goAboutCircle(int radius, float k, int dir = 1) {
  stepperL.setCurrentPosition(0);
  stepperR.setCurrentPosition(0);

  float left_radius = radius - (ROBOT_WIDTH_CM / 2.0);
  float right_radius = radius + (ROBOT_WIDTH_CM / 2.0);

  float left_distance;
  float right_distance;

  if (dir == 1) {
    left_distance = 2 * 3.14159 * left_radius * k;
    right_distance = 2 * 3.14159 * right_radius * k;
  }

  if (dir == -1) {
    right_distance = 2 * 3.14159 * left_radius * k;
    left_distance = 2 * 3.14159 * right_radius * k;
  }

  long left_target_steps = left_distance * STEPS_PER_CM_round;
  long right_target_steps = right_distance * STEPS_PER_CM_round;

  long positions[2];
  positions[0] = left_target_steps;
  positions[1] = right_target_steps;
  steppers.moveTo(positions);

  bool running = true;
  while (running) {
    running = steppers.run();
  }
}

void collectBallsHexagon(int missing) {
  //
  right60();
  stopRobot();
  turnAngle(160);
  arm.prepareBall();
  delay(500);
  arm.pickupBall();
  int colour = readColor();
  if (colour == missing){
    delay(200);
    arm.dropBall();
  }else{
    delay(200);
    arm.dropBallNoneed();
  }

//
  forward(330);
  turnAngle(240);
  stopRobot();
  arm.prepareBall();
  delay(500);
  arm.pickupBall();
  colour = readColor();
  if (colour == missing){
    delay(200);
    arm.dropBall();
  }else{
    delay(200);
    arm.dropBallNoneed();
  }

//
  forward(530);
  turnAngle(600);
  stopRobot();
  arm.prepareBall();
  delay(500);
  arm.pickupBall();
  colour = readColor();
  if (colour == missing){
    delay(200);
    arm.dropBall();
  }else{
    delay(200);
    arm.dropBallNoneed();
  }
//
  forward(460);
  turnAngle(430);
  stopRobot();
  arm.prepareBall();
  delay(500);
  arm.pickupBall();
  colour = readColor();
  if (colour == missing){
    delay(200);
    arm.dropBall();
  }else{
    delay(200);
    arm.dropBallNoneed();
  }
//
  forward(550);
  turnAngle(650);
  stopRobot();
  arm.prepareBall();
  delay(500);
  arm.pickupBall();
  colour = readColor();
  if (colour == missing){
    delay(200);
    arm.dropBall();
  }else{
    delay(200);
    arm.dropBallNoneed();
  }
//
  forward(350);
  turnAngle(550);
  forward(100);
  stopRobot();
  arm.prepareBall();
  delay(500);
  arm.pickupBall();

  colour = readColor();
  if (colour == missing){
    delay(200);
    arm.dropBall();
  }else{
    delay(200);
    arm.dropBallNoneed();
  }

  turnAngle(500, -1);
  while (getDistance(rightStraightIR) < 40) {
     wallFollow(6, 0, 2.85, 200);
  }
  goAboutCircle(18, 0.4, -1);
}
