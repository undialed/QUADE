#include <Servo.h>
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

#define DIAL1 A0
#define DIAL2 A1
#define DIAL3 A2
#define DIAL4 A3

void setup() {
  servo1.attach(2);
  servo2.attach(3);
  servo3.attach(4);
  servo4.attach(5);

  pinMode(DIAL1, INPUT);
  pinMode(DIAL2, INPUT);
  pinMode(DIAL3, INPUT);
  pinMode(DIAL4, INPUT);
}

void loop() {
  servo1.write(map(analogRead(DIAL1), 0, 1023, 0, 180));
  servo2.write(map(analogRead(DIAL2), 0, 1023, 0, 180));
  servo3.write(map(analogRead(DIAL3), 0, 1023, 0, 180));
  servo4.write(map(analogRead(DIAL4), 0, 1023, 0, 180));
  delay(10);
}
