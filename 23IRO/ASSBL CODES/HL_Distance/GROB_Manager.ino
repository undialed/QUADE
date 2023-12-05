// ____________________________________ PRE-SETUP ______________________________________
#include <Servo.h> // lib

Servo zAxis; // 180 degree turn                         [ServoType = 1]
Servo yAxis; // 4-bar mechanism for lifting             [ServoType = 2]

Servo rightGrip; // right servo of the gripper          [ServoType = 3]
Servo leftGrip; // left servo of the gripper            [ServoType = 4]

int const baseTurnResting = 30; // default value
int const baseTurnFacingFront = 160; 

int const gripSetCloseRight = 80; // default value
int const gripSetCloseLeft = 100; // default value
int const gripSetOpenRight = 150;
int const gripSetOpenLeft = 30;

int const gripLiftLowest = 140; // default value
int const gripLiftFlat = 90;
int const gripLiftHighest = 70;

int nCurrentGripAngle = 0;

// ____________________________________ PRE-SETUP ______________________________________

// ______________________________________ SETUP ________________________________________
// void setup() 
// {
//   Serial.begin(9600);

//   Serial.println("SERVO SETUP");
//   delay(300);
//   zAxis.attach(2);
//   Serial.println("[zAxis | D/O(2)]: COMPLETED");
//   yAxis.attach(3);
//   delay(300);
//   Serial.println("[yAxis | D/O(3)]: COMPLETED");
//   leftGrip.attach(5);
//   delay(300);
//   Serial.println("[leftGrip | D/O(5)]: COMPLETED");
//   rightGrip.attach(6); 
//   delay(300);
//   Serial.println("[rightGrip | D/O(6)]: COMPLETED");
//   delay(200);
//   Serial.println("DONE");

//   Serial.print("SERVO INITIALIZING");
//   zAxis.write(25); // base resting
//   Serial.print(".");
//   yAxis.write(90); // lift - flattened
//   Serial.print(".");
//   gripCall(1); // grip - closed
//   Serial.println(".");
//   delay(300);
//   Serial.println("SERVO INITIALIZED");
// }
// ______________________________________ SETUP ________________________________________



// _____________________________________ FUNCTIONS _____________________________________

// _____________________________________ GROB SETUP ____________________________________
void grobSetUp()
{
  Serial.println("SERVO SETUP");
  delay(300);
  zAxis.attach(2);
  Serial.println("[zAxis | D/O(2)]: COMPLETED");
  yAxis.attach(3);
  delay(300);
  Serial.println("[yAxis | D/O(3)]: COMPLETED");
  leftGrip.attach(5);
  delay(300);
  Serial.println("[leftGrip | D/O(5)]: COMPLETED");
  rightGrip.attach(6); 
  delay(300);
  Serial.println("[rightGrip | D/O(6)]: COMPLETED");
  delay(200);
  Serial.println("DONE");

  Serial.print("SERVO INITIALIZING");
  zAxis.write(25); // base resting
  Serial.print(".");
  yAxis.write(90); // lift - flattened
  Serial.print(".");
  gripCall(1); // grip - closed
  Serial.println(".");
  delay(300);
  Serial.println("SERVO INITIALIZED");
}
// _____________________________________ GROB SETUP ____________________________________

// ___________________________________ BASE FUNCTIONS __________________________________
void baseCall(char cDir) // base turn Control
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
// ___________________________________ BASE FUNCTIONS __________________________________

// ___________________________________ GRIP FUNCTIONS __________________________________
void gripManager(int nDestinAngle, bool bDir) // CurrentAngle is standardized on the right servo
{
  switch(bDir) // add or subs
  {
    case 1: // add
      for(int angR = rightGrip.read(); angR >= 25; angR--)
      {
        leftGrip.write(angR);
        rightGrip.write(map(angR,0,180,180,0));
        nCurrentGripAngle = angR;
        delay(20);
      }
      Serial.println("Servo[CLOSE]");
      break;
 
    case 0: // subs
      for(int angR = nCurrentGripAngle; angR < 100; angR++)
      {
        leftGrip.write(angR);
        rightGrip.write(map(angR,0,180,180,0));
        nCurrentGripAngle = angR;
        delay(15);
      }
      Serial.println("Servo[OPEN]");
      break;
  }
}

void gripCall(int nStatus)
{
  switch(nStatus)
  {
    case 0: // open
      gripManager(100, 1);
      break;

    case 1: // close
      gripManager(30, 0);
      break;
  }
}
// ___________________________________ GRIP FUNCTIONS __________________________________

// ___________________________________ LIFT FUNCTIONS __________________________________
void liftManager(int nDestinAngle, int nLevel)
{
  switch(nLevel) // add or subs
  {
    case 0: // add
      for(int nTheta = yAxis.read() ; nTheta < nDestinAngle ; nTheta++)
      {
        yAxis.write(nTheta);
        delay(15);
      }
      Serial.println("Lift[LOWEST]");
      break;

    case 1: // middle
      if(yAxis.read() > 90)
      {
        for(int nTheta = yAxis.read() ; nTheta > nDestinAngle ; nTheta--)
        {
          yAxis.write(nTheta);
          delay(15);
        }
      }
      else if(yAxis.read() < 90)
      {
        for(int nTheta = yAxis.read() ; nTheta < nDestinAngle ; nTheta++)
        {
          yAxis.write(nTheta);
          delay(15);
        }
      }
      Serial.println("Lift[MIDDLE]");
      break;
 
    case 2: // subs
      for(int nTheta = yAxis.read() ; nTheta > nDestinAngle ; nTheta--)
      {
        yAxis.write(nTheta);
        delay(15);
      }
      Serial.println("Lift[HIGHEST]");
      break;
  }
}

void liftCall(int nLevel) // lifter control
{
  switch(nLevel)
  {
    case 0: // Lowest
      liftManager(120, 0);
      break;

    case 1: // Flat(mid)
      liftManager(90, 1);
      break;

    case 2: // Highest
      liftManager(40, 2);
      break;
  }
}
// ___________________________________ LIFT FUNCTIONS __________________________________

// ___________________________________ MCR. FUNCTIONS __________________________________
void macroMoveCall(int nMacro)
{
  switch(nMacro)
  {
    case 0: // initial position macro [RESTING]
      baseCall('n');
      liftCall(1);
      gripCall(1);
      break;

    case 1: // basic grabbing macro [HALF CYCLE]
      liftCall(0);
      gripCall(1);
      liftCall(2);
      baseCall('n');
      liftCall(1);
      gripCall(0);
      break;

    case 2: // basic grabbing macro [FULL CYCLE]
      liftCall(2);
      baseCall('u');
      gripCall(0);
      liftCall(0);
      gripCall(1);
      liftCall(2);
      baseCall('n');
      liftCall(1);
      gripCall(0);
      break;
  }
}
// ___________________________________ MCR. FUNCTIONS __________________________________


// _____________________________________ FUNCTIONS _____________________________________

