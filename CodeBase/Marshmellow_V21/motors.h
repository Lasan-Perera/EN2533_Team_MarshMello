#pragma once
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Arduino.h>
#include <Servo.h>

extern Servo servoBig;      
extern Servo servoSmall1;   // pin 3
extern Servo servoSmall2;   // pin 5

// Motor control variables
extern int baseSpeed;
extern int maxSpeed;

// Stepper motor objects
extern AccelStepper stepperL;
extern AccelStepper stepperR;
extern MultiStepper steppers;

// Function declarations
void setupMotors();
void setupServos();
void right90();
void left90();
void forward(int steps);
void applyMotorSpeed(float correction);
void stopRobot();
void sweepServo(Servo &s, int startAngle, int endAngle, int speed = 10);
void sweepGrabber(int start1, int end1, int start2, int end2, int steps = 1, int speed = 10);
void armDown();
void armUp();
void grabberOpen();
void grabberClose();
void turnAngle(int steps, int dir = 1);
void applyMotorSpeed_slow(float correction);
void leftTurn(int turnSteps = 267);
void right60();

//  Left stepper pins
#define L0 45
#define L1 43
#define L2 41
#define StepPinL 39
#define DirPinL 37
#define EnPinL 47

//  Right stepper pins
#define R0 44
#define R1 42
#define R2 40
#define StepPinR 38
#define DirPinR 36
#define EnPinR 46

#define armServo 7
#define gripperLeft 3
#define gripperRight 5