#ifndef LABYRINTH_TASK_H
#define LABYRINTH_TASK_H

#include "ArmControlTask.h"
#include "Grid.h"
#include "Navigation.h"
#include "SensorsTask.h"
#include <Arduino.h>

enum TaskState {
  STATE_START,
  STATE_NAVIGATE_TO_BOX,
  STATE_SCAN_BOX,
  STATE_ALIGN_FOR_PICKUP,
  STATE_PICKUP_BOX,
  STATE_NAVIGATE_TO_SANCTUARY,
  STATE_DROP_BOX,
  STATE_NAVIGATE_TO_EXIT,
  STATE_FINISH
};

enum ObstacleType { NONE, WALL, BOX_NEW };

class LabyrinthTask {
public:
  LabyrinthTask(Navigation &nav, ArmControlTask &armTask, SensorsTask &sensors,
                MotorControl &motorControl);
  void init();
  void run();
  void setInitialPosition(Point startPos, char startHeading);
  bool taskDone();
  // Helpers - Exposed for Testing
  void manualManeuverRight();
  void manualManeuverLeft();
  bool recorrection();
  int getMissingColor();

private:
  Navigation &nav;
  ArmControlTask &armTask;
  SensorsTask &sensors;
  MotorControl &motorControl;
  Grid grid;

  TaskState currentState;

  // Position State
  Point currentPos;
  char currentHeading; // 'N', 'E', 'S', 'W'

  // Targets
  Point boxLocation;                     // Current target (Search Waypoint)
  Point searchWaypoints[TOTAL_CELLS];    // Granular Snake pattern (Every cell)
  Point findBestSanctuary(Point target); // Add this line
  int searchIndex;
  Point sanctuaryLocation;
  Point dropTarget;
  Point exitLocation;
  Point alignmentTarget;
  char alignmentHeading;

  enum ApproachSide { SIDE_NONE, SIDE_LEFT, SIDE_RIGHT };
  ApproachSide selectedApproach;

  int boxesCollected;
  const int totalBoxes = 2; // Green and Blue (or Red/Green etc)

  Color detectedColor;
  Color collectedColors[2]; // Track collected colors for inference

  // Helpers
  void updateState();
  void executeState();
  bool moveTo(Point target);
  void turnTo(char direction);
  void updatePosition(char moveDir);
  char getHeadingFromDirection(char moveDir);
  ObstacleType detectObstacleAndScan(); // Returns obstacle type

  // Private State Handlers
  void handleStart();
  void handleNavigateToBox();
  void handleScanBox();
  void handleAlignForPickup();
  void handlePickupBox();
  void handleNavigateToSanctuary();
  void handleDropBox();
  void handleNavigateToExit();
  void handleFinish();

  // Logic Helpers
  void advanceSearchTarget();
  void performPickupManeuver();
};

#endif // LABYRINTH_TASK_H
