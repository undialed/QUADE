void setSpeed (){

  inc = map(analogRead(A0), 0, 1023 , -150 , 105);
  speed = 150 + inc;
  Serial.println(inc);
}
//debugging
void testSensors(){
  Serial.print("S1: ");
  Serial.println(digitalRead(S1));
  Serial.print("S2: ");
  Serial.println(digitalRead(S2));
  Serial.print("S3: ");
  Serial.println(digitalRead(S3));
  Serial.print("S4: ");
  Serial.println(digitalRead(S4));
}