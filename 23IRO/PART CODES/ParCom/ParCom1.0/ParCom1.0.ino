// *******************************************************************************
// ********************************** INITIALIZ **********************************
// *******************************************************************************

// Libs
#include <List.hpp>
List<int> nNodeTypes;

// Sound Sensors
#define SX1 12
#define SX2 11

// DataInitiator
#define SIG 13

// Set Offset for higher accuracy in sampling (nSec * 1000)
int nOffSet = (0.375*1000); // 0.375

// 7 by 7 grid
int nGridRow = 7;
int nGridColumn = 7;

// preventing reading SIG more than once
bool SIG_OT = false;

// baudRate for receiving the datas (nSec * 1000)
int nBaudRate = (0.29*1000); // 0.29

// repeating var for printing nodes
int nRepreat = 0;

// Data Initiate Approval
bool bReadData = false;

// printNode? true/false
bool bApprovePrintNode = false;

// grid for simulation
int nGrid[7][7] = { { 0, 0, 0, 0, 0, 0, 0 },
                    { 0, 0, 0, 0, 0, 0, 0 },
                    { 0, 0, 0, 0, 0, 0, 0 },
                    { 0, 0, 0, 0, 0, 0, 0 },
                    { 0, 0, 0, 0, 0, 0, 0 },
                    { 0, 0, 0, 0, 0, 0, 0 },
                    { 0, 0, 0, 0, 0, 0, 0 } };

void setup() 
{
  pinMode(SX1, INPUT);
  pinMode(SX2, INPUT);
  pinMode(SIG, INPUT);

  // initiate list
  nNodeTypes.clear();

  // serial bebugging
  Serial.begin(9600);
  Serial.println("setup - done");
  Serial.println("");
  Serial.print("Grid: ");
  Serial.print(nGridColumn);
  Serial.print(" * ");
  Serial.println(nGridRow);
  //nGrid[1][1] = 1;

  // print grid
  prntGrid();
}

// *******************************************************************************
// ********************************** INITIALIZ **********************************
// *******************************************************************************



// *******************************************************************************
// ********************************** FUNCTIONS **********************************
// *******************************************************************************

void prntGrid()
{
  Serial.println("");
  
  // print grid
  for (int nRows = 0; nRows < (nGridRow); nRows++) 
  {
    for (int nColumn = 0; nColumn < (nGridColumn); nColumn++) 
    {
      Serial.print(nGrid[nRows][nColumn]);  // print node values
      Serial.print(" ");  // indentation between each nodes
    }
    Serial.println("");
  }
  Serial.println("");
}

void parallelCom(bool bSerialPrintNode = false)  // | false: do not print node types | true: print node types (in 7 * 7 grid) |
{
  if(digitalRead(SIG) == true)
  {
    if(SIG_OT == false)
    {
      // read data
      Serial.println("SIG | ON");
      Serial.println("");
      delay(nOffSet); // give 0.25sec offset
      bReadData = true;
      SIG_OT = true;
    }
  }
  if(bReadData == true)
  {
    for(int x = 0; x < (nGridRow); x++) // repeat 
    {
      for(int y = 0; y < (nGridColumn); y++)
      {
        if(digitalRead(SX1) == true)
        {
          if(digitalRead(SX2) == true)
          {
            // OBSTCL | append 3 | SENSOR: |1|1|
            nGrid[x][y] = 3;
            Serial.print("3");
          }
          else
          {
            // START | append 2 | SENSOR: |1|0|
            nGrid[x][y] = 2;
            Serial.print("2");
          }
        }
        else
        {
          if(digitalRead(SX2) == true)
          {
            // DESTIN | append 1 | SENSOR: |0|1|
            nGrid[x][y] = 1;
            Serial.print("1");
          }
          else 
          {
            // WHITE | append 0 | SENSOR: |0|0|
            nGrid[x][y] = 0;
            Serial.print("0");
          }
        }
        Serial.println("");
        delay(nBaudRate); // give 0.1sec delay
      }
    }

    Serial.println("DATA TRANSFERRED.");
    bReadData = false;
    bApprovePrintNode = true;
    SIG_OT = false;

  }
  if(bSerialPrintNode == true)
  {
    if(bApprovePrintNode == true)
    {
      prntGrid();
      bApprovePrintNode = false;
    }
  }
  else
  {
    Serial.println("");
  }
}

// *******************************************************************************
// ********************************** FUNCTIONS **********************************
// *******************************************************************************



// *******************************************************************************
// ********************************** EXECUTION **********************************
// *******************************************************************************

void loop() 
{
  parallelCom(1);
}

// *******************************************************************************
// ********************************** EXECUTION **********************************
// *******************************************************************************

