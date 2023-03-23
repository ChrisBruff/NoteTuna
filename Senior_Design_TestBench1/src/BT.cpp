#include <iostream>
using namespace std;
#include <BluetoothSerial.h>
#include <Arduino.h>


char cmd;

BluetoothSerial serialBT;

void BT_setup(){
    Serial.begin(9600);
  serialBT.begin("Esp32-BT");
  pinMode(2,OUTPUT);
  Serial.println("setup executed");
  if(!(CONFIG_BT_ENABLED) || !(CONFIG_BLUEDROID_ENABLED)){
    Serial.println("error1");
  }
}

void BT_loop(){//MAIN LOOP  
    if(serialBT.available()){
        cmd = serialBT.read();
    }
    if(cmd == '1'){
        digitalWrite(2, HIGH);
    }
    if(cmd == '0'){
        digitalWrite(2, LOW);
    }
    delay(20);
}
