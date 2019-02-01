#include <Wire.h>

int magPinL = 8;
int magPinR = 7;

bool isROn = false;
bool isLOn = false;

bool prevL = false;
bool prevR = false;

int acwStage = -1;
int cwStage = -1;

void setup()
{

  Serial.begin(9600);
#ifdef USE_SERIAL
  Serial.println("magnet:");
#endif
  pinMode(magPinL, INPUT);
  pinMode(magPinR, INPUT);

  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
}
int valL, valR;
void loop()
{
  valL = digitalRead(magPinL);
  valR = digitalRead(magPinR);
  /*
    if (valL == HIGH) {
    //  Serial.print("L");
    //Serial.println(valL);
    isLOn = true;
    }
    if (valR == HIGH) {
    // Serial.print("R");
    //   Serial.println(valR);
    isROn = true;
    }
  */
  if (prevL != valL) {
    if (prevL == LOW) {
      isLOn = true;
    } else {

    }
    prevL = valL;
  }
  if (prevR != valR) {
    if (prevL == LOW) {
      isROn = true;
    } else {

    }
    prevR = valR;
  }

  if (isLOn && !isROn) {
    acwStage = 0;
  }
  if (isROn && !isLOn) {
    cwStage = 0;
  }
  if (acwStage == 0 && isROn) {
    acwStage = 1;
  }
  if (cwStage == 0 && isLOn) {
    cwStage = 1;
  }

  if (cwStage == 1) {
#ifdef USE_SERIAL
    Serial.println("ACW");
#endif

    cwStage = -1;
    isLOn = false;
    isROn = false;
  }
  if (acwStage == 1) {
#ifdef USE_SERIAL
    Serial.println("CW");
#endif
    acwStage = -1;
    isLOn = false;
    isROn = false;
  }
  if (acwStage == -1 && cwStage == -1) {
    //Serial.println("ready");
  }
}
