
#include "config.h" //Pin Config
#include <Wire.h>
#include <EEPROM.h>

int magPinL = 8;
int magPinR = 7;

bool isROn = false;
bool isLOn = false;

bool prevL = false;
bool prevR = false;

int acwStage = -1;
int cwStage = -1;


long prevLMillis = 0;
long prevRMillis = 0;

#ifdef USE_OSC
#include <OSCMessage.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#define DELAY_RESET 500

EthernetUDP Udp;
//the Arduino's IP
int ip_end = 20 + ID;

IPAddress ip(192, 168, 100, ip_end);

//destination IP
IPAddress outIp(192, 168, 100, 255);
//destination PORT
const unsigned int outPort = 9001;

IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 100, 1);

/*
  int ip_end = 100 + ID;
  IPAddress ip(169, 254, 159, ip_end);

  //destination IP
  IPAddress outIp(169, 254, 159, 255);
  //destination PORT
  const unsigned int outPort = 9001;

  IPAddress subnet(255, 255, 0, 0);
  IPAddress gateway(169, 254, 0, 1);
*/
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; // you can find this written on the board of some Arduino Ethernets or shields

OSCMessage msgCW("/dir/cw");
OSCMessage msgACW("/dir/acw");
OSCMessage msgTest("/dir/test");

#endif

int LEDPin = 13;

void setup()
{

  Serial.begin(9600);
#ifdef USE_SERIAL
  Serial.println("magnet:");
#endif

#ifdef USE_OSC
  Ethernet.begin(mac, ip, gateway, subnet);
  Udp.begin(8888);

  Serial.print("IP = ");
  Serial.println(Ethernet.localIP());

  msgTest.add((int32_t)ID);

  Udp.beginPacket(outIp, outPort);
  msgTest.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msgTest.empty(); // free space occupied by message

  delay(DELAY_RESET);
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
      prevLMillis = millis();
    } else {

    }
    prevL = valL;
  }
  if (prevR != valR) {
    if (prevL == LOW) {
      isROn = true;
          prevRMillis = millis();
    } else {

    }
    prevR = valR;
  }
  if (millis() - prevLMillis > 500 && acwStage == 0) {
    acwStage = -1;
    isLOn = false;
    //Serial.println("ACW cancelled");
  }
  if (millis() - prevRMillis > 500 && cwStage == 0) {
    cwStage = -1;
    isROn = false;
    //Serial.println("CW cancelled");
  }
  if (isLOn && !isROn) {
    acwStage = 0;
    //Serial.println("ACW Stage 0");
  }
  if (isROn && !isLOn) {
    cwStage = 0;
     //   Serial.println("CW Stage 0");
  }
  if (acwStage == 0 && isROn  && millis() - prevRMillis < 250) {
    acwStage = 1;
  }
  if (cwStage == 0 && isLOn  && millis() - prevLMillis < 250) {
    cwStage = 1;
  }

  if (cwStage == 1) {
#ifdef USE_SERIAL
    Serial.println("ACW");
#endif

#ifdef USE_OSC
    msgACW.add((int32_t)ID);

    Udp.beginPacket(outIp, outPort);
    msgACW.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msgACW.empty(); // free space occupied by message

#endif

    cwStage = -1;
    isLOn = false;
    isROn = false;
  }
  if (acwStage == 1) {
#ifdef USE_SERIAL
    Serial.println("CW");
#endif

#ifdef USE_OSC
    msgCW.add((int32_t)ID);

    Udp.beginPacket(outIp, outPort);
    msgCW.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msgCW.empty(); // free space occupied by message

#endif
    acwStage = -1;
    isLOn = false;
    isROn = false;
  }
  if (acwStage == -1 && cwStage == -1) {
    //Serial.println("ready");
  }


  if (Serial.available() > 0) {
    // get incoming byte:
    char inByte = Serial.read();
    if (inByte == 'p') {
#ifdef USE_OSC
      testOSC();
#endif
    }
  }
}
#ifdef USE_OSC
void testOSC() {
  digitalWrite(LEDPin, HIGH);

  msgTest.add((int32_t)ID);

  Udp.beginPacket(outIp, outPort);
  msgTest.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msgTest.empty(); // free space occupied by message

  Serial.println("test OSC");
  digitalWrite(LEDPin, LOW);

}
#endif
