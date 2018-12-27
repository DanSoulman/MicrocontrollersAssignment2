//Temperature and Vibration detection alarm
//By Dan Coleman R0015192

//Current State: Missing the logging. Ran out of time.

#include "mbed.h"
//Screen and temp Imports----
#include "TextLCD.h"
#include "C12832.h"
#include "LM75B.h"
#include "MMA7660.h"
//---------------------------

C12832 lcd(p5, p7, p6, p8, p11); //LCD Screen 
LM75B temperatureSensor(p28,p27); //I2C Temperature Sensor used to read current temperature

//LEDS-----------------
DigitalOut redLED(p23);
PwmOut blueLED(p25);
//---------------------

//Pins for the accelerometer
MMA7660 accelerometer(p28, p27);
//--------------------------

PwmOut speaker(p26); //Speaker with PWM driver
Serial pc(USBTX,USBRX); //used for printf to PC over USB

AnalogIn pot1(p19); //Reads Potentiometer 1 - near LCD: This will be used to set the vibration threshold 
AnalogIn pot2(p20); //Reads Potentiometer 2 - near RGB LED: This will be used to set the temp threshold

//Tickers---------
Ticker blueSwapper; //Flashes the blueLED
Ticker redSwapper; //Flashes the redLED
//----------------

//Timer redLEDTimer; //For measuring response time (check ticker really is 0.2): Final Response time: 0.200017. 
//I commented back out this test, as it was really more for my own curioisity that the response time was correct

bool bAttachedBlue = false; //Used to attach blueSwapper 

 //Global Variable declarations-----------------------------
const float fdefaultTempThreshold = 30;//default Temp Threshold
const float fdefaultVibrationThreshold = 0.5; //default vibration threshold
float fAMPLITUDE_CHANGE = 0.75; //Used to change speaker if amplitude of vibration exceeds enough
float ftempThreshold = 0; //Used down below to vary the temperature threshold
float fvibrationThreshold = 0; //Used down below to vary the vibration threshold
float ftotalVibration = 0; //Used to work out the total vibration used 
//------------------------------------------------------------

//Clears Screen
void screenClear(){
     lcd.cls(); //Clears screen          
     lcd.locate(0,0); //Puts us at pixel location 0,0
    }

//I made this just to check individual features
//void debugPrint(float xAxis, float yAxis, float zAxis){
void debugPrint(){
    screenClear();
    //lcd.printf("Time Taken was %f", redLEDTimer.read()); //This wsa used to check response time was correct
    wait(4);
    //lcd.printf(" xAxis = %.2f\n yAxis = %.2f\n zAxis = %.2f\n " , xAxis, yAxis, zAxis);
    //lcd.printf(" xAxis = %.2f\n yAxis = %.2f\n zAxis = %.2f\n " , -xAxis, -yAxis, -zAxis);
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
    //redLEDTimer.stop();
    redLED = !redLED;
}

//Checks temperature and turns on alarm if needed. 
void temperatureChecking(float ftempThreshold){
    while(true){
        //Potentiometer affects the threshold
        ftempThreshold = (fdefaultTempThreshold * pot2.read());

        //If the Temperature is above the temp Treshold it flashes red and an alarm is set off            
        if(temperatureSensor.read() > ftempThreshold) {
            
            //Swaps from Blue LED to Red
            blueSwapper.detach();
            blueLED = 1;
            //redLEDTimer.start();
            //This attached the RED LED with a response time of 0.2 seconds (techically 0.200017)
            redSwapper.attach(&swapRedLED, 0.2);
            
            //turns on alarm 
            if(ftotalVibration < fAMPLITUDE_CHANGE){
                //A very basic way of having the alarm change based on amplitude of vibration
                speaker.period(1/200.0);
                speaker = 0.5;
            }
            else{
                speaker.period(1/150.0);
                speaker = 0.6;    
            }
            //Warns vibration exceeded
            screenWarning(1);
        }
        
    }
        
}

//Checks the vibration is above the threshold 
void vibrationChecking(){

    //Checks if the vibration is above the threshold
    if(ftotalVibration > fvibrationThreshold){
        //Prints Screen warning about the vibration Threshold getting exceeded
        screenWarning(2);
        
        
        //Flashes blue
        if(bAttachedBlue== false){
            blueSwapper.attach(&swapBlueLED, 1);
        }
        
        //Checks Temperature
        temperatureChecking(ftempThreshold);
    }
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
        xAxis = accelerometer.x();
        yAxis = accelerometer.y();
        zAxis = accelerometer.z();
        //-------------------
        
        //Varying thresholds with potentiometers----------------------------
        //Varying temperature Threshold with Pot 1
        ftempThreshold = (fdefaultTempThreshold * pot2.read());
        //Varying Vibration Threshold with Pot 1
        fvibrationThreshold = (fdefaultVibrationThreshold * pot1.read());
        //------------------------------------------------------------------
    
    
        //Reads in the axis for Vibration-----------------------------------------------  
        ftotalVibration = sqrt ((xAxis *xAxis) + (yAxis * yAxis) + (zAxis * zAxis)); //might change this as Nash said the formaulae goes above the max
        ftotalVibration -= 0.98; //This was my (poor) attempt to account for gravity 
        vibrationChecking();//Checks to see if the vibration exceeds the threshold 
        //--------------------------------------------------------------------------------
    
        //Printing the temp to LCD--------------------------
        screenClear();
        lcd.printf("Temp %.2f Vibration %.2f \nTemp Treshold: %.2f \nVibration Treshold: %.2f" ,temperatureSensor.read(),ftotalVibration,ftempThreshold,fvibrationThreshold);
        wait(0.5);
        //--------------------------------------------------    
    
    }//End of while loop    

}//End of main
