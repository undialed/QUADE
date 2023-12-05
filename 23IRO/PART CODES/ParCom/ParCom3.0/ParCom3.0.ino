// FINAL PARCOM 3.5 PRT
// |  28/10/2023 SAT  |
// | Baudrate 0.1 wrk |  

//_______________________________________________________INITILIZE_______________________________________________________

// DataInitiator
#define SIG 12

// Sound Sensors
#define SX1 13
#define SX2 3

// Set Offset for higher accuracy in sampling (nSec * 1000)
#define nOffSet 220  // 0.15 : 250 | 0.1 : 220
// baudRate for receiving the datas (nSec * 1000)
#define nBaudRate 165 //  0.15 : 190 | 0.1 : 165

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

char cGrid[7][7] = { { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' } };

void setup() 
{
  Serial.begin(9600);

  pinMode(SX1, INPUT);
  pinMode(SX2, INPUT);
  pinMode(SIG, INPUT);

  Serial.print("OffSet: ");
  Serial.println(nOffSet);
  Serial.print("BaudRate: ");
  Serial.println(nBaudRate);

  // print grid
  // prntGrid('c');
}

//_______________________________________________________INITILIZE_______________________________________________________



//_______________________________________________________FUNCTIONS_______________________________________________________
void prntGrid(char cGridType) // print grid from the array
{
  Serial.println(""); // indentation between Serial prints
  
  // print grid
  for (int nRows = 0; nRows < (nGridRow); nRows++) 
  {
    for (int nColumn = 0; nColumn < (nGridColumn); nColumn++) 
    {
      switch(cGridType)
      {
        case 'n':
          Serial.print(nGrid[nRows][nColumn]);  // print node values
          break;

        case 'c':
          Serial.print(cGrid[nRows][nColumn]);  // print node values
          break;
      }
      Serial.print(" ");  // indentation between each nodes
    }
    Serial.println(""); // indent
  }
  Serial.println(""); // indent
}

// append node type to the nGrid array
void appendNodeType(int nX, int nY, int nNodeType, char cNodeType) // NX: x axis increment | NY: y axis increment | nNodeType: NodeType(0: blank; 1: start; 2: destin; 3: obstcl)
{
  nGrid[nY][nX] = nNodeType; // appending nodeType into the array
  cGrid[nY][nX] = cNodeType; // appending nodeType into the array
  Serial.print(nGrid[nY][nX]); // serial debug : print its nodetype stored in the array
}

// read node from the SOUND SENSOR INPUTS and append its value into the array
void readNode(int NX, int NY) // NX: x axis increment | NY: y axis increment
{
  if(digitalRead(SX1) == false)
  {
    if(digitalRead(SX2) == false)
    {
      // OBSTCL | append 3 | SENSOR: |1|1|
      appendNodeType(NX, NY, 2, 'x');
    }
    else
    {
      // DESTIN | append 2 | SENSOR: |1|0|
      appendNodeType(NX, NY, 3, 'd');
    }
  }
  else
  {
    if(digitalRead(SX2) == false)
    {
      // START | append 1 | SENSOR: |0|1|
      appendNodeType(NX, NY, 1, 's');
    }
    else 
    {
      // WHITE | append 0 | SENSOR: |0|0|
      appendNodeType(NX, NY, 0, 'o');
    }
  }
  // Serial.println(""); // indent between nodes
}

// void finalisation() // printing data transferred
// {
//   bApprovePrintNode = true; // approve node print
//   Serial.println("o = blank nodes");
//   Serial.println("x = blocked nodes");
//   Serial.println("s = start node");
//   Serial.println("d = destination node");
//   bReadData = false; // exit from reading nodes
// }

void parallelCom(bool bSerialPrintNode)  // | false: do not print node types | true: print node types (in 7 * 7 grid) |
{
  if(digitalRead(SIG) == false) // signal buzzer is ON
  {
    if(bSIG_OT == false) // OneTime-loop
    {
      // read data
      // Serial.println("SIG | ON"); // bebug
      // Serial.println("");

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
        delay(nBaudRate); // give delay between the nodes
      }
      Serial.println("");
      while(digitalRead(SIG) != 0){} // loop in this WHILE until the SIG==true
    }
    bApprovePrintNode = true; // approve node print
    bReadData = false; // exit from reading nodes
    // finalisation(); // exit and print "DATA TRANSFERRED"
  }

  if(bSerialPrintNode == true) // Does the parameter given is true? then:
  {
    if(bApprovePrintNode == true) // has the readNode() gave you a permission? then:
    {
      // prntGrid('n'); // print node int
      prntGrid('c'); // print node char
      bApprovePrintNode = false; // exit
      delay(1000); // after 1sec, it gets ready to read nodes again
      bSIG_OT = false; // able to read nodes again
    }
  }
}
//_______________________________________________________FUNCTIONS_______________________________________________________


void loop()
{
  parallelCom(1);
}
