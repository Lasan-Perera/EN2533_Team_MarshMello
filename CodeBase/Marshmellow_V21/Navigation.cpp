#include "Navigation.h"

Navigation::Navigation(MotorControl &mc, SensorsTask &s)
    : motorControl(mc), sensors(s) {
  lastError = 0;
  integral = 0;
}

void Navigation::update() { motorControl.run(); }

void Navigation::followLine(int speed) {
  // PID Logic using getLinePosition

  float position = sensors.getLinePosition();

  // User params: Kp=3.9, Ki=0, Kd=1.85
  // Error is position (0 is center)
  float error = position;

  integral += error;
  float derivative = error - lastError;
  float correction =
      10 * ((3.9 * error) + (0.0 * integral) + (1.85 * derivative));
  // 10 * ((7 * error) + (0.0 * integral) + (1.85 * derivative));

  lastError = error;

  motorControl.applyMotorSpeed(correction, speed);
}

bool Navigation::moveCells(int numCells) {
  // Serial.println("DEBUG: Inside moveCells");
  if (numCells > 0) {
    // 1. Clear start intersection if we are on one
    if (sensors.isFourWay()) {
      Serial.println("[NAV] Clearing start intersection...");
      while (sensors.isFourWay()) {
        followLine(1000);
      }
      Serial.println("[NAV] Cleared.");
    }

    int num_intersection = 0;
    // Serial.println("DEBUG: Moving to next intersection...");
    unsigned long lastPrint = 0;
    unsigned long lastObstacleCheck = 0;

    long startSteps = motorControl.getCurrentPosition();

    // User-provided structure: Loop until target reached
    while (numCells > num_intersection) {
      // Safety Check: Emergency Stop (Check every 100ms to verify path without
      // stalling motors)
      if (millis() - lastObstacleCheck > 50) {
        lastObstacleCheck = millis();
        if (sensors.isObstacleFront()) {
          Serial.println("[NAV] Emergency Stop! Obstacle Detected.");
          motorControl.stop();
          break;
        }
      }

      followLine(1000);

      if (millis() - lastPrint > 500) {
        lastPrint = millis();
      }

      if (sensors.isFourWay()) {
        long currentSteps = motorControl.getCurrentPosition();

        // DEBOUNCE: Only accept intersection if we moved at least 300 steps
        // since the start of this segment.
        if (abs(currentSteps - startSteps) > 200) {
          Serial.println("[NAV] Intersection Validated.");
          num_intersection++;

          // Reset start counter to CURRENT position for the next segment
          startSteps = currentSteps;

          if (num_intersection == numCells) {
            // Target reached! Stop here.
            break;
          }

          // Clear intermediate intersection
          while (sensors.isFourWay()) {
            followLine(1000);
          }
          Serial.println("[NAV] Intersection Cleared.");

        } else {
          // Ignored (Noise or Double Detection)
          // Serial.println("[NAV] Intersection Ignored (Too Close).");
        }
      }
    }
  }
  motorControl.stop();
  return true;
}

void Navigation::turnIntersection(char direction) {
  // Move slightly forward to center rotation axis on intersection
  // Use PID to stay centered while approaching pivot point (approx 7cm)

  if (direction == 'L') {
    moveForwardPID(320);
    motorControl.turnLeft(90);
  } else if (direction == 'R') {
    moveForwardPID(320);
    motorControl.turnRight(90);
  } else if (direction == 'B') {
    moveForwardPID(320);
    motorControl.turnLeft(90);
    motorControl.turnLeft(90);
  }

  while (motorControl.isRunning())
    motorControl.run();
}

bool Navigation::isAtIntersection() { return sensors.isFourWay(); }

void Navigation::moveForwardPID(long steps) {
  long targetSteps = steps;
  long startSteps = motorControl.getCurrentPosition();

  while (abs(motorControl.getCurrentPosition() - startSteps) < targetSteps) {
    followLine(1000); // Reduced speed to prevent stalle for start)
  }
  motorControl.stop();
}

void Navigation::moveBackwardPID(long steps) {
  long targetSteps = abs(steps); // Ensure positive magnitude
  long startSteps = motorControl.getCurrentPosition();

  while (abs(motorControl.getCurrentPosition() - startSteps) < targetSteps) {
    followLine(-1000); // Backward speed 300
  }
  motorControl.stop();
}
