//Whatever the hell Assignment 2 does
//By Dan Coleman R00151926


#include "mbed.h"
//Screen and temp Imports----
#include "TextLCD.h"
#include "C12832.h"
#include "LM75B.h"
#include "MMA7660.h"
//---------------------------

C12832 lcd(p5, p7, p6, p8, p11); //Does the Screen bit
LM75B tmp(p28,p27); //I2C Temperature Sensor

//RGB LED with 3 PWM outputs for dimmer control---
PwmOut r(p23);
PwmOut g(p24);
PwmOut b(p25);
//------------------------------------------------

//Pins for the accelerometer
MMA7660 acc(p28, p27);
//--------------------------

PwmOut speaker(p26); //Speaker with PWM driver
Serial pc(USBTX,USBRX); //used for printf to PC over USB

AnalogIn pot1(p19); //Reads Pot 1 - near LCD: This will be used to set the temp threshold 
AnalogIn pot2(p20); //Reads Pot 2 - near RGB LED: This will be used to set the vibration threshold


void screenWarning(int option){
    switch(option) {

    case 1  :
    lcd.cls(); //Clears screen
    lcd.locate(0,0); //Puts us at pixel location 0,0
    lcd.printf("DANGER \nVIBRATION AND \nTEMPERATURE TOO HIGH !!! ");
    wait(1);
    break;

    case 2  :
    lcd.cls(); //Clears screen
    lcd.locate(0,0); //Puts us at pixel location 0,0
    lcd.printf("DANGER \nVIBRATION TOO HIGH");
    wait(1);
    break;
    
    case 3  :
    lcd.cls(); //Clears screen
    lcd.locate(0,0); //Puts us at pixel location 0,0
    lcd.printf("DANGER \nTEMPERATURE TOO HIGH");
    wait(1);
    break;      
    
    default  :
    lcd.cls(); //Clears screen
    lcd.locate(0,0); //Puts us at pixel location 0,0
    lcd.printf("Everything is in order");
    wait(1);
    break;   
    
    }
}






void sensorChecking(float tempThreshold, float vibrationThreshold, float TotalVibration){
        //If the vibration and Temp are above Thresholds gets more obnoxious
        if(tmp.read() > tempThreshold && TotalVibration > vibrationThreshold){
           r = 0.0;
           g = 1.0;
           b = 1.0;
           speaker.period(1/300.0);
           speaker = 0.5;
           wait(1);
           screenWarning(1);  
           
        }else if(TotalVibration > vibrationThreshold){
            b = 0.0;
            r = 1.0;
            g = 1.0;
            speaker.period(1/200.0);
            speaker = 0.5;
            wait(1);
            screenWarning(2);
           
        //If the Temperature is above the temp Treshold it flashes red and an alarm is set off
        }else if(tmp.read() > tempThreshold) {
            b = 0.0;
            r = 1.0;
            g = 1.0;
            speaker.period(1/200.0);
            speaker = 0.5;
            wait(1);
            screenWarning(3);
            
        //Turns off light and sound
        } else { 
            r = 1.0;
            g = 0.0;
            b = 1.0;
            speaker = 0.0;
            wait(1);
        //screenWarning(4); removed for being unnessicary 
        }   
     }
     
//I made this just to check individual features
void debugPrint(float xAxis, float yAxis, float zAxis){
    lcd.cls(); //Clears screen
    lcd.locate(0,0); //Puts us at pixel location 0,0
    //lcd.printf(" xAxis = %.2f\n yAxis = %.2f\n zAxis = %.2f\n " , xAxis, yAxis, zAxis);
    //lcd.printf(" xAxis = %.2f\n yAxis = %.2f\n zAxis = %.2f\n " , -xAxis, -yAxis, -zAxis);
    }


int main() {
      //Variable declarations-----------------------------
        float defTempThreshold = 30;//default Temp Threshold
        float defDoNotShakeThreshold = 1.5; //default vibration threshold
        //RGB LED off
        r=1.0;  g=1.0;  b=1.0;
        
      while (1) {// loops consistantly to allow for change if values     
        //Sets Adjustable Threshold Values---------------------------------------
        //Varying TempTreshold with Pot 1
        float tempThreshold = (defTempThreshold * pot1.read());
        //Varying Vibration Threshold with Pot 2
        float vibrationThreshold = (defDoNotShakeThreshold * pot2.read());
      
      
       //Reads in the axis for Vibration-----------------------------------------------
        float xAxis = acc.x();
        float yAxis = acc.y();
        float zAxis = acc.z();
        float TotalVibration = sqrt ((xAxis *xAxis) + (yAxis * yAxis) + (zAxis * zAxis)); //might change this as Nash said the formaulae goes above the max
      //--------------------------------------------------------------------------------
      
        //Printing the temp to LCD--------------------------
        lcd.cls(); //Clears screen          TODO: Optimize
        lcd.locate(0,0); //Puts us at pixel location 0,3
        lcd.printf("Temp %.2f Vibration %.2f", tmp.read(),TotalVibration); //Prints temp
        lcd.locate(0, 10); // Set new location for print
        lcd.printf("Temp Treshold: %.2f", tempThreshold);
        lcd.locate(0,20);  // Set new location for print
        lcd.printf("Vibration Treshold: %.2f", vibrationThreshold);
        //--------------------------------------------------
        
        
        //debugPrint(xAxis, yAxis, zAxis);
        sensorChecking(tempThreshold, vibrationThreshold, TotalVibration);
        wait(0.5);
     }
}