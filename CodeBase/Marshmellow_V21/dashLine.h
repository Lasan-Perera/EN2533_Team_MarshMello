#pragma once


extern const int LEFT_IR;
extern const int RIGHT_IR;

extern bool leftCorner;
extern bool rightCorner;

// extern int irValues[8];
// extern bool irBits[8]; 
extern int weight_[8];


void dashLineXramp();
void dashedLineFollow(float kp_, float kd_);