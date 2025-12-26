#include "SensorsTask.h"
#include "RobotConfig.h"

SensorsTask::SensorsTask() : tcs(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X) {}

void SensorsTask::init() {
  // Initialize Line Sensor Pins
  for (int i = 0; i < 8; i++) {
    pinMode(LINE_SENSOR_PINS[i], INPUT);
  }

  // Initialize Sharp IR Pins
  pinMode(IR_FRONT_PIN, INPUT);
  pinMode(IR_LEFT_FRONT_PIN, INPUT);
  pinMode(IR_LEFT_BACK_PIN, INPUT);
  pinMode(IR_RIGHT_FRONT_PIN, INPUT);
  pinMode(IR_RIGHT_BACK_PIN, INPUT);

  // Initialize Height Sensor
  pinMode(HEIGHT_SENSOR_PIN, INPUT);

  // Initialize Color Sensor
  if (!tcs.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
  }

  // Generate Sharp IR Lookup Table (LUT)
  // Distance (cm) = 5.2819 * Volts^-1.161
  // Volts = ADC * (5.0 / 1023.0)
  Serial.println("Generating Sensor LUT...");
  for (int i = 0; i < 1024; i++) {
    if (i < 10) {
      distanceLUT[i] = 255; // Too far (using 255 as max/invalid for uint8)
      continue;
    }
    float volts = i * (5.0 / 1023.0);
    int dist = (int)(5.2819 * pow(volts, -1.161));
    if (dist > 255)
      dist = 255;
    if (dist < 0)
      dist = 0;
    distanceLUT[i] = (uint8_t)dist;
  }
}

// Weights for 8 sensors (User provided: -50 to +50)
int weights_[8] = {-50, -35, -15, -5, +5, +15, +35, +50};

float SensorsTask::getLinePosition() {
  long sumWeighted = 0;
  long sumValues = 0;
  for (int i = 0; i < 8; i++) {
    int value = 1023 - analogRead(LINE_SENSOR_PINS[i]); // white line Black bg
    sumWeighted += (long)value * weights_[i];
    sumValues += value;
  }
  if (sumValues == 0)
    return 0; // error handling
  return sumWeighted / (float)sumValues;
}

bool SensorsTask::isFourWay() {
  // User code: if (sensorValues[i] > 250) return false;
  // If any sensor is > 250 (Black?), return false.
  // So if ALL are <= 250 (White?), return true.
  // This implies checking for a cross intersection (all white).

  for (int i = 0; i < 8; i++) {
    if (analogRead(LINE_SENSOR_PINS[i]) > 250)
      return false;
  }
  return true;
}

float SensorsTask::getDistance(int sharpIRPin) { return readSharpIR(sharpIRPin); }

bool SensorsTask::isObstacleFront() {
  float distance = getDistance(IR_FRONT_PIN);
  // Serial.print("IR Front: ");
  // Serial.println(distance);
  return distance < 15; // Increased threshold to reduce false positives
}

bool SensorsTask::isObstacleLeft() {
  Serial.print("IR Left: ");
  Serial.println(getDistance(IR_LEFT_FRONT_PIN));
  return getDistance(IR_LEFT_FRONT_PIN) < 15 ||
         getDistance(IR_LEFT_BACK_PIN) < 15;
}

bool SensorsTask::isObstacleRight() {
  Serial.print("IR Right: ");
  Serial.println(getDistance(IR_RIGHT_FRONT_PIN));
  return getDistance(IR_RIGHT_FRONT_PIN) < 15 ||
         getDistance(IR_RIGHT_BACK_PIN) < 15;
}

float SensorsTask::getObstacleDistance() {

  bool state_prox = digitalRead(HEIGHT_SENSOR_PIN);
  return state_prox ? 0.0 : 100.0; // Active LOW?
}

Color SensorsTask::readColor() {
  int redCount = 0;
  int greenCount = 0;
  int blueCount = 0;
  int noneCount = 0;

  Serial.println("[SENSORS] Sampling Color (5x)...");

  for (int i = 0; i < 5; i++) {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    // delay(20); // Small integration/stability delay
    Serial.print("Color: ");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.println(b);

    if (c < 10) {
      noneCount++;
    } else if (r > g && r > b) {
      redCount++;
    } else if (g > r && g > b) {
      greenCount++;
    } else if (b > r && b > g) {
      blueCount++;
    } else {
      noneCount++;
    }
  }

  // Return Majority
  // Return Majority
  if (redCount >= greenCount && redCount >= blueCount && redCount > 0) {
    Serial.println("Result: RED");
    return COLOR_RED;
  }
  if (greenCount >= redCount && greenCount >= blueCount && greenCount > 0) {
    Serial.println("Result: GREEN");
    return COLOR_GREEN;
  }
  if (blueCount >= redCount && blueCount >= greenCount && blueCount > 0) {
    Serial.println("Result: BLUE");
    return COLOR_BLUE;
  }

  return COLOR_NONE;
}

int SensorsTask::readSharpIR(uint8_t pin) {
  // 3-sample average
  long total = 0;
  for (int i = 0; i < 5; i++) {
    total += analogRead(pin);
    // Removed delay(5) to prevent blocking motor loop
  }
  int val = total / 5;

  // Use Lookup Table
  return distanceLUT[val];
}

void SensorsTask::printDebugValues() {
  Serial.print("Line: [");
  for (int i = 0; i < 8; i++) {
    Serial.print(analogRead(LINE_SENSOR_PINS[i]));
    if (i < 7)
      Serial.print(", ");
  }
  Serial.print("] | IR Front: ");
  Serial.println(analogRead(IR_FRONT_PIN));
}
