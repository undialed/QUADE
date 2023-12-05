// _________________________________________FUNCTIONS_________________________________________
void HLSetup()
{
  Wire.begin();
  while (!huskylens.begin(Wire))
  {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
}

// l: load | u: unload
void serialCom(char cMode)
{
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
  else
  {
    while (huskylens.available()) // when Serial available run:
    {
      HUSKYLENSResult result = huskylens.read(); // undate husky datas
      printResult(result, 't'); // read and print HuskyLens datas

      if(cTagStatus == 'r') // if tag is in range:
      { 
        if(cMode == 'l') // load
        {
          macroMoveCall(3); // full Load
          delay(2000);
          Serial.write(seComDone, 4); // grabbed the object
          bMissionAccomplished = true;
        }
        else if(cMode == 'u') // unload
        {
          macroMoveCall(0); // full Unload
          delay(2000);
          Serial.write(seComDone, 4); // unloaded the object
          bMissionAccomplished = true;
        }
      } 
      else if(cTagStatus == 'f') // tag too far
      {
        if(cMode == 'l') // load
        {
          Serial.write(seComForw, 4); // go forward
          Serial.print("Forw");
          bMissionAccomplished = false;
        }
      }
      else if(cTagStatus == 'c') // tag too close
      {
        if(cMode == 'l') // load
        {
          Serial.write(seComBack, 4); // go backward
          Serial.print("Back");
          bMissionAccomplished = false;
        }
      }
    }    
  }
}

void centreObj()
{
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) {Serial.println(F("No block or arrow appears on the screen!")); analogWrite(BASE_MS, 0);}
  else
  {
    while (huskylens.available()) // when Serial available run:
    {
      HUSKYLENSResult result = huskylens.read(); // undate husky datas
      printResult(result, 'a'); // read and print HuskyLens datas

      if(cTagPos == 'm')
      {
        analogWrite(BASE_MS, 0);
        Serial.println("MOTOR: STOP");
        if(nCheck < 6)
        {
          nCheck++;
          delay(200);
        }
        else
        {
          Serial.println("MOTOR: ALIGNED!");
          bBoxAligned = true;
        }
        // Serial.write(seComAlgn);
        // delay(1000);
      }
      else if(cTagPos == 'r') // go left (opposite to the cTagpos)
      {
        digitalWrite(BASE_MD, 0);
        analogWrite(BASE_MS, nLiCaMspeed);
        Serial.println("MOTOR: LEFT");
        bBoxAligned = false;
        nCheck = 0;
      }
      else if(cTagPos == 'l') // go right (opposite to the cTagpos)
      {
        digitalWrite(BASE_MD, 1);
        analogWrite(BASE_MS, nLiCaMspeed);
        Serial.println("MOTOR: RIGHT");
        bBoxAligned = false;
        nCheck = 0;
      }
    }    
  }
}

void distGrabObj()
{
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
  else
  {
    while (huskylens.available()) // when Serial available run:
    {
      HUSKYLENSResult result = huskylens.read(); // undate husky datas
      printResult(result, 't'); // read and print HuskyLens datas

      if(cTagStatus == 'r') // if tag is in range:
      { 
        gripCall(1);
      } 
      else if(cTagStatus == 'f') // tag too far
      {
        Serial.println("Too far");
      }
      else if(cTagStatus == 'c') // tag too close
      {
        Serial.println("Too close");
      }
    }
  }
}

void printHuskyData(HUSKYLENSResult result)
{
  Serial.println(String()+F("Height: ")+result.height);
  Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
}

void printResult(HUSKYLENSResult result, char cMode)
{
  int nDir;

  if (result.command == COMMAND_RETURN_BLOCK)
  {
    switch(cMode)
    {
      case 't': // tagRecog
        if((result.height > 130) && (result.height < 140))// tag inrange
        {
          cTagStatus = 'r';
          // printHuskyData(result);       
        }
        else if(result.height < 120) // tag too far
        {
          cTagStatus = 'f';
          // printHuskyData(result);   
        }
        else if(result.height > 140) // tag too close
        {
          cTagStatus = 'c';
          // printHuskyData(result);       
        }
        break;

      case 'a': // Auto-centreObj
        if((result.yCenter > 20) && (result.yCenter < 220)) // yAxis is fixed
        {
          nBox_xCoord = result.xCenter; 
          if((result.xCenter > tagXRangeMin) && (result.xCenter < tagXRangeMax)) // tagXRangeMin = 155 | tagXRangeMax = 185
          {
            // tag middle, stop motor
            cTagPos = 'm';
            Serial.println("TAG: MIDDLE");
            // printHuskyData(result);       
          }
          else if(result.xCenter < tagXRangeMin) 
          {
            // too left
            cTagPos = 'r';
            Serial.println("TAG: RIGHT");
            // printHuskyData(result);       
          }
          else if(result.xCenter > tagXRangeMax)
          {
            // too right
            cTagPos = 'l';
            Serial.println("TAG: LEFT");
            // printHuskyData(result);       
          }
        }
        break;
    }
  }
}
// _________________________________________FUNCTIONS_________________________________________

