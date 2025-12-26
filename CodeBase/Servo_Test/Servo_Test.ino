#include <ServoTimer2.h>

// ---------------- SERVO OBJECTS ----------------
ServoTimer2 servoBig;
ServoTimer2 servoSmall1;
ServoTimer2 servoSmall2;

// ---------------- DEG → MICROSECOND MAP ----------------
int degToPulse(int angle) {
  return map(angle, 0, 180, 500, 2400);   // valid range for ServoTimer2
}

// ---------------- SWEEP ONE SERVO ----------------
void sweepServo(ServoTimer2 &s, int startAngle, int endAngle, int speed = 10) {
  int startPulse = degToPulse(startAngle);
  int endPulse   = degToPulse(endAngle);

  if (startPulse < endPulse) {
    for (int pulse = startPulse; pulse <= endPulse; pulse += 10) {
      s.write(pulse);
      delay(speed);
    }
  } else {
    for (int pulse = startPulse; pulse >= endPulse; pulse -= 10) {
      s.write(pulse);
      delay(speed);
    }
  }
}

// ---------------- SWEEP TWO SERVOS TOGETHER ----------------
void sweepGrabber(int start1, int end1, int start2, int end2, int speed = 10) {
  int p1 = degToPulse(start1);
  int p2 = degToPulse(start2);

  int t1 = degToPulse(end1);
  int t2 = degToPulse(end2);

  int dir1 = (t1 > p1) ? 10 : -10;
  int dir2 = (t2 > p2) ? 10 : -10;

  while (p1 != t1 || p2 != t2) {
    if (p1 != t1) p1 += dir1;
    if (p2 != t2) p2 += dir2;

    servoSmall1.write(p1);
    servoSmall2.write(p2);

    delay(speed);
  }
}

// ---------------- MOVEMENT FUNCTIONS ----------------
void armDown() {
  sweepServo(servoBig,  servoBigAngle(), 0);
}

void armUp() {
  sweepServo(servoBig, servoBigAngle(), 180);
}

void grabberOpen() {
  sweepGrabber(getAngle(servoSmall1), 20, getAngle(servoSmall2), 140, 10);
}

void grabberClose() {
  sweepGrabber(getAngle(servoSmall1), 130, getAngle(servoSmall2), 45, 10);
}

// ---------- Helper to read current angle ----------
int getAngle(ServoTimer2 &s) {
  int pulse = s.read();
  return map(pulse, 500, 2400, 0, 180);
}

int servoBigAngle() {
  int pulse = servoBig.read();
  return map(pulse, 500, 2400, 0, 180);
}

// ---------------- SETUP ----------------
void setup() {
  servoBig.attach(7);      // CHANGE PIN IF NEEDED
  servoSmall1.attach(3);
  servoSmall2.attach(5);

  // Initialize to safe positions
  servoBig.write(degToPulse(90));
  servoSmall1.write(degToPulse(80));
  servoSmall2.write(degToPulse(110));

  delay(500);
}

// ---------------- MAIN LOOP ----------------
void loop() {
  // Example demo
  armDown();
  delay(500);

  grabberOpen();
  delay(500);

  grabberClose();
  delay(500);

  armUp();
  delay(500);
}
