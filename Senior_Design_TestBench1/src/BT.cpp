
#include <BluetoothSerial.h>
#include <Arduino.h>


char cmd;

BluetoothSerial serialBT;

void BT_setup(){
  serialBT.begin("Esp32-BT");
  pinMode(2,OUTPUT);
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
