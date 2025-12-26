#include "LabyrinthTask.h"
#include "MotorControl.h"

// Constants
#define NOD_DISTANCE_STEPS 450
#define HEIGHT_SENSOR_THRESHOLD 50.0

LabyrinthTask::LabyrinthTask(Navigation &nav, ArmControlTask &armTask,
                             SensorsTask &sensors, MotorControl &motorControl)
    : nav(nav), armTask(armTask), sensors(sensors), motorControl(motorControl) {
  // Constructor only handles dependency injection
}

void LabyrinthTask::init() {
  currentState = STATE_START;
  currentPos = {0, 0};
  currentHeading = 'N';
  exitLocation = {GRID_SIZE - 1, GRID_SIZE - 1};
  boxesCollected = 0;
  searchIndex = 0;
  boxLocation = {-1, -1};

  // Mark Drop Locations as Obstacles (No lines there)
  grid.setObstacle(0, 0);
  grid.setObstacle(2, 8);
  grid.setObstacle(4, 8);
  grid.setObstacle(6, 8);

  collectedColors[0] = COLOR_NONE;
  collectedColors[1] = COLOR_NONE;

  // Load Predefined Obstacles from Config
  for (int i = 0; i < PREDEFINED_OBSTACLES_COUNT; i++) {
    grid.setObstacle(PREDEFINED_OBSTACLES[i].x, PREDEFINED_OBSTACLES[i].y);
    Serial.print("[INIT] Predefined Obstacle at: ");
    Serial.print(PREDEFINED_OBSTACLES[i].x);
    Serial.print(",");
    Serial.println(PREDEFINED_OBSTACLES[i].y);
  }

  // Initialize search waypoints (Granular Snake Pattern)
  int idx = 0;
  for (int x = 0; x < GRID_SIZE; x++) {
    if (x % 2 == 0) {
      // For x=0, skip (0,0) as it is the start/home cell
      int startY = (x == 0) ? 1 : 0;
      for (int y = startY; y < GRID_SIZE; y++) {
        searchWaypoints[idx++] = {x, y};
      }
    } else {
      for (int y = GRID_SIZE - 1; y >= 0; y--) {
        searchWaypoints[idx++] = {x, y};
      }
    }
  }

  searchIndex = 0;
  boxLocation = searchWaypoints[searchIndex];
  sanctuaryLocation = {0, 0};
  boxesCollected = 0;
}

void LabyrinthTask::setInitialPosition(Point startPos, char startHeading) {
  currentPos = startPos;
  currentHeading = startHeading;
}

void LabyrinthTask::run() { executeState(); }

bool LabyrinthTask::recorrection() {
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
    motorControl.runSpeed();
  }
  motorControl.stop();
  delay(500);
  motorControl.moveForward(350); // Align wheels to intersection center
  motorControl.stop();
  delay(500);

  // 2. Turn Left
  motorControl.turnLeft(90);
  delay(500);

  // 3. Go forward until 2nd 4-way
  // motorControl.moveForward(350); // Clear intersection
  motorControl.stop();
  delay(100);

  while (!sensors.isFourWay()) {
    nav.followLine(1000);
    motorControl.runSpeed();
  }
  motorControl.stop();
  delay(500);
  motorControl.moveForward(350); // Align wheels to intersection center
  motorControl.stop();
  delay(500);

  // 4. Turn 350
  motorControl.turnRight(90);
  delay(500);

  // 5. Set Position
  setInitialPosition({0, 1}, 'N');
  Serial.println(">>> CALIBRATION COMPLETE. STARTING TASK AT (0,1) N <<<");
  return true;
}

void LabyrinthTask::executeState() {
  switch (currentState) {
  case STATE_START:
    handleStart();
    break;
  case STATE_NAVIGATE_TO_BOX:
    handleNavigateToBox();
    break;
  case STATE_SCAN_BOX:
    handleScanBox();
    break;
  case STATE_ALIGN_FOR_PICKUP:
    handleAlignForPickup();
    break;
  case STATE_PICKUP_BOX:
    handlePickupBox();
    break;
  case STATE_NAVIGATE_TO_SANCTUARY:
    handleNavigateToSanctuary();
    break;
  case STATE_DROP_BOX:
    handleDropBox();
    break;
  case STATE_NAVIGATE_TO_EXIT:
    handleNavigateToExit();
    break;
  case STATE_FINISH:
    handleFinish();
    break;
  }
}

void LabyrinthTask::handleStart() {
  Serial.println("State: START -> NAVIGATE_TO_BOX");
  Serial.flush();
  currentState = STATE_NAVIGATE_TO_BOX;
}

void LabyrinthTask::advanceSearchTarget() {
  searchIndex++;
  while (searchIndex < TOTAL_CELLS) {
    Point nextPt = searchWaypoints[searchIndex];
    if (grid.isObstacle(nextPt.x, nextPt.y) ||
        grid.isScannedEmpty(nextPt.x, nextPt.y)) {
      searchIndex++;
    } else {
      break;
    }
  }
  if (searchIndex < TOTAL_CELLS) {
    boxLocation = searchWaypoints[searchIndex];
  } else {
    currentState = STATE_NAVIGATE_TO_EXIT;
  }
}

void LabyrinthTask::handleNavigateToBox() {
  Serial.println("DEBUG: Inside STATE_NAVIGATE_TO_BOX");
  Serial.flush();
  delay(100);

  // Debug Prints
  Serial.print("Current: ");
  Serial.print(currentPos.x);
  Serial.print(",");
  Serial.println(currentPos.y);
  Serial.print("Target: ");
  Serial.print(boxLocation.x);
  Serial.print(",");
  Serial.println(boxLocation.y);
  delay(100);

  // Sanity check: Should already be at target (currentPos)
  if (currentPos == boxLocation) {
    advanceSearchTarget();
  } else {
    // Check if target is an obstacle then it recalculates the path
    if (grid.isObstacle(boxLocation.x, boxLocation.y)) {
      advanceSearchTarget();
      return;
    }
    if (!moveTo(boxLocation)) {
      // Did we switch state (e.g. found a box)?
      if (currentState != STATE_NAVIGATE_TO_BOX)
        return;

      Serial.println("[NAV] Move blocked.");
      if (grid.isObstacle(boxLocation.x, boxLocation.y)) {
        Serial.println("[NAV] Target is obstacle. Skipping.");
        advanceSearchTarget();
      } else {
        Serial.println("[NAV] Path blocked but target open. Re-routing...");
      }
    }
  }
}

bool LabyrinthTask::taskDone() { return currentState == STATE_FINISH; }

void LabyrinthTask::handleScanBox() {
  Serial.println("State: SCAN_BOX");
  // // detectedColor is already set by manualManeuver logic
  Serial.print("[SCAN] Using detected color: ");
  Serial.println(detectedColor);

  if (detectedColor != COLOR_NONE) {
    // Save the color
    if (boxesCollected < totalBoxes) {
      collectedColors[boxesCollected] = detectedColor;
    }

    // 1. Identify the Target (The Void/Obstacle Cell)
    if (detectedColor == COLOR_RED)
      dropTarget = DROP_LOCATION_RED;
    else if (detectedColor == COLOR_BLUE)
      dropTarget = DROP_LOCATION_BLUE;
    else if (detectedColor == COLOR_GREEN)
      dropTarget = DROP_LOCATION_GREEN;
    else
      dropTarget = {0, 0};

    // 2. Calculate the Best Sanctuary Spot
    // This automatically picks the closest available side (N, S, E, or W)
    sanctuaryLocation = findBestSanctuary(dropTarget);

    Serial.print("[LOGIC] Drop Target: ");
    Serial.print(dropTarget.x);
    Serial.print(",");
    Serial.println(dropTarget.y);
    Serial.print("[LOGIC] Approach Spot: ");
    Serial.print(sanctuaryLocation.x);
    Serial.print(",");
    Serial.println(sanctuaryLocation.y);

    currentState = STATE_NAVIGATE_TO_SANCTUARY;
  }
}

void LabyrinthTask::handleAlignForPickup() {
  Serial.println("State: ALIGN_FOR_PICKUP");

  // Sanity check: Should already be at target (currentPos)
  if (currentPos != alignmentTarget) {
    Serial.println("[ALIGN] Error: Not at alignment target? Correcting...");
    alignmentTarget = currentPos; // Force it for manual maneuver
  }

  // Ensure heading is correct (should be set to currentHeading)
  if (currentHeading != alignmentHeading) {
    Serial.println("[ALIGN] Heading mismatch. Turning...");
    turnTo(alignmentHeading);
  }

  // Immediate transition to Pickup
  Serial.println("[ALIGN] Aligned. Transitioning to PICKUP.");
  currentState = STATE_PICKUP_BOX;
}

void LabyrinthTask::performPickupManeuver() {
  Serial.println("[PICKUP] Starting Manual Blind Maneuver (Scout Mode).");
  // nav.moveForwardPID(320);
  // motorControl.turnRight(90);
  // while (!sensors.isFourWay()) {
  //   nav.followLine(1000);
  // }
  // motorControl.stop();
  // armTask.preparePickup();
  nav.moveForwardPID(200);
  delay(1000);
  motorControl.turnRight(90);
  delay(1000);
  bool checkup = sensors.isObstacleFront();
  motorControl.turnLeft(90);
  delay(1000);
  motorControl.moveBackward(480);
  delay(1000);
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  motorControl.stop();
  delay(1000);

  if (!checkup) {
    Serial.println("[PICKUP] No obstacle in the RIGHT. Trying Right approach.");
    nav.moveForwardPID(350);
    motorControl.turnRight(90);
    while (!sensors.isFourWay()) {
      nav.followLine(1000);
    }
    motorControl.stop();
    if (sensors.isObstacleLeft()) {
      Serial.println(
          "[PICKUP] Obstacle detected on left. Using left approach.");
      selectedApproach = SIDE_LEFT;

    } else {
      Serial.println("[PICKUP] No obstacle in the left. Using Right approach.");
      selectedApproach = SIDE_RIGHT;
      // reverse code
    }

    motorControl.moveBackward(850); // Back off slightly to ensure line capture
    while (!sensors.isFourWay()) {
      nav.followLine(1000);
    }
    nav.moveForwardPID(320);
    motorControl.turnLeft(90);
    motorControl.stop();
    motorControl.moveBackward(480);
    while (!sensors.isFourWay()) {
      nav.followLine(1000);
    }
    motorControl.stop();
  } else {
    selectedApproach = SIDE_LEFT;
  }

  if (selectedApproach == SIDE_RIGHT) {
    manualManeuverRight();
  } else if (selectedApproach == SIDE_LEFT) {
    manualManeuverLeft();
  }
  // Re-Center at Start
  Serial.println("[PICKUP] Re-Aligning at Start Intersection...");
  motorControl.moveBackward(150); // Back off slightly to ensure line capture
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  // motorControl.stop();
  Serial.println("[PICKUP] Manual Maneuver Complete. Restored State.");

  // Fix position drift: Robot ends up 1 cell FORWARD (at the box location)
  // after maneuver Resync logic to match physical reality
  // if (currentHeading == 'N')
  //   currentPos.y++;
  // else if (currentHeading == 'S')
  //   currentPos.y--;
  // else if (currentHeading == 'E')
  //   currentPos.x++;
  // else if (currentHeading == 'W')
  //   currentPos.x--;

  Serial.print("[PICKUP] Position Updated (Forward) to: ");
  Serial.print(currentPos.x);
  Serial.print(",");
  Serial.println(currentPos.y);
}

void LabyrinthTask::manualManeuverRight() {
  // === RIGHT MANUAL MANEUVER (With Scout Check) ===

  // 1. Turn Right (Face Entry)
  Serial.println("[MANUAL-R] 1. Turn Right.");
  nav.moveForwardPID(320);
  motorControl.turnRight(90);
  while (motorControl.isRunning())
    motorControl.run();
  motorControl.stop();

  // 5. PICKUP
  Serial.println("[MANUAL-R] 5. PICKUP.");
  motorControl.disableMotors();
  armTask.preparePickup();
  motorControl.enableMotors();

  // 2. Move Forward 25cm (To Entry)
  Serial.println("[MANUAL-R] 2. Going until the intersection.");
  motorControl.moveForward(550);
  while (motorControl.isRunning())
    motorControl.run();
  motorControl.stop();

  // 3. Turn Left (Face Stand)
  Serial.println("[MANUAL-R] 3. Turn Left.");
  motorControl.turnLeft(90);
  while (motorControl.isRunning())
    motorControl.run();
  motorControl.stop();

  // 4. Move Forward 25cm (To Stand)
  Serial.println("[MANUAL-R] 4. Forward 25cm.");
  motorControl.moveForward(420);
  while (motorControl.isRunning())
    motorControl.run();
  motorControl.stop();

  // 5. PICKUP
  delay(1000);
  motorControl.disableMotors();
  this->detectedColor = sensors.readColor();
  Serial.print("[MANUAL-R] 5. Detected Color: ");
  Serial.println(this->detectedColor);
  armTask.pickupBox();
  motorControl.enableMotors();

  // 6. Reverse 25cm (To Entry)
  Serial.println("[MANUAL-R] 6. Reverse 25cm.");
  motorControl.moveBackward(420);
  while (motorControl.isRunning())
    motorControl.run();

  // 7. Turn Left (Face Start - West of Entry)
  // We are at Entry Facing N. Start is West.
  Serial.println("[MANUAL-R] 7. Turn Left.");
  motorControl.turnRight(90);
  while (motorControl.isRunning())
    motorControl.run();

  // 8. Move Forward 25cm (To Start)
  Serial.println("[MANUAL-R] 8. Forward 25cm.");
  motorControl.moveBackward(450);
  while (motorControl.isRunning())
    motorControl.run();

  // 9. Turn Right (Restore Heading N)
  // We are at Start Facing W. Original was N.
  Serial.println("[MANUAL-R] 9. Turn Right (Restore).");
  motorControl.turnLeft(90);

  motorControl.moveBackward(600);
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  motorControl.stop();
}

void LabyrinthTask::manualManeuverLeft() {
  // === LEFT MANUAL MANEUVER (With Scout Check) ===

  // 1. Turn Left (Face Entry)
  Serial.println("[MANUAL-L] 1. Turn Left.");
  nav.moveForwardPID(350);
  motorControl.turnLeft(90);
  while (motorControl.isRunning())
    motorControl.run();

  // 2. Move Forward 25cm (To Entry)
  Serial.println("[MANUAL-L] 2. Forward 25cm.");
  motorControl.moveForward(610);
  while (motorControl.isRunning())
    motorControl.run();

  Serial.println("[MANUAL-R] 5. Prepare.");
  motorControl.disableMotors();
  armTask.preparePickup();
  motorControl.enableMotors();

  // === SCOUT CHECK ===
  // We are at Entry. Facing West relative to Box.
  // Stand is to our RIGHT.

  // 4b. Turn 180 (Face S)
  Serial.println("[MANUAL-L] 4b. Turn 90.");
  motorControl.turnLeft(90);
  while (motorControl.isRunning())
    motorControl.run();
  motorControl.stop();

  Serial.println("Backwards");
  motorControl.moveBackward(1200);
  while (motorControl.isRunning())
    motorControl.run();

  // 5. PICKUP
  Serial.println("[MANUAL-L] 5. PICKUP.");
  motorControl.disableMotors();
  this->detectedColor = sensors.readColor();
  armTask.pickupBox();
  motorControl.enableMotors();

  // 6. Turn 180 (Face N)
  Serial.println("[MANUAL-L] 6. Turn 180.");
  motorControl.moveForward(1200);
  while (motorControl.isRunning())
    motorControl.run();

  // 7. Reverse 25cm (To Entry)
  Serial.println("[MANUAL-L] 7. Reverse 25cm.");
  motorControl.turnLeft(90);
  while (motorControl.isRunning())
    motorControl.run();

  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }

  // 8. Turn Right (Face Start).
  // We are at Entry Facing N. Start is East.
  Serial.println("[MANUAL-L] 8. Turn Right.");
  nav.moveForwardPID(350);
  motorControl.turnLeft(90);
  while (motorControl.isRunning())
    motorControl.run();

  // I added this code for testing this if not working remove it
  motorControl.moveBackward(450);
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  motorControl.stop();

  // 9. Forward 25cm (To Start)
  // Serial.println("[MANUAL-L] 9. Forward 25cm.");
  // motorControl.moveForward(450);
  // while (motorControl.isRunning())
  //   motorControl.run();

  // // 10. Turn Left (Restore Heading) - Face N is Left of E.
  // Serial.println("[MANUAL-L] 10. Turn Left (Restore).");
  // motorControl.turnLeft(90);
  // nav.followLine(1000);
}

// This should be well thought out.
void LabyrinthTask::handlePickupBox() {
  Serial.println("State: PICKUP_BOX");
  // if (sensors.isObstacleLeft()) {
  //   Serial.println("[PICKUP] Path Blocked! Aborting Pickup.");
  //   currentState = STATE_SCAN_BOX; // Abort (Wait, if unsuccessful, should go
  //                                  // back to Navigate?)
  //   // If we abort here, we are at alignment spot.
  //   // Logic says SCAN_BOX... but we haven't picked it up?
  //   // If blocked, we skip?
  //   currentState = STATE_NAVIGATE_TO_BOX; // Retry or skip
  //   return;
  // }

  performPickupManeuver();
  currentState = STATE_SCAN_BOX;
}

void LabyrinthTask::handleNavigateToSanctuary() {
  // 1. Check if we have arrived
  if (currentPos == sanctuaryLocation) {
    currentState = STATE_DROP_BOX;
    return;
  }

  // 2. Attempt to move
  // moveTo() returns FALSE if the path is blocked OR if the target is an
  // obstacle
  bool moveSuccess = moveTo(sanctuaryLocation);

  if (!moveSuccess) {
    // 3. DIAGNOSIS: Why did we fail?
    // Check if the Sanctuary ITSELF is now marked as an obstacle
    if (grid.isObstacle(sanctuaryLocation.x, sanctuaryLocation.y)) {

      Serial.println("[NAV] Sanctuary is blocked! Re-calculating...");

      // 4. RE-PLAN: Ask for the NEXT best spot
      // Since the old spot is now marked in the grid, findBestSanctuary won't
      // pick it again.
      Point newSpot = findBestSanctuary(dropTarget);

      // Check if we are completely stuck
      if (newSpot == sanctuaryLocation) {
        Serial.println("[CRITICAL] All approach angles blocked! Aborting.");
        currentState = STATE_NAVIGATE_TO_EXIT;
      } else {
        Serial.print("[NAV] Switching Sanctuary to: ");
        Serial.print(newSpot.x);
        Serial.print(",");
        Serial.println(newSpot.y);

        sanctuaryLocation = newSpot;
        // The loop will retry moving to this new spot in the next cycle
      }
    } else {
      // The sanctuary is clear, but we can't find a path to it (Maze blocked
      // elsewhere). Standard BFS behavior (retry or wait).
      Serial.println("[NAV] Path blocked (Sanctuary clear). Retrying BFS...");
    }
  }
}

void LabyrinthTask::handleDropBox() {
  Serial.println("State: DROP_BOX");

  if (currentPos != sanctuaryLocation) {
    Serial.println("[DROP] Not at Standing Point! Correcting...");
    if (moveTo(sanctuaryLocation))
      return;
  }

  // 2. Determine where the Void/Obstacle is relative to us
  // Robot is at (2,7). Target is (2,8).
  char faceTargetHeading = 'N';

  if (dropTarget.x > currentPos.x)
    faceTargetHeading = 'E';
  else if (dropTarget.x < currentPos.x)
    faceTargetHeading = 'W';
  else if (dropTarget.y > currentPos.y)
    faceTargetHeading = 'N'; // This triggers!
  else if (dropTarget.y < currentPos.y)
    faceTargetHeading = 'S';

  // 3. Turn to face the void (North)
  if (currentHeading != faceTargetHeading) {
    turnTo(faceTargetHeading);
  }

  // 4. Perform the Drop
  // Robot is at (2,7) facing North (towards 2,8).
  // It turns Right (East), Drops, Turns Left (North).
  Serial.println("[DROP] Dropping into obstacle zone...");
  motorControl.moveForward(320);
  motorControl.turnRight(90);
  motorControl.moveBackward(320);
  motorControl.disableMotors();
  armTask.dropBox();
  motorControl.enableMotors();
  motorControl.moveBackward(320);
  motorControl.turnRight(90);
  motorControl.moveBackward(320);
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  motorControl.stop();
  armTask.pickupBox();
  motorControl.moveForward(320);
  motorControl.turnLeft(90);
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  motorControl.stop();
  motorControl.moveForward(320);
  motorControl.turnLeft(90);
  motorControl.moveForward(450);
  while (!sensors.isFourWay()) {
    nav.followLine(1000);
  }
  motorControl.stop();

  // 5. Cleanup
  boxesCollected++;
  if (boxesCollected < totalBoxes) {
    if (searchIndex < TOTAL_CELLS)
      currentState = STATE_NAVIGATE_TO_BOX;
    else
      currentState = STATE_NAVIGATE_TO_EXIT;
  } else {
    currentState = STATE_NAVIGATE_TO_EXIT;
  }
}

void LabyrinthTask::handleNavigateToExit() {
  if (currentPos == exitLocation) {
    currentState = STATE_FINISH;
  } else {
    moveTo(exitLocation);
  }
}

void LabyrinthTask::handleFinish() {
  Serial.println("State: FINISH");
  nav.update();

  int missing = getMissingColor();

  if (missing == COLOR_RED)
    Serial.println(">>> OUTPUT: The missing color is RED <<<");
  else if (missing == COLOR_GREEN)
    Serial.println(">>> OUTPUT: The missing color is GREEN <<<");
  else if (missing == COLOR_BLUE)
    Serial.println(">>> OUTPUT: The missing color is BLUE <<<");
}


int LabyrinthTask::getMissingColor() {
  Serial.print("[DEBUG] Collected Colors: ");
  Serial.print(collectedColors[0]);
  Serial.print(", ");
  Serial.println(collectedColors[1]);

  bool hasRed =
      (collectedColors[0] == COLOR_RED || collectedColors[1] == COLOR_RED);
  bool hasGreen =
      (collectedColors[0] == COLOR_GREEN || collectedColors[1] == COLOR_GREEN);
  bool hasBlue =
      (collectedColors[0] == COLOR_BLUE || collectedColors[1] == COLOR_BLUE);

  if (!hasRed)
    return 0;
  else if (!hasGreen)
    return 1;
  else if (!hasBlue)
    return 2;
  else
    return -1;
}

ObstacleType LabyrinthTask::detectObstacleAndScan() {
  // 1. Scan Environment and Update Grid (Scanned Empty)
  int frontX = currentPos.x;
  int frontY = currentPos.y;

  if (currentHeading == 'N')
    frontY++;
  else if (currentHeading == 'S')
    frontY--;
  else if (currentHeading == 'E')
    frontX++;
  else if (currentHeading == 'W')
    frontX--;

  // Read Sensor ONCE
  bool obstacleFront = sensors.isObstacleFront();

  // Update Front (Empty)
  // if (!obstacleFront) {
  //   grid.setScannedEmpty(frontX, frontY);
  // }

  if (obstacleFront) {
    Serial.println("[OBSTACLE] Detected Front!");

    int boxX = currentPos.x;
    int boxY = currentPos.y;

    if (currentHeading == 'N')
      boxY++;
    else if (currentHeading == 'S')
      boxY--;
    else if (currentHeading == 'E')
      boxX++;
    else if (currentHeading == 'W')
      boxX--;

    if (grid.isObstacle(boxX, boxY)) {
      // FIX: Allow re-evaluation if this obstacle IS our current target
      // This happens if we tried one side, failed, marked the blocking wall,
      // and now need to retry the box (which is already 'Obstacle') to try the
      // other side.
      Point detected = {boxX, boxY};
      if (detected != boxLocation) {
        Serial.println("[NAV] Known obstacle ahead (Not Target). Skipping.");
        return ObstacleType::WALL;
      } else {
        Serial.println("[NAV] Known obstacle IS Target. Re-evaluating for "
                       "alternate path...");
      }
    }

    // APPROACH AND MEASURE LOGIC
    Serial.println("[OBSTACLE] Approaching to 3cm for Identification...");
    long stepsMoved = 0;
    while (sensors.getDistance(IR_FRONT_PIN) > 3.0) {
      motorControl.moveForward(50); // Move 0.5cm steps
      while (motorControl.isRunning())
        motorControl.run();
      stepsMoved += 50;

      if (stepsMoved > 400)
        break; // Timeout
    }

    // Check Height Sensor
    bool isTall = (sensors.getObstacleDistance() > HEIGHT_SENSOR_THRESHOLD);

    // Retreat
    motorControl.moveBackward(stepsMoved); // Return exactly
    while (motorControl.isRunning())
      motorControl.run();

    if (isTall || currentState != STATE_NAVIGATE_TO_BOX) {
      Serial.println("[DECISION] Obstacle is TALL (Wall). Marking...");
      grid.setObstacle(boxX, boxY);
      return ObstacleType::WALL;
    } else {
      Serial.println(
          "[DECISION] Obstacle is SHORT (Box). Calculating Approach.");
      grid.setObstacle(boxX, boxY); // Ensure it remains marked as obstacle/box
      // HUTTHOOOOOOOOOOOOOoooooooo
      // === MANUAL TRIGGER (User Requested) ===
      Serial.println(
          "[ALIGN] Box Detected. Force-Delegating to Manual Maneuver.");
      alignmentTarget = currentPos;
      alignmentHeading = currentHeading;
      selectedApproach = SIDE_NONE; // Default to None
      return ObstacleType::BOX_NEW;
    }
  }
  return ObstacleType::NONE;
}

bool LabyrinthTask::moveTo(Point target) {
  ObstacleType obs = detectObstacleAndScan();
  if (obs == ObstacleType::BOX_NEW) {
    currentState = STATE_ALIGN_FOR_PICKUP;
    return false;
  }
  // Removed premature 'WALL' return to allow BFS to find alternate path (e.g.
  // Turn) else if (obs == ObstacleType::WALL) { return false; }

  char moveDir = grid.getNextMoveBFS(currentPos, target);

  if (moveDir == 'X') {
    Serial.println("[BFS] No path found!");
    return false;
  }

  int nextX = currentPos.x;
  int nextY = currentPos.y;
  if (moveDir == 'N')
    nextY++;
  else if (moveDir == 'S')
    nextY--;
  else if (moveDir == 'E')
    nextX++;
  else if (moveDir == 'W')
    nextX--;

  if (grid.isObstacle(nextX, nextY)) {
    Serial.println("[NAV] Safety: BFS returned move into obstacle! Aborting.");
    return false;
  }

  Serial.print("[MOVE] Next: ");
  Serial.println(moveDir);
  turnTo(moveDir);

  obs = detectObstacleAndScan();
  if (obs == ObstacleType::BOX_NEW) {
    currentState = STATE_ALIGN_FOR_PICKUP;
    return false;
  } else if (obs == ObstacleType::WALL) {
    return false;
  }

  if (!nav.moveCells(1)) {
    Serial.println("[Move] Blocked mid-move!");
    obs = detectObstacleAndScan();
    if (obs == ObstacleType::BOX_NEW) {
      currentState = STATE_ALIGN_FOR_PICKUP;
    }
    return false;
  }
  Serial.println("[MOVE] Cell move complete.");
  updatePosition(moveDir);

  Serial.print("[ARRIVED] Cell: ");
  Serial.print(currentPos.x);
  Serial.print(",");
  Serial.println(currentPos.y);
  return true;
}

void LabyrinthTask::turnTo(char direction) {
  if (currentHeading == direction)
    return;

  if (currentHeading == 'N') {
    if (direction == 'E')
      nav.turnIntersection('R');
    else if (direction == 'W')
      nav.turnIntersection('L');
    else if (direction == 'S') {
      nav.turnIntersection('B');
    }
  } else if (currentHeading == 'E') {
    if (direction == 'S')
      nav.turnIntersection('R');
    else if (direction == 'N')
      nav.turnIntersection('L');
    else if (direction == 'W') {
      nav.turnIntersection('B');
    }
  } else if (currentHeading == 'S') {
    if (direction == 'W')
      nav.turnIntersection('R');
    else if (direction == 'E')
      nav.turnIntersection('L');
    else if (direction == 'N') {
      nav.turnIntersection('B');
    }
  } else if (currentHeading == 'W') {
    if (direction == 'N')
      nav.turnIntersection('R');
    else if (direction == 'S')
      nav.turnIntersection('L');
    else if (direction == 'E') {
      nav.turnIntersection('B');
    }
  }

  currentHeading = direction;
}

void LabyrinthTask::updatePosition(char moveDir) {
  if (moveDir == 'N')
    currentPos.y++;
  else if (moveDir == 'S')
    currentPos.y--;
  else if (moveDir == 'E')
    currentPos.x++;
  else if (moveDir == 'W')
    currentPos.x--;
}

Point LabyrinthTask::findBestSanctuary(Point target) {
  Point bestSpot = {-1, -1};
  int minDistance = 9999;

  // Neighbor offsets: North, East, South, West
  int dx[] = {0, 1, 0, -1};
  int dy[] = {1, 0, -1, 0};

  for (int i = 0; i < 4; i++) {
    int nx = target.x + dx[i];
    int ny = target.y + dy[i];

    // Check 1: Is the spot inside the grid?
    // Check 2: Is the spot free of known obstacles?
    if (grid.isValid(nx, ny) && !grid.isObstacle(nx, ny)) {

      // Calculate Manhattan Distance from CURRENT robot position
      int dist = abs(currentPos.x - nx) + abs(currentPos.y - ny);

      if (dist < minDistance) {
        minDistance = dist;
        bestSpot = {nx, ny};
      }
    }
  }

  // FAILSAFE: If all spots are blocked or invalid, default to South (y-1).
  // This prevents the robot from crashing with a {-1, -1} coordinate.
  if (bestSpot.x == -1) {
    Serial.println("[ERR] Target surrounded! Defaulting South.");
    return {target.x, target.y - 1};
  }

  return bestSpot;
}