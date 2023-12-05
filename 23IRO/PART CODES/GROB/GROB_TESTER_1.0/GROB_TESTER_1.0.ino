#include <Servo.h> // lib

Servo zAxis; // 180 degree turn
Servo yAxis; // 4-bar mechanism for lifting

Servo rightGrip; // right servo of the gripper
Servo leftGrip; // left servo of the gripper

int const baseTurnFacingFront = 0; // default value
int const baseTurnResting = 0;

int const gripSetCloseRight = 80; // default value
int const gripSetCloseLeft = 100; // default value
int const gripSetOpenRight = 150;
int const gripSetOpenLeft = 30;

int const gripLiftLowest = 140; // default value
int const gripLiftFlat = 90;
int const gripLiftHighest = 70;

void setup() 
{
  Serial.begin(9600);

  Serial.println("SERVO SETUP");
  delay(300);
  zAxis.attach(2);
  Serial.println()("[zAxis | D/O(2)]: COMPLETED");
  yAxis.attach(3);
  delay(300);
  Serial.println("[yAxis | D/O(3)]: COMPLETED");
  leftGrip.attach(5);
  delay(300);
  Serial.println("[leftGrip | D/O(5)]: COMPLETED");
  rightGrip.attach(6); 
  delay(300);
  Serial.println()("[rightGrip | D/O(6)]: COMPLETED");
  delay(200);
  Serial.println()("DONE");

  Serial.print("SERVO INITIALIZING");
  delay(300);
  Serial.print(".");
  delay(300);
  liftControl(0);
  Serial.print(".");
  delay(300);
  gripControl(0);
  Serial.print(".");
  delay(300);
  Serial.println("SERVO INITIALIZED");
}

void baseControl(char cDir) // base turn Control
{
  switch(cDir)
  {
    case 'u': // facing front
      zAxis.write(baseTurnFacingFront);
      delay(500);
      break;

    case 'n': // resting
      zAxis.write(baseTurnResting);
      delay(500);
      break;
  }
}

void gripControl(int nStatus) // gripper control
{
  switch(nStatus)
  {
    case 0: // close
      for(int angR = rightGrip.read(); angR < 100; angR++)
      {
        leftGrip.write(angR);
        rightGrip.write(map(angR,0,180,180,0));
        delay(15);
      }
      Serial.println("Servo[CLOSE]");
      break;

    case 1: // open
      for(int angR = rightGrip.read(); angR >= 30; angR--)
      {
        leftGrip.write(angR);
        rightGrip.write(map(angR,0,180,180,0));
        delay(20);
      }
      Serial.println("Servo[OPEN]");
      break;
  }
}

void liftControl(int nLevel) // lifter control
{
  switch(nLevel)
  {
    case 0: // Lowest
      for(int angR = yAxis.read(); angR < 130; angR++)
      {
        yAxis.write(angR);
        delay(15);
      }
      break;

    case 1: // Flat(mid)
      if(yAxis.read() > 90)
      {
        for(int angR = yAxis.read(); angR > 90; angR--)
        {
          yAxis.write(angR);
          delay(15);
        }
      }
      else if(yAxis.read() < 90)
      {
        for(int angR = yAxis.read(); angR < 90; angR++)
        {
          yAxis.write(angR);
          delay(15);
        }
      }
      break;

    case 2: // Highest
      for(int angR = yAxis.read(); angR > 50; angR--)
      {
        yAxis.write(angR);
        delay(15);
      }
      break;
  }
}


void loop() 
{
  liftControl(2);
  gripControl(1);
  delay(5000);
  liftControl(0);
  gripControl(0);
}
