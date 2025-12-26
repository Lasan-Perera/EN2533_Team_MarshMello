#pragma once
#include <Arduino.h>
// Number of sensors
#define NUM_SENSORS 8

extern int weights[NUM_SENSORS];

// Function declarations
bool isFourWay();
float getLinePosition();
void gofow(int intersections);
