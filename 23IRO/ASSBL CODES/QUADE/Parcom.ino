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
  cGrid[nY][nX] = cNodeType; // appending nodeType into the array
  Serial.print(cGrid[nY][nX]); // serial debug : print its nodetype stored in the array
}

// read node from the SOUND SENSOR INPUTS and append its value into the array
void readNode(int NX, int NY) // NX: x axis increment | NY: y axis increment
{
  if(digitalRead(SX1) == false)
  {
    if(digitalRead(SX2) == false)
    {
      // OBSTCL | append 3 | SENSOR: |1|1|
      appendNodeType(NX, NY, 3, 'd');
    }
    else
    {
      // DESTIN | append 1 | SENSOR: |1|0|
      appendNodeType(NX, NY, 1, 's');
    }
  }
  else
  {
    if(digitalRead(SX2) == false)
    {
      // START | append 2 | SENSOR: |0|1|
      appendNodeType(NX, NY, 2, 'x');
    }
    else 
    {
      // WHITE | append 0 | SENSOR: |0|0|
      appendNodeType(NX, NY, 0, 'o');
    }
  }
  // Serial.println(""); // indent between nodes
}

void parallelCom(bool bSerialPrintNode)  // | false: do not print node types | true: print node types (in 7 * 7 grid) |
{
  if(digitalRead(SIG) == false) // signal buzzer is ON
  {
    if(bSIG_OT == false) // OneTime-loop
    {
      bReadData = true; // allow to read nodes
      bSIG_OT = true; // exit
    }
  }
  else{return;}

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
  }

  if(bSerialPrintNode == true) // Does the parameter given is true? then:
  {
    if(bApprovePrintNode == true) // has the readNode() gave you a permission? then:
    {
      prntGrid('c'); // print node char
      bApprovePrintNode = false; // exit

      for(int i = 0 ; i < y ; i++){
        for(int j = 0 ; j < x ; j++){
          if(cGrid[i][j] == 'o'){Coordinates[i][j][0] = 0; nGrid[i][j] = 0;}
          else if(cGrid[i][j] == 's'){goals.addLast(j*10 + i);goal_type.addLast('P');}
          else if(cGrid[i][j] == 'd'){goals.addLast(j*10 + i);goal_type.addLast('D');}
          else if(cGrid[i][j] == 'x'){Coordinates[i][j][0] = 2; nGrid[i][j] = 2;}
        }
      } 
      
      delay(1000); // after 1sec, it gets ready to read nodes again
      bSIG_OT = false; // able to read nodes again
    }
  }
}