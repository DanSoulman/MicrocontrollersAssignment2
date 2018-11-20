//Whatever the hell Assignment 2 does
//By Dan Coleman R00151926


#include "mbed.h"
//Screen and temp Imports----
#include "TextLCD.h"
#include "C12832.h"
#include "LM75B.h"
//---------------------------

C12832 lcd(p5, p7, p6, p8, p11); //Does the Screen bit
LM75B tmp(p28,p27); //I2C Temperature Sensor

//RGB LED with 3 PWM outputs for dimmer control---
PwmOut r(p23);
PwmOut g(p24);
PwmOut b(p25);
//------------------------------------------------

PwmOut speaker(p26); //Speaker with PWM driver
Serial pc(USBTX,USBRX); //used for printf to PC over USB

AnalogIn pot1(p19); //Reads Pot 1 - near LCD: This will be used to set the temp threshold 
AnalogIn pot2(p20); //Reads Pot 2 - near RGB LED: This will be used to set the vibration threshold


int main() {
      //Variable declarations-----------------------------
        float defTempThreshold = 30;//default Temp Threshold
        float defDoNotShakeThreshold = 1; //default vibration threshold
        //RGB LED off
        r=1.0;  g=1.0;  b=1.0;
      while (1) {// loops consistantly to allow for change if values
        
        
      //Sets Adjustable Threshold Values---------------------------------------
      //Varying TempTreshold with Pot 1
      float tempThreshold = (defTempThreshold * pot1.read());
      //Varying Vibration Threshold with Pot 2
      float vibrationThreshold = (defDoNotShakeThreshold * pot2.read());
        //Printing the temp to LCD--------------------------
        lcd.cls(); //Clears screen          TODO: Optimize
        lcd.locate(0,0); //Puts us at pixel location 0,3
        lcd.printf("Temp is %.2f", tmp.read()); //Prints temp
        lcd.locate(0, 10); // Set new location for print
        lcd.printf("Temp Treshold: %.2f", tempThreshold);
        lcd.locate(0,20);  // Set new location for print
        lcd.printf("Vibration Treshold: %.2f", vibrationThreshold);
        //--------------------------------------------------
        
       //If the Temperature is above the temp Treshold it flashes red and an alarm is set off
        if(tmp.read() > tempThreshold) {
            r = 0.0;
            g = 1.0;
            speaker = 0.5; //alarm tone using PWM
        } else { //Turns off light and sound
            r = 1.0;
            g =0.0;
            speaker = 0.0;
        }
        wait(0.5);
     }
}