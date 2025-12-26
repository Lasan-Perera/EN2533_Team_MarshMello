#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <Arduino.h>

// --- Stepper Motors (AccelStepper) ---
// --- Stepper Motors (AccelStepper) ---
// Left Stepper
#define L0 45
#define L1 43
#define L2 41
#define StepPinL 39
#define DirPinL 37
#define EnPinL 47

// Right Stepper
#define R0 44
#define R1 42
#define R2 40
#define StepPinR 38
#define DirPinR 36
#define EnPinR 46

#define STEPPER_MAX_SPEED 1500
#define STEPPER_ACCELERATION 1000

// --- Line Sensor Array (8 SensorsTask) ---
// Assuming analog pins for now, adjust as needed
const uint8_t LINE_SENSOR_PINS[] = { A0, A2, A4, A6, A8, A10, A12, A14 };
#define LINE_THRESHOLD 350

// --- Sharp IR SensorsTask (Obstacle Detection) ---
#define IR_FRONT_PIN A1
#define IR_LEFT_FRONT_PIN A11
#define IR_LEFT_BACK_PIN A3
#define IR_RIGHT_FRONT_PIN A5
#define IR_RIGHT_BACK_PIN A9

// --- Height Sensor (Obstacle vs Object) ---
#define HEIGHT_SENSOR_PIN 26  // Digital pin for 14cm height sensor

// --- Servos ---
// --- Servos ---
#define SERVO_ARM_PIN 7
#define SERVO_GRIPPER_1_PIN 8
#define SERVO_GRIPPER_2_PIN 5

// --- Color Sensor (TCS34725) ---
// I2C pins (SDA, SCL) are standard on Arduino Mega (20, 21)

// --- Robot Physical Constants ---
#define WHEEL_DIAMETER_CM 8.5
#define WHEEL_BASE_CM 17
#define STEPS_PER_REV 800

// --- Grid Constants ---
#define CELL_SIZE_CM 25.0
#define LINE_WIDTH_CM 3.0

// --- Drop Locations (X, Y) ---
// Note: Use initializer list format for Point struct assignment
#define DROP_LOCATION_BLUE \
  { 2, 8 }
#define DROP_LOCATION_GREEN \
  { 4, 8 }
#define DROP_LOCATION_RED \
  { 6, 8 }

// --- Predefined Obstacles ---
struct ObstacleCoord {
  int x;
  int y;
};

// Add your static obstacles here
const ObstacleCoord PREDEFINED_OBSTACLES[] = {
  { 3, 8 }, { 5, 8 }
  // {1, 5}, // Example: Pillar at (1,5)
};

const int PREDEFINED_OBSTACLES_COUNT =
  sizeof(PREDEFINED_OBSTACLES) / sizeof(ObstacleCoord);

#endif  // ROBOT_CONFIG_H
