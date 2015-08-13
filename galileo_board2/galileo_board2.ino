 //galileo 2
#include <LiquidCrystal.h> 
//#include <Servo.h>
#include <pt.h>

#define PT_DELAY(pt, ms, ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

//#define LDR A5
#define TEMP A4
#define TEMP2 A1
#define PEEP 10
#define SSR 10
#define MINR 110
#define MAXR 190

//Servo myservo;
struct pt pt_taskReqStr;
struct pt pt_taskSetAir;
struct pt pt_taskLight;
struct pt pt_taskTemp;
struct pt pt_taskSendSerial;

String reqStr = "";
char air1, air2;
int temp, temp2;
LiquidCrystal lcd (8,9,4,5,6,7);

PT_THREAD(taskReqStr(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    reqestSerial();
    PT_DELAY(pt, 600, ts);
  }
  PT_END(pt);
}

PT_THREAD(taskSetAir(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    setAir();
    PT_DELAY(pt, 600, ts);
  }
  PT_END(pt);
}

/*
PT_THREAD(taskLight(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    lightLoop();
    PT_DELAY(pt, 600, ts);
  }
  PT_END(pt);
}
*/

PT_THREAD(taskTemp(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    tempLoop();
    PT_DELAY(pt, 600, ts);
  }
  PT_END(pt);
}

PT_THREAD(taskSendSerial(struct pt* pt)) {
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1) {
    sendSerial();
    PT_DELAY(pt, 600, ts);
  }
  PT_END(pt);
}

void reqestSerial() {
  //Serial.println(Serial1.available());
  if(Serial1.available() > 0) {
    reqStr = Serial1.readStringUntil('\r');
    //Serial.print(reqStr);
    air1 = reqStr[0];
    air2 = reqStr[2];
  }
}

void sendSerial() {
  Serial1.print(String(temp));
  Serial1.print(","+String(temp2));
  Serial1.print('\r');
}

void setAir() {
  // air
  //Serial.println(String(air1) + "," +String(air2));
  if(air1 == '1') {
    digitalWrite(SSR, HIGH);
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(SSR, LOW);
    digitalWrite(13, LOW);
  }
  if(air2 == '1') {
    digitalWrite(14, HIGH);
  } else {
    digitalWrite(14, LOW);
  }
}

/*
int r = MINR;
void openServo() {
  for(; r <= MAXR; r += 5) {
    myservo.write(r);
    delay(50);
  }
}

void closeServo() {
  for(; r >= MINR; r -= 5) {
    myservo.write(r);
    delay(50);
  }
}
*/
/*
void lightLoop() {
  int ldr = analogRead(LDR);
  Serial.println("light: "+ String(ldr));
  if(ldr < 100) {
    closeServo();
  } else {
    openServo();
    //peep();
  }
}
*/
void peep() {
  for(int i = 0; i < 5; ++i) {
    analogWrite(PEEP, 100);
    delay(500);
    analogWrite(PEEP, 0);
    delay(500);
  }
}

void tempLoop() {
  temp = (25*analogRead(TEMP) - 2050)/100;
  temp2 = (25*analogRead(TEMP2) - 2050)/100;
  //Serial.println("Temp: "+ String(temp));
  lcd.setCursor(0, 0);
  lcd.print("Temperature : ");
  lcd.setCursor(0, 1);
  lcd.print(String(temp)+","+String(temp2));
  delay(500);
}

void setup() {
  PT_INIT(&pt_taskLight);
  PT_INIT(&pt_taskTemp);
  PT_INIT(&pt_taskReqStr);
  PT_INIT(&pt_taskSetAir);
  PT_INIT(&pt_taskSendSerial);
  Serial.begin(9600);
  Serial1.begin(115200);
  lcd.begin(16, 2);
  //pinMode(LDR, INPUT);
  pinMode(TEMP, INPUT);
  pinMode(TEMP2, INPUT);

  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(PEEP, OUTPUT);
 // myservo.attach(11);
}

void loop() {
  taskReqStr(&pt_taskReqStr);
  taskSetAir(&pt_taskSetAir);
  //taskLight(&pt_taskLight);
  taskTemp(&pt_taskTemp);
  taskSendSerial(&pt_taskSendSerial);
  reqestSerial();
}
