// PARCOM 3.0  |  BaudRate = 0.25//

// Sound Sensors
#define SX1 12
#define SX2 11

// DataInitiator
#define SIG 13

// Set Offset for higher accuracy in sampling (nSec * 1000)
#define nOffSet 350  // (0.125*1000)
// baudRate for receiving the datas (nSec * 1000)
#define nBaudRate 290 //  (0.25*1000)

int nInitialOffset = nOffSet + nBaudRate;

// 7 by 7 grid
int nGridRow = 7;
int nGridColumn = 7;

// preventing reading SIG more than once
bool bSIG_OT = false;

// repeating var for printing nodes
int nRepreat = 0;

// Data Initiate Approval
bool bReadData = false;

// printNode? true/false
bool bApprovePrintNode = false;

// nGrid reading for FOR loop
// int x = 0;
// int y = 0;

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

void prntGrid() // print grid from the array
{
  Serial.println(""); // indentation between Serial prints
  
  // print grid
  for (int nRows = 0; nRows < (nGridRow); nRows++) 
  {
    for (int nColumn = 0; nColumn < (nGridColumn); nColumn++) 
    {
      Serial.print(nGrid[nRows][nColumn]);  // print node values
      Serial.print(" ");  // indentation between each nodes
    }
    Serial.println(""); // indent
  }
  Serial.println(""); // indent
}

// append node type to the nGrid array
void appendNodeType(int nX, int nY, int nNodeType)
{
  nGrid[nY][nX] = nNodeType;
  Serial.print(nGrid[nY][nX]);
}

void readNode(int NX, int NY)
{
  if(digitalRead(SX1) == true)
  {
    if(digitalRead(SX2) == true)
    {
      // OBSTCL | append 3 | SENSOR: |1|1|
      appendNodeType(NX, NY, 3);
    }
    else
    {
      // START | append 2 | SENSOR: |1|0|
      appendNodeType(NX, NY, 2);
    }
  }
  else
  {
    if(digitalRead(SX2) == true)
    {
      // DESTIN | append 1 | SENSOR: |0|1|
      appendNodeType(NX, NY, 1);
    }
    else 
    {
      // WHITE | append 0 | SENSOR: |0|0|
      appendNodeType(NX, NY, 0);
    }
  }
  // Serial.println(""); // indent between nodes
  delay(nBaudRate); // give 0.29sec delay
}

void finalisation() // printing data transferred
{
  Serial.println(""); // indent between nodes
  bApprovePrintNode = true; // approve node print
  Serial.println("DATA TRANSFERRED.");
  bReadData = false; // exit from reading nodes
}

void parallelCom(bool bSerialPrintNode)  // | false: do not print node types | true: print node types (in 7 * 7 grid) |
{
  if(digitalRead(SIG) == true) // signal buzzer is ON
  {
    if(bSIG_OT == false) // OneTime-loop
    {
      // read data
      Serial.println("SIG | ON"); // bebug
      Serial.println("");

      //y++; // increase y axis by 1 each time when it has received SIG==true
      bReadData = true; // allow to read nodes
      bSIG_OT = true; // exit
    }
  }

  if(bReadData == true) // read nodes
  {
    for(int y = 0; y < (nGridRow); y++) // y axis increment 
    {
      delay(nOffSet); // give 0.25sec offset
      for(int x = 0; x < (nGridColumn); x++) // x axis increment
      {
        readNode(x, y); // read node
      }
      Serial.println("");
      while(digitalRead(SIG) != 1) {} // loop in this WHILE until the SIG==true
    }
    finalisation(); // exit and print "DATA TRANSFERRED"
  }

  if(bSerialPrintNode == true) // Does the parameter given is true? then:
  {
    if(bApprovePrintNode == true) // has the readNode() gave you a permission? then:
    {
      prntGrid(); // print node 
      bApprovePrintNode = false; // exit
    }
  }
}

void loop()
{
  parallelCom(1);
  
}


