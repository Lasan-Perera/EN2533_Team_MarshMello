#include "sensors.h"
#include "lineFollow.h"
#include "motors.h"

int digitalWeights[8] = {-15, -10, -5, 0, 0, 5, 10, 15};
int getDigitalLinePosition() {
  int sumWeighted = 0;
  int sumHits     = 0;

  for (int i = 1; i <= 8; i++) {
    int isWhite = (sensorDigitalValues[i] == 0);  // 0 = white = ON LINE
    if (isWhite) {
      sumWeighted += digitalWeights[i - 1];
      sumHits++;
    }
  }

  if (sumHits == 0) return 0;   // Line lost (center command)
  
  return sumWeighted;           // Range: roughly -3 to +3
}
void lineFollowDigital(float Kp, float Ki, float Kd) {

  readArrowSensorsDigital();      // updates sensorDigitalValues[]
  float position = getDigitalLinePosition();
  error = position;
  integral += error;
  derivative = error - previousError;

  correction = (Kp * error + Ki * integral + Kd * derivative) * 6;
  previousError = error;
  
  applyMotorSpeed(correction);
}

const int LOST_LINE_LIMIT = 5;

enum FollowMode {
  MODE_FOLLOW,   // normal arrow/line follow
  MODE_GAP,      // line lost, move straight until we find line again
  MODE_ALIGN     // turning to side with more white
};

FollowMode mode = MODE_FOLLOW;

int lostCount    = 50;
int alignDir     = 0;   // -1 = turn left, +1 = turn right

bool anyWhite() {
  for (int i = 0; i < 10; i++) {
    if (sensorDigitalValues[i] == 0) return true;
  }
  return false;
}

int countRightWhite() {
  int c = 0;
  for (int i = 0; i <= 3; i++) {
    if (sensorDigitalValues[i] == 0) c++;
  }
  return c;
}

int countLeftWhite() {
  int c = 0;
  for (int i = 6; i <= 9; i++) {
    if (sensorDigitalValues[i] == 0) c++;
  }
  return c;
}

int countCenterWhite() {
  int c = 0;
  if (sensorDigitalValues[4] == 0) c++;
  if (sensorDigitalValues[5] == 0) c++;
  return c;
}


void arrowFollowLoop() {
  readArrowSensorsDigital();

  bool onLine       = anyWhite();
  int leftWhite     = countLeftWhite();
  int rightWhite    = countRightWhite();
  int centerWhite   = countCenterWhite();

  switch (mode) {
    case MODE_FOLLOW: {
      if (onLine) {
        lostCount = 0;
        lineFollowDigital(9, 0, 2);
      } else {
        lostCount++;
        if (lostCount >= LOST_LINE_LIMIT) {
          Serial.println("Line lost -> GAP mode");
          mode = MODE_GAP;
          lostCount = 0;
        } else {
          applyMotorSpeed(0);
        }
      }
      break;
    }
    case MODE_GAP: {
      applyMotorSpeed(0);
      if (onLine) {
        Serial.println("Line found in GAP mode -> decide direction");
        if (centerWhite > 0) {
          Serial.println("Center sees white -> back to FOLLOW");
          mode = MODE_FOLLOW;
          lostCount = 0;
        } else {
          forward(200);
          if (leftWhite > rightWhite) {
            alignDir = -1;   // choose left (you can flip if your mapping is opposite)
            Serial.println("More white on LEFT -> ALIGN LEFT");
          } else if (rightWhite > leftWhite) {
            alignDir = +1;   // choose right
            Serial.println("More white on RIGHT -> ALIGN RIGHT");
          } else {
            // equal white but not centered: you can choose a default
            alignDir = 0;
            Serial.println("Equal whites with no center; you decide default behavior");
          }
          mode = MODE_ALIGN;
        }
      }
      break;
    }

    case MODE_ALIGN: {
      onLine       = anyWhite();
      leftWhite    = countLeftWhite();
      rightWhite   = countRightWhite();
      centerWhite  = countCenterWhite();

      if (centerWhite > 0) {
        Serial.println("Center white during ALIGN -> back to FOLLOW");
        mode      = MODE_FOLLOW;
        lostCount = 0;
        break;
      }
      if (alignDir < 0) {
        // ==============================
        // TODO: ALIGN LEFT TURNING CODE
        // e.g. slow left, fast right, or pure rotation
        stepperL.setSpeed(-650);
        stepperR.setSpeed(+650);
        stepperL.runSpeed();
        stepperR.runSpeed();
        // ==============================
      } else if (alignDir > 0) {
        // ==============================
        // TODO: ALIGN RIGHT TURNING CODE
        // e.g. fast left, slow right, or pure rotation
        stepperL.setSpeed(+650);
        stepperR.setSpeed(-650);
        stepperL.runSpeed();
        stepperR.runSpeed();
        // ==============================
      } else {
        // No direction chosen (rare fallback)
        applyMotorSpeed(0);  // straight
      }

      // You can also add your own condition here, e.g.:
      // if (someAlignmentConditionMet) {
      //   mode = MODE_FOLLOW;
      //   reset PID if needed
      // }

      break;
    }
  }
}


