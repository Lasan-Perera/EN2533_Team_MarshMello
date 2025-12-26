#include "motors.h"
#include "helperFunctions.h"
#include "sensors.h"
#include "lineFollow.h"
#include "ballCollection.h"
#include "dashLine.h"
#include "barcodeRead.h"
#include "arrowLine.h"
#include "armControl.h"
#include "ArmControlTask.h"
#include "LabyrinthTask.h"
#include "MotorControl.h"
#include "Navigation.h"
#include "RobotConfig.h"
#include "SensorsTask.h"

enum TaskMode {
  MODE_MAZE,      // normal arrow/line follow
  MODE_DASHRAMP,  // line lost, move straight until we find line again
  MODE_BARCODE,
  MODE_BALLS,
  MODE_ARROW,
  MODE_WALLS,
  MODE_SHOOT  // turning to side with more white
};

MotorControl motorControl;
SensorsTask sensors;
Navigation navigation(motorControl, sensors);
ArmControlTask armControlTask;
LabyrinthTask task(navigation, armControlTask, sensors, motorControl);
int missing;

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println(">>> ROBOT INITIALIZING <<<");

  motorControl.init();
  Serial.println("HAL: Motor Control Initialized");
  motorControl.enableMotors();  // Explicit enable

  // HARDWARE TEST: Move forward 100 steps blindly to verify motors
  // Serial.println("TESTING: Move Forward 400 steps...");
  // motorControl.moveForward(400);
  // delay(1000);
  // Serial.println("TESTING: Move Backward 400 steps...");
  // motorControl.moveBackward(400);
  // delay(1000);

  sensors.init();
  Serial.println("HAL: SensorsTask Initialized");
  Serial.flush();

  armControlTask.init();
  Serial.println("HAL: Arm Control Initialized");
  armControlTask.disable();  // Detach to save power/prevent conflict

  delay(500);
  motorControl.init();
  motorControl.enableMotors();
  Serial.println("HAL: Motor Control Re-asserted");

  task.init();
  Serial.println("HAL: Task Initialized");
  Serial.flush();

  Serial.println(">>> SYSTEM READY. STARTING TASK <<<");
  setupIRSensors();
  setupDistanceSensors();
  delay(500);


  // dashLineXramp();
  // forward(300);
  // while (!isFourWay()){
  //   lineFollow90Bends(1, 0, 0);
  // }
  // resetPID();
  // forward(350);
  // right90();
  // collectBalls();
  // forward(350);
  // right90();
  // while(!isFourWay()) {
  //   arrowFollowLoop();
  // }
}


TaskMode currentMode = MODE_MAZE;
bool arrowDone = false;
bool mazeDone = false;

void loop() {
  switch (currentMode) {
    case MODE_MAZE:
      static bool calibrated = false;
      if (!calibrated) {
        task.recorrection();
        calibrated = true;
      }
      if (mazeDone) {
        missing = task.getMissingColor();
        setupMotors();
        setupIRSensors();
        // setupServos();
        setupColorSensor();
        setupDistanceSensors();
        setupArm();
        delay(1000);
        armUp();
        grabberOpen();
        forward(-800);
        gofow(-1);
        forward(700);
        left90();
        forward(500);
        currentMode = MODE_DASHRAMP;
      }
      task.run();
      mazeDone = task.taskDone();

      break;

    case MODE_DASHRAMP:
      dashLineXramp();
      currentMode = MODE_BARCODE;
      break;

    case MODE_BARCODE:
      // while (!isFourWay() && barcodeFinished()) {
      //   lineFollow(3.9, 0, 1.85);
      //   readSensors();
      //   if (!barcodeFinished()) {
      //     barcodeUpdate(sensorValues, stepperR.currentPosition());
      //   } else {
      //     sphereCount = getSphereCount();
      //   }
      // }
      // while (!isFourWay()) {
      //   if(sensorValues[0] < 300 || sensorValues[1] < 300) {
      //     right90();
      //   }
      //   // dashedLineFollow(100, 100);
      //   lineFollow(3.9, 0, 1.85);
      // }
      forward(800);
      right90();
      while (!isFourWay()) {
        lineFollow(3.9, 0, 1.85);
      }
      stopRobot();
      forward(1500);
      arm.indication();
      while (!isFourWay()) {
        lineFollow(3.9, 0, 1.85);
      }
      forward(350);
      right90();
      currentMode = MODE_BALLS;
      break;

    case MODE_BALLS:
      while (getDistance(frontIR) > 6) {
        lineFollow(3.9, 0, 1.85);
      }
      turnAngle(500);
      while (getDistance(rightStraightIR) < 15) {
        wallFollow(6, 0, 1.85, 240);
        if (getDistance(rightStraightIR) > 10) {
          stepperL.setAcceleration(1000.0);
          stepperR.setAcceleration(1000.0);
          stepperL.move(850);
          stepperR.move(350);
          while (stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
            stepperL.run();
            stepperR.run();
          }
          forward(900);
          right90();
          forward(800);
          break;
        }
      }
      collectBallsHexagon(missing);
      while (!isFourWay()) {
        wallFollow(6, 0, 2.85, 240);
      }
      stopRobot();
      forward(350);
      left90();
      while (!isFourWay()) {
        lineFollow(3.9, 0, 1.85);
      }
      forward(350);
      right90();
      currentMode = MODE_ARROW;
      break;

    case MODE_ARROW:
      arrowFollowLoop();
      break;

    case MODE_WALLS:

      break;

    case MODE_SHOOT:

      break;
  }
}
