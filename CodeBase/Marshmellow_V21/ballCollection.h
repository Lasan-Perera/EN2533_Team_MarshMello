#pragma once

// extern int needColour;

void goAboutCircle(int radius, float k,int dir = 1);
void collectBalls();
void collectBallsHexagon(int missing);

const float WHEEL_DIAMETER_CM = 8.5;
const float ROBOT_WIDTH_CM = 17;
const int STEPS_PER_REV = 200;
const int MICROSTEPS = 4; // 1/4 step
const float STEPS_PER_CM_round = (STEPS_PER_REV * MICROSTEPS) / (3.14159 * WHEEL_DIAMETER_CM);
