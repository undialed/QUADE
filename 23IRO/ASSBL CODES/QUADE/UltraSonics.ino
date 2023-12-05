//test code not apart of navigation

//use huskylens for landmarking
//ultrasonic only sends if the map is different from the original
void obstacleDetection(char str[5]){
  //#define trigEchoF 14
  //#define trigEchoL 15
  //#define trigEchoR 16
  //#define trigEchoB 17

  long duration;
  int distance;

  for(int i = 14 ; i < 18 ; i++){
    pinMode(i , OUTPUT);
    digitalWrite(i , LOW);
    delayMicroseconds(2);
    digitalWrite(i , HIGH);
    delayMicroseconds(10);
    digitalWrite(i , LOW);

    pinMode(i , INPUT);
    duration = pulseIn(i , HIGH);
    distance = duration * 0.034 / 2;

    if((distance > 5)&&(distance < 10)){
      str[i-14] = "1";
    }
    else{
      str[i-14] = "0";
    }
  }
}