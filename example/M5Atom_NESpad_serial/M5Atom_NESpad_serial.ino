#include <M5Atom.h>

/* NESpad Arduino library
* https://github.com/joshmarinacci/nespad-arduino   
*/
#include <NESpad.h>

#define nesStrobe 22
#define nesClock 19
#define nesData 23


// put your own strobe/clock/data pin numbers here -- see the pinout in readme.txt
NESpad nintendo = NESpad(nesStrobe,nesClock,nesData);

byte state = 0;

void setup() {
  // M5.begin(SerialEnable, I2CEnable, DisplayEnable);
  M5.begin(true, false, true);
//  Serial.begin(115200);  

  M5.dis.drawpix(0, 0x0000FF);  //LED BLUE
  delay(100);

  Serial.println("NES Pad Test");
}

void loop() {
  
  state = nintendo.buttons();

  // shows the shifted bits from the joystick
  // buttons are high (1) when up 
  // and low (0) when pressed
  Serial.println(~state, BIN);

  if(state == 0){
    M5.dis.drawpix(0, 0x0000FF);  //LED BLUE
  }else{
    M5.dis.drawpix(0, 0xFF0000);  //LED RED
    if(state & NES_A){
      Serial.println("NES_A");
    }else if(state & NES_B){
      Serial.println("NES_B");
    }else if(state & NES_SELECT){
      Serial.println("NES_SELECT");
    }else if(state & NES_START){
      Serial.println("NES_START");
    }else if(state & NES_UP){
      Serial.println("NES_UP");
    }else if(state & NES_DOWN){
      Serial.println("NES_DOWN");
    }else if(state & NES_LEFT){
      Serial.println("NES_LEFT");
    }else if(state & NES_RIGHT){
      Serial.println("NES_RIGHT");
    }else{
      Serial.println("Unknown");
    }
  }

  delay(100);
}
