/* After receiving call after call from my kids' daycare, decided to use
 * spare parts to make an automatic "Days since last incident" counter.
 * Since it's got a built in Real Time Clock, why not add a clock to it?
 *   
 * new stuff!
 * got a metro mini, 
 * https://www.adafruit.com/product/2590
 * two white seven segment displays,
 * https://www.adafruit.com/product/1002
 * more colorful buttons,
 * https://www.adafruit.com/product/1009
 * and another rtc
 * https://www.adafruit.com/product/3296
 * 
 * All the parts are chained with the data pins all tied to each other 
 * then tied to A4.  The clock pins are tied together then connected to A5.
 * New buttons are tied to,
 * Red 2
 * Gray 3
 * Green 4
 *   
 */

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "RTClib.h"
//library for the Real Time Clock.

RTC_DS1307 rtc;
//probably create the rtc object to do stuff with below.
Adafruit_7segment matrix1 = Adafruit_7segment();
//create an object for the first seven segment display.
Adafruit_7segment matrix2 = Adafruit_7segment();
//create an object for the second seven segment display.

#define red 2
#define green 3
#define gray 4
//set the pins for the buttons.

int date = 0;
//variable for the date to see if it's today or yesterday.
int incident = 0;
//variable to keep track of how many days since things last went wrong.
int counter = 0;
//counter to make colon blink.
int hourAdjust = 0;
//variable to adjust hour for daylight savings and such.
int minuteAdjust = 0;
//variable to adjust minutes if needed.
bool colon = 0;
//flag for the colon to blink with.


void setup() {
  Serial.begin(9600);
  Serial.println("starting");
  //debugging for moral support.

  rtc.begin();
  //startup the rtc.

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  matrix1.begin(0x70);
  matrix1.setBrightness(0);
  //startup the first seven segment display.
  matrix2.begin(0x71);
  matrix2.setBrightness(0);
  //startup the second seven segment display.

  pinMode(red, INPUT_PULLUP);
  pinMode(green, INPUT_PULLUP);
  pinMode(gray, INPUT_PULLUP);
  //set the pins for the buttons to be inputs and use the internal 
  //pullup resistor instead of having to setup one myself.
}

void loop() {
  int readRed = digitalRead(red);
  int readGreen = digitalRead(green);
  int readGray = digitalRead(gray);
  //create variables to check on the buttons.

  if(readRed == LOW){
    //if the red button is pressed.
    incident = 0;
    delay(500);
  }

  if(readGreen == LOW){
    //if the green button is pressed.
    if(hourAdjust > 23){
      hourAdjust = 0;
    }

    else{
      hourAdjust ++;
      //add one to the hourAdjust value.
    }
   
    delay(500);
  }

  if(readGray == LOW){
    //if the gray button is pressed.
    if(minuteAdjust > 59){
      minuteAdjust = 0;
    }

    else{
      minuteAdjust ++;
      //add one to the minuteAdjust value.
    }
    Serial.println(minuteAdjust);
    delay(500);
  }

  checker();
  incidentDisplay();
  timeDisplay();
}

void checker(){
  DateTime now = rtc.now();
  //set the time to now.
  int today = now.day();
  //set the variable for today to today's date.

  if(today != date){
    //check to see if today's date matches the date on record.
    //if it's different, add a day to the incident counter, then 
    //set the day to the new one.

    incident ++;
    //increase the incident counter.
    date = today;
    //set the date to today.
  }
}

void incidentDisplay(){
  //display the days since last incident number.
  matrix1.print(incident);
  matrix1.writeDisplay();
  //push the incident number to the display then turn it on.
}

void timeDisplay(){
  DateTime now = rtc.now();
  int hourNow = now.hour();
  hourNow = hourNow + hourAdjust;
  //set the hour then adjust it if needed from the button press adjustment.
  if(hourNow > 36){
    hourNow = hourNow - 36;
  }

  else if(hourNow > 24){
    hourNow = hourNow - 24;
  }

  else if(hourNow > 12){
    hourNow = hourNow - 12;
  }
  
  

  
  int minutesNow = now.minute() + minuteAdjust;
  if(minutesNow > 119){
    minutesNow = minutesNow - 120;
  }

  else if(minutesNow > 59){
    minutesNow = minutesNow - 60;
  }

  if(counter == 100){
    //counter to blink colon on and off for seconds, kinda.
    counter = 0;
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
    //if it's not time to turn on or off, just increase the number.
  }

  int nowTime = ((hourNow * 100) + minutesNow);
  //get the time and put it in a variable.
  //cheating by multiplying the hour by 100 so that the minutes
  //can sneak in the display.
  
  matrix2.print(nowTime);
  matrix2.drawColon(colon);
  matrix2.writeDisplay();
  //push the time to the display and then turn it on.
}
