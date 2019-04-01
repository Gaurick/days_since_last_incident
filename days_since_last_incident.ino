
/*
 * After receiving call after call from my kids' daycare, decided to use
 * spare parts to make an automatic "Days since last incident" counter.
 * Since it's got a built in Real Time Clock, why not add a clock to it?
 * 
 * Uses 5v Trinket Pro,
 * https://www.adafruit.com/product/2000
 * Seven Segment Backpack,
 * https://www.adafruit.com/product/878
 * Alpha Numeric display,
 * https://www.adafruit.com/product/1911
 * Real Time Clock
 * https://www.elegoo.com/
 * 
 * Tied the two LED backpacks and the RTC together and connected the lines to...
 * VCC to +5v.
 * Gnd to Gnd.
 * SDA to A4.
 * SCL to A5.
 * 
 * Push button connects...
 * one side to Gnd.
 * other side to 3.
 * 
 * 
 * To do...
 * Put on a sign.
 * Take pictures?
 * I'm pretty happy with it right now.
 * 
 */


#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
//libraries to make the parts do the things.

DS3231 clock;
RTCDateTime dt;
//startup and set the object for the real time clock.

Adafruit_7segment matrix = Adafruit_7segment();
//set the object for the seven segment display for the days since last incident.

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
//set the object for the alpha-numeric display.

int today = 0;
//variable to hold onto today.
int now = 0;
//variable for right now.
int incident = 0;
//how many days without incident have passed.
int counter = 0;
//counter to make the colon blink.
bool colon = 0;
//flag for the colon to turn on or off.

void setup() {
  Serial.begin(9600);
  //start the serial debugger for debugging.

  clock.begin();
  //start up the clock stuff.

  pinMode(3, INPUT_PULLUP);
  //set pin 3 to be used on the button and use the trinket's internal pull up resistor for it.

  matrix.begin(0x71);
  //startup the seven segment thing and set the address.

  alpha4.begin(0x70);

  dt = clock.getDateTime();
  //check the time.
  today = dt.day;
  //set today as today.

  alpha4.clear();
  //clear the alpha-numeric display.

  //clock.setDateTime(__DATE__, __TIME__);
  //uncomment to set the time on the realtime clock.
  //otherwise leave it commented out since it'll mess things up.

  alpha4.setBrightness(0);
  matrix.setBrightness(0);
  //dim the led displayes.

}

void loop() {
  int sensorVal = digitalRead(3);
  //create a variable and read if the button is pressed or not.
  
  if(sensorVal == LOW){
    //if the button is pressed,
    incident = 0;
    //reset the incident counter to 0.
    alpha4.clear();
    matrix.clear();
  }

  checker();
  //function to check to see if a day without incidents has passed.

  incidentWrite();
  //function to write the incident data to the alpha numeric display.

  timeWrite();
  //function to write the time to the seven segment display.
}

void checker(){
  //function to see if a day has passed, and if so to increase the days without incident variable.
  dt = clock.getDateTime();
  now = dt.day;
  //check what day it is and set it as now.
  
  if(today != now){
    //if today isn't now then a day has passed.
    incident ++;
    //add a day without incident.
    today = now;
    //today is now.
  }
}

void incidentWrite(){
  //functions to display the days without incident.
  //the alphanumeric display i used requires numbers to be input per digit.
  //hence the breaking up of the number then displaying it.
  //also it requires the number to be in ascii and not just a decimal,
  //so each number has to have 48 added to it because the first ascii number (1) is 48.
  int a = incident / 1000;
  int b = (incident - (a * 1000));
  b = b / 100;
  int c = (incident - ((a * 1000) + (b * 100)));
  c = c / 10;
  int d = (incident - ((a * 1000) + (b * 100) + (c * 10)));

  alpha4.writeDigitAscii(3, (d + 48));
  if(incident > 9){
    alpha4.writeDigitAscii(2, (c + 48));
    if(incident > 99){
      alpha4.writeDigitAscii(1, (b + 48));
      if(incident > 999){
        alpha4.writeDigitAscii(0, (a + 48));
        if(incident > 9999){
          //if we go over 9,999 days without incident display an error.
          alpha4.writeDigitAscii(0, 69);
          alpha4.writeDigitAscii(1, 114);
          alpha4.writeDigitAscii(2, 114);
          alpha4.writeDigitAscii(3, 45);
        }
      }
    }
  }
  alpha4.writeDisplay();
  //actually show what's been "written" to the numbers.
}

void timeWrite(){
  //show the time.
  int h = dt.hour;
  int m = dt.minute;

  if(h > 12){
    //since i'm a civilian, change 24 hour time to 12 hour time.
    h = h - 12;
  }

  if(h > 9){
    //if the 10's digit of the hour is there, show it.
    matrix.writeDigitNum(0, (h/10));
  }

  else{
    matrix.writeDigitRaw(0, 0);
    //need to "write" a blank value otherwise the 1 will remain in the hour spot when it shouldn't.
  }

  matrix.writeDigitNum(1, (h%10));
  matrix.writeDigitNum(3, (m/10));
  matrix.writeDigitNum(4, (m%10));
  //show the rest of the time.

  if(counter == 100){
    //counter stuff to make the colon blink.
    counter = 0;
    //reset the counter variable.
    if(colon == 0){
      colon = 1;
      //if it's off, turn it on.
    }

    else{
      colon = 0;
      //if it's on, turn it off.
    }
  }

  else{
    counter ++;
    //otherwise just increase the counter to "delay".
  }
  matrix.drawColon(colon);
  //set the colon to be on or off based on what it should be to blink slightly.
  matrix.writeDisplay();
  //actually show the time on the display.
}
