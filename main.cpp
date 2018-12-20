//Temperature and Vibration detection alarm
//By Dan Coleman R0015192

//Current State: Missing the USB part but I ran out of time to add it. 

#include "mbed.h"
//Screen and temp Imports----
#include "TextLCD.h"
#include "C12832.h"
#include "LM75B.h"
#include "MMA7660.h"
//---------------------------

C12832 lcd(p5, p7, p6, p8, p11); //LCD Screen 
LM75B tmp(p28,p27); //I2C Temperature Sensor

//LEDS-----------------
DigitalOut redLED(p23);
PwmOut blueLED(p25);
//---------------------

//Pins for the accelerometer
MMA7660 acc(p28, p27);
//--------------------------

PwmOut speaker(p26); //Speaker with PWM driver
Serial pc(USBTX,USBRX); //used for printf to PC over USB

AnalogIn pot1(p19); //Reads Pot 1 - near LCD: This will be used to set the vibration threshold 
AnalogIn pot2(p20); //Reads Pot 2 - near RGB LED: This will be used to set the temp threshold

//Tickers---------
Ticker blueSwapper; //Flashes the blueLED
Ticker redSwapper; //Flashes the redLED
//----------------

bool attachedBlue = false; //Used to attach blueSwapper 

 //Global Variable declarations-----------------------------
float defTempThreshold = 30;//default Temp Threshold
float defDoNotShakeThreshold = 0.5; //default vibration threshold
float tempThreshold = 0; //Used down below to vary the temperature threshold
float vibrationThreshold = 0; //Used down below to vary the vibration threshold
float TotalVibration = 0; //Used to work out the total vibration used 
//------------------------------------------------------------

//Clears Screen
void screenClear(){
     lcd.cls(); //Clears screen          
     lcd.locate(0,0); //Puts us at pixel location 0,0
    }


//This prints a warning showing what the microcontroller is exceeding 
void screenWarning(int option){
    switch(option) {

    case 1  :
    screenClear();
    lcd.printf("VIBRATION & TEMP\nEXCEEDED \nFIX THEN RESET");
    wait(0.5);
    break;

    case 2  :
    screenClear();
    lcd.printf("DANGER \nEXCEEDED VIBRATION \nTHRESHOLD");
    wait(0.5);
    break;
  }
}

//Turns the BlueLED on/off (swapped from previous state) 
void swapBlueLED(){
    blueLED = !blueLED;
}

//Turns the redLED on/off (swapped from previous state
void swapRedLED(){
    redLED = !redLED;
}

//Checks temperature and turns on alarm if needed. 
void temperatureChecking(float tempThreshold){
        while(true){
            //Potentiometer affects the threshold
            tempThreshold = (defTempThreshold * pot2.read());

            //If the Temperature is above the temp Treshold it flashes red and an alarm is set off            
            if(tmp.read() > tempThreshold) {
                
                //Swaps from Blue LED to Red
                blueSwapper.detach();
                blueLED = 1;
                redSwapper.attach(&swapRedLED, 0.2);
                
                //turns on alarm and warning
                speaker.period(1/200.0);
                speaker = 0.5;
                screenWarning(1);
            }
            
        }
        
}

//Checks the vibration is above the threshold 
void vibrationChecking(){

        //Checks if the vibration is above the threshold
        if(TotalVibration > vibrationThreshold){
            //Prints Screen warning about the vibration Threshold getting exceeded
            screenWarning(2);
            
            
            //Flashes blue
            if(attachedBlue == false){
                blueSwapper.attach(&swapBlueLED, 1);
            }
            
            //Checks Temperature
            temperatureChecking(tempThreshold);
        }
     }
     
//I made this just to check individual features
void debugPrint(float xAxis, float yAxis, float zAxis){
    screenClear();
    //lcd.printf(" xAxis = %.2f\n yAxis = %.2f\n zAxis = %.2f\n " , xAxis, yAxis, zAxis);
    //lcd.printf(" xAxis = %.2f\n yAxis = %.2f\n zAxis = %.2f\n " , -xAxis, -yAxis, -zAxis);
    }



int main() {
       //RGB LED off
        redLED = 1;
        blueLED = 1;
        
        //Axis for vibration
        float xAxis;
        float yAxis;
        float zAxis;
        
                
        while (true) {// loops consistantly to allow for change if values     
            //Sets Vibration Axis
            xAxis = acc.x();
            yAxis = acc.y();
            zAxis = acc.z();
            //-------------------
            
            //Varying thresholds with potentiometers----------------------------
            //Varying temperature Threshold with Pot 1
            tempThreshold = (defTempThreshold * pot2.read());
            //Varying Vibration Threshold with Pot 1
            vibrationThreshold = (defDoNotShakeThreshold * pot1.read());
            //------------------------------------------------------------------
      
      
            //Reads in the axis for Vibration-----------------------------------------------  
            TotalVibration = sqrt ((xAxis *xAxis) + (yAxis * yAxis) + (zAxis * zAxis)); //might change this as Nash said the formaulae goes above the max
            TotalVibration -= 0.98; //This was my (poor) attempt to account for gravity 
            vibrationChecking();//Checks to see if the vibration exceeds the threshold 
            //--------------------------------------------------------------------------------
      
            //Printing the temp to LCD--------------------------
            screenClear();
            lcd.printf("Temp %.2f Vibration %.2f \nTemp Treshold: %.2f \nVibration Treshold: %.2f" ,tmp.read(),TotalVibration,tempThreshold,vibrationThreshold);
            wait(0.5);
            //--------------------------------------------------    
        
        }//End of while loop    

    }//End of main
