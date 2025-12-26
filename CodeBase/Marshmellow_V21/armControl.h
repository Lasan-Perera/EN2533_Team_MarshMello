#include <Servo.h>

class ArmControl {
public:
  ArmControl();
  void init();
  void enable();
  void disable();
  void pickupBox();
  void preparePickup();
  void dropBox();
  void home();
  void run(); // Wait for completion
  void prepareBall();
  void pickupBall();
  void dropBall();
  void dropBallNoneed();
  void indication();
  
private:
  struct ServoMotion {
    Servo *s;
    int target;
    int speed; // ms per step
    unsigned long lastStep = 0;
    bool active = false;
  };

  Servo servoBig;    // Arm
  Servo servoSmall1; // Gripper 1
  Servo servoSmall2; // Gripper 2

  ServoMotion armMotion;
  ServoMotion grab1Motion;
  ServoMotion grab2Motion;

  void startMove(ServoMotion &m, int target, int speed);
  void updateMove(ServoMotion &m);

  void grabberAngle(int angle);
  void armAngle(int angle);
};

void setupArm();

extern ArmControl arm;


