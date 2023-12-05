#include "HUSKYLENS.h"

HUSKYLENS huskylens;
//HUSKYLENS green line >> SDA(A4); blue line >> SCL(A5)
void printResult(HUSKYLENSResult result);
bool bTagInRange = false;

void setup() {
  Serial.begin(115200);
  HLSetup();
}

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

void HLReadDistance()
{
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
  else
  {
      Serial.println(F("###########"));
      while (huskylens.available())
      {
          HUSKYLENSResult result = huskylens.read();
          printResult(result); // read and print HuskyLens datas
      }    
  }
}

void printResult(HUSKYLENSResult result)
{
  if (result.command == COMMAND_RETURN_BLOCK)
  {
    if(result.height > 120)
    {
      bTagInRange = true;
      Serial.println("TAG: IN-RANGE");
    }
    else
    {
      bTagInRange = false;
      Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
    }
  }
  else
  {
      Serial.println("Object unknown!");
  }
}
// _________________________________________FUNCTIONS_________________________________________

void loop() 
{
  HLReadDistance();
}

