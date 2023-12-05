// ____________________________________ PRE-SETUP ______________________________________
#include <Servo.h> 
#include "HUSKYLENS.h"
#include <

Servo zAxis; // baseTurning
Servo yAxis; // gripperLifting

Servo rightGrip; // right servo of the gripper
Servo leftGrip; // left servo of the gripper        

//HUSKYLENS green line >> SDA(A4); blue line >> SCL(A5)
void printResult(HUSKYLENSResult result);
HUSKYLENS huskylens;

// AprilTag Infos
char cTagStatus = 'n';
char cTagPos;

// Servo turning values
int const baseTurnResting = 30; // default value
int const baseTurnFacingFront = 160; 

int const gripSetClose = 80; // default value
int const gripSetOpen = 50;

int const gripLiftLowest = 140; // default value
int const gripLiftFlat = 80;
int const gripLiftHighest = 30;

// Tag X coord. Range
int tagXCentre = 165;
int tagXRangeMin = tagXCentre - 15;
int tagXRangeMax = tagXCentre + 15;

// box X coord
int nBox_xCoord;

// stores the gripper's current angle for FOR LOOP parameters
int nCurrentGripAngle;

// motor Direction setup
int nMotorDir = 0;

// LiCaM Checking alignment
int nCheck = 0;

// LiCaM motor speed
int nLiCaMspeed = (abs(nBox_xCoord - 160) * 1.5);

// is thte box aligned with the gripper?
bool bBoxAligned = false;

// repeat serialCom until missionAccomplished is true
bool bMissionAccomplished = false;

// object collected?
bool bObjCollected = false;

// SeCom Sending values
char seComForw[5] = "Forw";
char seComBack[5] = "Back";
char seComDone[5] = "Done";
char seComAlgn[5] = "Algn";

// SeCom reading
char seComGrab[5];

// linear motion motors
#define BASE_MS 6
#define BASE_MD 7

// limit switches
#define LMT_R 13
#define LMT_L 12

// stores the liCaM's last movement
char lastMovement;

// ____________________________________ PRE-SETUP ______________________________________ 


// ______________________________________ SETUP ________________________________________
void setup() 
{
  Serial.begin(115200);

  // Servo setups
  zAxis.attach(2);
  yAxis.attach(3);
  leftGrip.attach(5);
  rightGrip.attach(4); 

  // LiCoM motor setups
  pinMode(BASE_MD, OUTPUT);

  // Limit switches setups
  pinMode(LMT_R, INPUT);
  pinMode(LMT_L, INPUT);

  // Servo Initializing
  Serial.print("SERVO INITIALIZING");
  zAxis.write(baseTurnResting); // base resting
  Serial.print(".");
  yAxis.write(gripLiftHighest); // lift - flattened
  Serial.print(".");
  gripCall(0); // grip - opened
  baseCall('n');
  liftCall(1);
  Serial.println(".");
  Serial.println("SERVO INITIALIZED");

  // set temp. value to lastMovement 
  lastMovement = 'f';

  HLSetup(); // start HuskyLens

  // WAIT for the SeCom to allow to launch HuskyLens
  /*if(String(cStartCam) == "Strt") // if SeCom is equal to "Strt", start HuskyLens
  {
    HLSetup(); // start HuskyLens
  }
  else // if SeCom is not equal to "Strt", do not start HuskyLens, instead keep reading [cStartCam]
  {
    Serial.readBytes(cStartCam, 4); // Read HuskyLens
    Serial.println("NOT READY TO LAUNCH HUSKYLENS.");
  }
  */
}
// ______________________________________ SETUP ________________________________________



// _____________________________________ FUNCTIONS _____________________________________

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
void gripCall(int nDir) // CurrentAngle is standardized on the right servo
{
  switch (nDir) 
  {
    case 0:
      Serial.println("Servo[Open]");
      for(int angR = leftGrip.read(); angR >= gripSetOpen; angR--)
      {
        leftGrip.write(angR);
        rightGrip.write(map(angR,0,180,180,0));
        nCurrentGripAngle = angR;
        delay(15);
      }
      break;

    case 1:
      Serial.println("Servo[Close]");
      for(int angR = leftGrip.read(); angR < gripSetClose; angR++)
      {
        leftGrip.write(angR);
        rightGrip.write(map(angR,0,180,180,0));
        nCurrentGripAngle = angR;
        delay(15);
      }
      break;
  }
}
// ___________________________________ GRIP FUNCTIONS __________________________________

// ___________________________________ LIFT FUNCTIONS __________________________________

void liftCall(int nLevel)
{
  switch(nLevel) // add or subs
  {
    case 0: // add
      for(int nTheta = yAxis.read() ; nTheta < gripLiftLowest ; nTheta++)
      {
        yAxis.write(nTheta);
        delay(15);
      }
      Serial.println("Lift[LOWEST]");
      break;

    case 1: // middle
      if(yAxis.read() > 90)
      {
        for(int nTheta = yAxis.read() ; nTheta > gripLiftFlat ; nTheta--)
        {
          yAxis.write(nTheta);
          delay(15);
        }
      }
      else if(yAxis.read() < 90)
      {
        for(int nTheta = yAxis.read() ; nTheta < gripLiftFlat ; nTheta++)
        {
          yAxis.write(nTheta);
          delay(15);
        }
      }
      Serial.println("Lift[MIDDLE]");
      break;
 
    case 2: // subs
      for(int nTheta = yAxis.read() ; nTheta > gripLiftHighest ; nTheta--)
      {
        yAxis.write(nTheta);
        delay(15);
      }
      Serial.println("Lift[HIGHEST]");
      break;
  }
}

// ___________________________________ LIFT FUNCTIONS __________________________________

// ___________________________________ MCR. FUNCTIONS __________________________________

// 0: unload[FULL]   1: load[FULL]   2: unload[HALF]   3: load[HALF]
void macroMoveCall(int nMacro)
{
  switch(nMacro)
  {
    case 0: // unload [FULL CYCLE]
      gripCall(1);
      liftCall(2);
      baseCall('u');
      liftCall(0);
      gripCall(0);
      liftCall(2);
      baseCall('n');
      liftCall(1);
      gripCall(1);
      Serial.println(3); // unloaded [seCOM]
      break;

    case 1: // load [FULL CYCLE]
      liftCall(2);
      baseCall('u');
      gripCall(0);
      liftCall(0);
      gripCall(1);
      liftCall(2);
      baseCall('n');
      liftCall(1);
      gripCall(0);
      Serial.println(2); // loaded [seCOM]
      break;

    case 2: // unload [HALF CYCLE]
      gripCall(1);
      liftCall(2);
      baseCall('u');
      liftCall(0);
      gripCall(0);
      Serial.println(3); // unloaded [seCOM]
      break;

    case 3: // load [HALF CYCLE]
      gripCall(1);
      liftCall(2);
      baseCall('n');
      liftCall(1);
      gripCall(0);
      Serial.println(2); // loaded [seCOM]
      break;
  }
}

// moving average servo control
void mvAvgMove(int nLiftGoal, int nGripGoal)
{
  for(int i = yAxis.read(); i < nLiftGoal; i++)
  {
    
  }
}

void loadObj(int nSlotNum)
{
  liftCall(2);
  baseCall('u');
  gripCall(0);
  liftCall(0);
  HorizMove('n');
  gripCall(1);
  liftCall(2);
  baseCall('n');
  switch (nSlotNum) 
  {
    case 1:
      HorizMove('l');
      break;

    case 2:
      HorizMove('r');
      break;
  }
  liftCall(1);
  gripCall(0);
}

void unloadObj(int nSlotNum)
{
  liftCall(2);
  switch (nSlotNum) 
  {
    case 1:
      HorizMove('l');
      break;

    case 2:
      HorizMove('r');
      break;
  }
  gripCall(0);
  liftCall(1);
  gripCall(1);
  liftCall(2);
  baseCall('u');
  liftCall(0);
  HorizMove('n');
  gripCall(0);
  liftCall(2);
  baseCall('n');
  switch (nSlotNum) 
  {
    case 1:
      HorizMove('l');
      break;

    case 2:
      HorizMove('r');
      break;
  }
  liftCall(1);
  gripCall(1);
}

// aligns the arm to the box, and loads onto its base
void grabThebox()
{
  while(bObjCollected == false)
  {
    liftCall(2);
    baseCall('u');
    liftCall(0);
    gripCall(0);
    HorizMove('a'); // box aligned!
    while(bMissionAccomplished == false){ serialCom('l'); Serial.println("serialCom"); }
    bObjCollected = true;
  }
}

// no matter what comes, put the box down
void dropTheBox()
{
  gripCall(1);
  liftCall(2);
  baseCall('u');
  liftCall(0);
  gripCall(0);
  Serial.write(seComDone); // send signal to the base that it has dropped the Obj.
}
// ___________________________________ MCR. FUNCTIONS __________________________________

// ___________________________________ LNRM FUNCTIONS __________________________________
void HorizMove(char cDir)
{
  char cLastMove;
  int nSpeed = 255;
  int nOffsetDelay = 600;

  switch(cDir)
  {
    case 'r':
      // right
      nMotorDir = 0;
      digitalWrite(BASE_MD, nMotorDir);
      while(digitalRead(LMT_R) == true)
      {
        analogWrite(BASE_MS, nSpeed);
      }
      cLastMove = 'r';
      break;

    case 'l':
      // left
      nMotorDir = 1;
      digitalWrite(BASE_MD, nMotorDir);
      while(digitalRead(LMT_L) == true)
      {
        analogWrite(BASE_MS, nSpeed);
      }
      cLastMove = 'l';
      break;

    case 'n':
      // neutral
      if(cLastMove == 'r') // if current pos = right, move left
      {
        nMotorDir = 1; // change dir to left
        digitalWrite(BASE_MD, nMotorDir);
        analogWrite(BASE_MS, nSpeed);
        delay(nOffsetDelay);
        // analogWrite(BASE_MS, 0);
      }
      else if(cLastMove == 'l') // if current pos = left, move right
      {
        nMotorDir = 0; // change dir to right
        digitalWrite(BASE_MD, nMotorDir);
        analogWrite(BASE_MS, nSpeed);
        delay(nOffsetDelay);
        // analogWrite(BASE_MS, 0);
      }
      break;
    
    case 'a':
      // automatic
      while(bBoxAligned == false) centreObj();
      break;
  }
}
// ___________________________________ LNRM FUNCTIONS __________________________________

// ___________________________________ QUADE FUNCTIONS __________________________________
void quade()
{
  Serial.readBytes(seComGrab, 4);
  if(String(seComGrab) == "Grab")
  {
    Serial.println("Received Grab");

    grabThebox();
  }
  else if(String(seComGrab) == "Drop")
  {
    Serial.println("Received Drop");
    dropTheBox();
  }
}
// ___________________________________ QUADE FUNCTIONS __________________________________

// _____________________________________ FUNCTIONS _____________________________________


void loop() 
{ 
  quade();
}
