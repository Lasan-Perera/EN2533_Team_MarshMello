#pragma once

// PID variables
extern float error;
extern float previousError;
extern float integral;
extern float derivative;
extern float correction;

bool isFourWay();
float getLinePosition();
void lineFollow(float Kp, float Ki, float Kd);
void lineFollow90Bends(float Kp, float Ki, float Kd);
void wallFollow(float Kp, float Ki, float Kd, float targetDistance);
void resetPID();