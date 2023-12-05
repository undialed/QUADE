void Gyro(char type) {
  Serial.println("Gyro: ");
  int speed;
  // if programming failed, don't try to do anything
  if (!dmpReady) return;
  // read a packet from FIFO
  bool loop = false;
  float threshold = 0.08;
  while(loop == false){
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
      // display angles in radians
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      if(type == 'U'){
        speed = (150+inc )* (abs(ypr[0] - (0)));
        if(speed > 150+inc) speed = 150+inc;
        if(speed < 60+inc) speed = 60+inc;
        if(ypr[0] < (0)-threshold){
          motors('1' , speed);
        }
        if(ypr[0] > (0)+threshold){
          motors('2' , speed);
        }
        if((ypr[0] > (0) - threshold) && (ypr[0] < (0) + threshold)){
          motors('S',0);
          Serial.println("Facing Upwards");
          loop = true;
          return;
        }
      }
      if(type == 'L'){
        speed = 150 * (abs(ypr[0] - (PI/2)));
        if(speed > 150+inc) speed = 150+inc;
        if(speed < 60+inc) speed = 60+inc;
        if(ypr[0] > (PI/2)+threshold){
          motors('2' , speed);
        }
        if(ypr[0] < (PI/2)-threshold){
          motors('1' , speed);
        }
        if((ypr[0] > (PI/2) - threshold) && (ypr[0] < (PI/2) + threshold)){
          motors('S',0);
          Serial.println("Facing Left");
          loop = true;
          return;
        }
      }
      if(type == 'R'){
        speed = 150 * (abs(ypr[0] + (PI/-2)));
        if(speed > 150+inc) speed = 150+inc;
        if(speed < 60+inc) speed = 60+inc;
        if(ypr[0] < (PI/-2) - threshold){
          motors('1' , speed);
        }
        if(ypr[0] > (PI/-2) + threshold){
          motors('2' , speed);
        }
        if((ypr[0] < (PI/-2) + threshold) && (ypr[0] > (PI/-2) - threshold)){
          motors('S',0);
          Serial.println("Facing Right");
          loop = true;
          return;
        }
      }
      if(type == 'D'){
        speed = 150 * (abs(ypr[0] - (PI)));
        if(speed > 150+inc) speed = 150+inc;
        if(speed < 60+inc) speed = 60+inc;
        if((ypr[0] > (-PI) + threshold) &&(ypr[0] < 0)){
          motors('2' , speed);
        }
        if((ypr[0] < (PI) - threshold) &&(ypr[0] > 0)){
          motors('1' , speed);
        }
        if((ypr[0] < (-PI) + threshold) && (ypr[0] > (PI) - threshold)){
          motors('S',0);
          Serial.println("Facing Downwards");
          loop = true;
          return;
        }
      }
    }
  }
}