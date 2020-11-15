#include <SoftwareSerial.h>
#define ai_1 A5
#define stepper_pin_1 4
#define stepper_pin_2 5
#define stepper_pin_3 6
#define stepper_pin_4 7
#define above_ir 9
#define below_ir 8
int androidCommand;
enum STATE {
  OPENED, CLOSED, HALF
};
STATE isOpened;
byte above;
byte below;
int shine = 0;
SoftwareSerial bt(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(stepper_pin_1, OUTPUT);
  pinMode(stepper_pin_2, OUTPUT);
  pinMode(stepper_pin_3, OUTPUT);
  pinMode(stepper_pin_4, OUTPUT);
  pinMode(above_ir, INPUT);
  pinMode(below_ir, INPUT);
  delay(500);
  above = digitalRead(above_ir);
  below = digitalRead(below_ir);
  //delay(2000);
  if(above == HIGH) {
    isOpened = OPENED;
  }else if(below == LOW){
    isOpened = CLOSED;
  }else { 
    isOpened = HALF;
  }
  switch(isOpened) {
    case OPENED:
    Serial.println("OPENED");
    break;
    case CLOSED:
    Serial.println("CLOSED");
    break;
    case HALF:
    Serial.println("HALF");
    break;
  }
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
  above = digitalRead(above_ir);
  below = digitalRead(below_ir);
}

void mOpen() {
  if(isOpened == CLOSED || isOpened == HALF) {
    //actionWithJalousie(false);
    openJalousie();
    isOpened = OPENED;
    bt.write("opened");
  }
}

void mClose() {
  if(isOpened == OPENED || isOpened == HALF) {
    // actionWithJalousie(true);
    closeJalousie();
    isOpened = CLOSED;
    bt.write("closed");
  }
}


void openJalousie() {
  int step_number = 0;
  while(above != HIGH) {
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
    step_number++;
    if(step_number > 3)
      step_number = 0;
    above = digitalRead(above_ir);
    delay(2);
  }
}

void closeJalousie() {
  int cnt = 0;
  int step_number = 0;
  while(below != LOW) {
      //Serial.println("Close and cnt is  " + cnt);
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
    step_number++;
    if(step_number > 3)
      step_number = 0;
    below = digitalRead(below_ir);
    delay(2);
  }
}
