void pickUp(){
  Serial.println("PickUp: ");
  realign();

  int b_loop = false;
  while(b_loop == false){
    if(String(mystr) == "Forw"){
      motors('F' , speed);
      delay(100);
      motors('S' , 0);
      delay(700);
      Serial.print("F");
    }
    else if(String(mystr) == "Back"){
      motors('B' , speed);
      delay(100);
      motors('S' , 0);
      delay(700);
      Serial.print("B");
    }
    else if(String(mystr) == "Done"){
      motors('B' , speed);
      realign();
      b_loop = true;
      Serial.print("D");
    }
    Serial.readBytes(mystr,4);
    //orientate();
    Serial.write("Grab",4);
    Serial.print("C");
  }
  
}

void dropOff(){ 
  Serial.println("DropOff: ");
  for(int check = 0 ; check < 6 ; check++){
    orientate();
    realign();
    delay(200);
  }

  int outLoop = false;
  while(outLoop == false){
    speed = 150 + inc;
    if (digitalRead(S1) + digitalRead(S2) + digitalRead(S3) + digitalRead(S4) == 0){
      motors('F' , speed);
    }
    else if (digitalRead(S1) + digitalRead(S2) + digitalRead(S3) + digitalRead(S4) == 4){
      motors('B' , speed);
    }
    else if(((digitalRead(S3) == false) && (digitalRead(S4) == false)) && ((digitalRead(S1) == true) && (digitalRead(S2) == true))){
      motors('S' , 0);
      Serial.write("Drop",4);
      delay(1000);
      outLoop = true;
    }
  }
}

void orientate(){
  Serial.println("orientate: ");
  int orientation = current_destination - current_location;
  Serial.println(orientation);
  int loop = false;
  while(loop == false){
    if(orientation == -10){
      Gyro('L');
      loop = true;
      return;
    }
    if(orientation == 10){
      Gyro('R');
      loop = true;
      return;
    }
    if(orientation == 1){
      Gyro('D');
      loop = true;
      return;
    }
    if(orientation == -1){
      Gyro('U');
      loop = true;
      return;
    }
  }
}