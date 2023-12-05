void directionMaker(){
  Serial.println("Maker: ");
  int b;
  bool first = false;
  while(first == false){
    //to stop before or on the patch
    if(goal_type[0] == 'P'){b = path.getSize()-1;first = true;}
    else if(goal_type[0] == 'D'){b = path.getSize()-1;first = true;}
    else if(goal_type[0] == 'N'){b = path.getSize();first = true;}
  }

  Serial.println(b);

  Gyro('U');
  realign();

  for(int i = 0 ; i < b ;i++){
    if(path[i] == 'U'){
      inPatch = true;
      move('F');
      current_location -= 1;
    }
    if(path[i] == 'D'){
      inPatch = true;
      move('B');
      current_location += 1;
    }
    if(path[i] == 'L'){
      inPatch = true;
      move('L');
      current_location -= 10;
    }
    if(path[i] == 'R'){
      inPatch = true;
      move('R');
      current_location += 10;
    }
  }
  motors('S' , 0);
  realign();
  motors('S' , 0);
}

void move(char type){
  Serial.println("Move: ");
  process = false;
  while(process == false){
    switch(type){
      case 'F':
        patch('F');
      break;
      case 'B':
        patch('B');
      break;
      case 'L':
        patch('L');
      break;
      case 'R':
        patch('R');
      break;
      case 'S':
        motors('S' , 0);
      break;
    }
  }
}

void motors(char type , int speed1){
  setSpeed();
  Serial.println("Motors: ");
  speed = 150 +inc;
  int sideways = speed*0.5;
  switch(type){
    case 'F':
      digitalWrite(MD1 , 1);
      analogWrite(MS1 , speed1);
      digitalWrite(MD2 , 1);
      analogWrite(MS2 , speed1);
      digitalWrite(MD3 , 1);
      analogWrite(MS3 , speed1);
      digitalWrite(MD4 , 1);
      analogWrite(MS4 , speed1);
    break;
    case 'B':
      digitalWrite(MD1 , 0);
      analogWrite(MS1 , speed1);
      digitalWrite(MD2 , 0);
      analogWrite(MS2 , speed1);
      digitalWrite(MD3 , 0);
      analogWrite(MS3 , speed1);
      digitalWrite(MD4 , 0);
      analogWrite(MS4 , speed1);
    break;
    case '1':
      digitalWrite(MD1 , 0);
      analogWrite(MS1 , speed1);
      digitalWrite(MD2 , 1);
      analogWrite(MS2 , speed1);
      digitalWrite(MD3 , 0);
      analogWrite(MS3 , speed1);
      digitalWrite(MD4 , 1);
      analogWrite(MS4 , speed1);
    break;
    case '2':
      digitalWrite(MD1 , 1);
      analogWrite(MS1 , speed1);
      digitalWrite(MD2 , 0);
      analogWrite(MS2 , speed1);
      digitalWrite(MD3 , 1);
      analogWrite(MS3 , speed1);
      digitalWrite(MD4 , 0);
      analogWrite(MS4 , speed1);
    break;
    case 'L':
      digitalWrite(MD1 , 0);
      analogWrite(MS1 , speed1+sideways);
      digitalWrite(MD2 , 1);
      analogWrite(MS2 , speed1+sideways);
      digitalWrite(MD3 , 1);
      analogWrite(MS3 , speed1+sideways);
      digitalWrite(MD4 , 0);
      analogWrite(MS4 , speed1+sideways);
    break;
    case 'R':
      digitalWrite(MD1 , 1);
      analogWrite(MS1 , speed1+sideways);
      digitalWrite(MD2 , 0);
      analogWrite(MS2 , speed1+sideways);
      digitalWrite(MD3 , 0);
      analogWrite(MS3 , speed1+sideways);
      digitalWrite(MD4 , 1);
      analogWrite(MS4 , speed1+sideways);
    break;
    case 'S':
      analogWrite(MS1 , 0);
      analogWrite(MS2 , 0);
      analogWrite(MS3 , 0);
      analogWrite(MS4 , 0);
    break;
  }
}

void patch(char type){
  Serial.println("Patch: ");
  bool outLoop = false;

  int Contact_1;
  int Contact_2;
  int Contact_3;
  int Contact_4;

  if(type == 'L'){ Contact_1 = 16;Contact_2 = 2;Contact_3 = 17;Contact_4 = 15;}
  if(type == 'F'){ Contact_1 = 2;Contact_2 = 15;Contact_3 = 16;Contact_4 = 17;}
  if(type == 'B'){ Contact_1 = 17;Contact_2 = 16;Contact_3 = 15;Contact_4 = 2;}
  if(type == 'R'){ Contact_1 = 15;Contact_2 = 17;Contact_3 = 2;Contact_4 = 16;}

  int previous_millis = millis();
  int starting_millis;

  while(outLoop == false){
    if(inPatch == true){
      if(digitalRead(Contact_1)+digitalRead(Contact_2)+digitalRead(Contact_3)+digitalRead(Contact_4) == 4){
        inPatch = false;
        Serial.print("OffPatch");
      }
      else{   
        speed = 150 + inc;
        motors(type , speed);
      }
    }
    else if (inPatch == false){
      if(digitalRead(Contact_1)+digitalRead(Contact_2)+digitalRead(Contact_3)+digitalRead(Contact_4) == 4){
        Gyro('U');
        motors(type , speed);
      }
      if(digitalRead(Contact_1)+digitalRead(Contact_2) <= 1){
        Gyro('U');
        motors(type , speed*(0.5));
      }
      if(digitalRead(Contact_1)+digitalRead(Contact_2)+digitalRead(Contact_3)+digitalRead(Contact_4) <= 1){
        motors('S' , 0);
        process = true; outLoop = true;
      }
      else if(((digitalRead(Contact_1) + digitalRead(Contact_3)) == 0) || ((digitalRead(Contact_2) + digitalRead(Contact_4)) == 0)){
        realign();
        process = true; outLoop = true;
      }
    }
  }
}

void realign(){
  Serial.println("Realign: ");
  bool outLoop = false;
  char previousMotion = 'N';
  if(digitalRead(S1) + digitalRead(S2) + digitalRead(S3) + digitalRead(S4) < 4){
    while(outLoop == false){
      speed = 150 + inc;
      if (digitalRead(S1) + digitalRead(S2) + digitalRead(S3) + digitalRead(S4) == 0){
        motors('S' , 0);
        outLoop = true;
      }
      else if(((digitalRead(S1) == false) || (digitalRead(S2) == false)) && ((digitalRead(S3) == true) && (digitalRead(S4) == true))){
        motors('F' , speed);
        previousMotion = 'F';
      }
      else if(((digitalRead(S3) == false) || (digitalRead(S4) == false)) && ((digitalRead(S1) == true) && (digitalRead(S2) == true))){
        motors('B' , speed);
        previousMotion = 'B';
      }
      else if(( (digitalRead(S1) == false) && (digitalRead(S3) == false) && ((digitalRead(S2) == true) || (digitalRead(S4) == true))) ){
        motors('L' , speed);
        previousMotion = 'L';
      }
      else if(( (digitalRead(S2) == false) && (digitalRead(S4) == false) && ((digitalRead(S1) == true) || (digitalRead(S3) == true))) ){
        motors('R' , speed);
        previousMotion = 'R';
      }
      else if(digitalRead(S1) + digitalRead(S2) + digitalRead(S3) + digitalRead(S4) == 4){
        switch(previousMotion){
          case 'N':
          motors('S', 0);
          break;
          case 'F':
          motors('F', speed);
          break;
          case 'B':
          motors('B', speed);
          break;
          case 'L':
          motors('L', speed);
          break;
          case 'R':
          motors('R', speed);
          break;
        }
      }
    }
  }
}