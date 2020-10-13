#include <SoftwareSerial.h>
#define ai_1 A5
#define stepper_pin_1 9
#define stepper_pin_2 10
#define stepper_pin_3 11
#define stepper_pin_4 12
int androidCommand;
bool isOpened = true;
int shine = 0;
SoftwareSerial bt(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(stepper_pin_1, OUTPUT);
  pinMode(stepper_pin_2, OUTPUT);
  pinMode(stepper_pin_3, OUTPUT);
  pinMode(stepper_pin_4, OUTPUT);
  bt.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bt.available())
    androidCommand = bt.parseInt();
  if(androidCommand == 10) {
    shine = analogRead(ai_1);
    if(shine < 120) {
      // open
      mOpen();
    } else {
      // close
      mClose();
    }
    /*actionWithJalousie(shine < 120);*/
    Serial.println("shine is  " + shine);
    delay(200);
  }
  if(androidCommand == 20 || androidCommand == 21 || androidCommand == 22) {
    if(androidCommand == 21) {
      // open
      mOpen();
      androidCommand = 20;
    }
    if(androidCommand == 22) {
      // close
      mClose();
      androidCommand = 20;
    }
  }
}

void mOpen() {
  if(!isOpened) {
    actionWithJalousie(true);
    isOpened = true;
    bt.write("opened");
  }
}

void mClose() {
  if(isOpened) {
    actionWithJalousie(false);
    isOpened = false;
    bt.write("closed");
  }
}

void actionWithJalousie(bool toOpen) {
  int cnt = 0;
  int step_number = 0;
  while(cnt < 2048) {
    if(toOpen) {
      //Serial.println("Open and cnt is  " + cnt);
      switch(step_number){
        case 0:
        digitalWrite(stepper_pin_1, HIGH);
        digitalWrite(stepper_pin_2, LOW);
        digitalWrite(stepper_pin_3, LOW);
        digitalWrite(stepper_pin_4, LOW);
        break;
        case 1:
        digitalWrite(stepper_pin_1, LOW);
        digitalWrite(stepper_pin_2, HIGH);
        digitalWrite(stepper_pin_3, LOW);
        digitalWrite(stepper_pin_4, LOW);
        break;
        case 2:
        digitalWrite(stepper_pin_1, LOW);
        digitalWrite(stepper_pin_2, LOW);
        digitalWrite(stepper_pin_3, HIGH);
        digitalWrite(stepper_pin_4, LOW);
        break;
        case 3:
        digitalWrite(stepper_pin_1, LOW);
        digitalWrite(stepper_pin_2, LOW);
        digitalWrite(stepper_pin_3, LOW);
        digitalWrite(stepper_pin_4, HIGH);
        break;
      }
    } else {
      //Serial.println("Close and cnt is  " + cnt);
      switch(step_number){
        case 0:
        digitalWrite(stepper_pin_1, LOW);
        digitalWrite(stepper_pin_2, LOW);
        digitalWrite(stepper_pin_3, LOW);
        digitalWrite(stepper_pin_4, HIGH);
        break;
        case 1:
        digitalWrite(stepper_pin_1, LOW);
        digitalWrite(stepper_pin_2, LOW);
        digitalWrite(stepper_pin_3, HIGH);
        digitalWrite(stepper_pin_4, LOW);
        break;
        case 2:
        digitalWrite(stepper_pin_1, LOW);
        digitalWrite(stepper_pin_2, HIGH);
        digitalWrite(stepper_pin_3, LOW);
        digitalWrite(stepper_pin_4, LOW);
        break;
        case 3:
        digitalWrite(stepper_pin_1, HIGH);
        digitalWrite(stepper_pin_2, LOW);
        digitalWrite(stepper_pin_3, LOW);
        digitalWrite(stepper_pin_4, LOW);
      } 
    }
    step_number++;
    if(step_number > 3)
      step_number = 0;
    cnt++;
    delay(2);
  }
}
