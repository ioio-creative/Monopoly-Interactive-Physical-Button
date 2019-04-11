/*
    Make an OSC message and send it over UDP
*/
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OSCMessage.h>

#define DELAY_RESET 500

EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(169, 254, 255, 1);
//IPAddress ip(169, 254, 208, 209);
//destination IP
//IPAddress outIp(169, 254, 208, 255);
IPAddress outIp(169, 254, 255, 255);
const unsigned int outPort = 9001;


IPAddress subnet(255, 255, 0, 0);
IPAddress gateway(169, 254, 0, 1);

OSCMessage msgPlay("/video/play");
OSCMessage msgReset("/video/reset");

bool isResetAndPlay = false;

long prevMillis = 0;

int inPin = A0;
int LEDPin = 13;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; // you can find this written on the board of some Arduino Ethernets or shields
void setup() {
  //Ethernet.begin(mac,ip);

  pinMode(inPin, INPUT_PULLUP); 

  Ethernet.begin(mac, ip, gateway, subnet);
  Udp.begin(8888);
  Serial.begin(9600);

  Serial.print("IP = ");
  Serial.println(Ethernet.localIP());

  msgReset.add((int32_t)random(0, 255));

  Udp.beginPacket(outIp, outPort);
  msgReset.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msgReset.empty(); // free space occupied by message

  delay(DELAY_RESET);

}


void loop() {
 // Serial.println(analogRead(inPin));
 /* if (digitalRead(inPin) == HIGH && (millis() - prevMillis) > 2000) {
    isResetAndPlay = true;
    prevMillis = millis();
  }*/

if (Serial.available() > 0) {
    // get incoming byte:
    char inByte = Serial.read();
    if(inByte == 'p'){
      resetAndPlay();
    } 
  }
  if (isResetAndPlay) {
    resetAndPlay();
  }
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From IP : ");

    IPAddress remote = Udp.remoteIP();
    //print out the remote connection's IP address
    Serial.print(remote);

    Serial.print(" on port : ");
    //print out the remote connection's port
    Serial.println(Udp.remotePort());
  }

}

void resetAndPlay() {

  digitalWrite(LEDPin, HIGH);

  msgReset.add((int32_t)random(0, 255));

  Udp.beginPacket(outIp, outPort);
  msgReset.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msgReset.empty(); // free space occupied by message

  delay(DELAY_RESET);

  msgPlay.add((int32_t)random(0, 255));

  Udp.beginPacket(outIp, outPort);
  msgPlay.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msgPlay.empty(); // free space occupied by message
  Serial.println("reset and play");
    digitalWrite(LEDPin, LOW);

  isResetAndPlay = false;
}
