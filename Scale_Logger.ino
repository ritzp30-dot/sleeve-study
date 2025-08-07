This contains the algorithm to measure the water consumption in  the Arduino logger scale


/**
 *
 * Scale Logger 
 * MIT License
 * (c) Nathaniel Hall
 *
**/

//#include <RHReliableDatagram.h>
//#include <RH_RF95.h>
#include "HX711.h"
#include <SPI.h>
#include <SD.h>
//#include <Adafruit_SleepyDog.h>
#include "RTClib.h"
#define VBATPIN A9


//Initiate the RTC
RTC_PCF8523 rtc;

//Indicate the SD card chipselect pin
const int chipSelect = 10;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 11;
//Initiate Hx711 scale
HX711 scale;

void setup() {
 
 //Wait for Serial- Remove after debugging 
 Serial.begin(9600);
 delay(2000);

 //Initialize RTC 
 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

if (! rtc.initialized()) {
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));}
rtc.start();

//Begin Scale and Tare the weight
 scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
 scale.set_scale(212.77); // this value is obtained by calibrating the scale with known weights; see the README for details
 scale.tare();
 
 Serial.print("Initializing SD card...");
// see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }

  Serial.println("card initialized.");
}


void loop() {

  String dataString = "";

  //Grab the time 
  DateTime time = rtc.now();
  //Full Timestamp
  String weightTime= time.timestamp(DateTime::TIMESTAMP_FULL);
  
  float weight= scale.get_units(10);
  

  dataString= weightTime + "," + String(weight);

  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.print("Saved the following: ");
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.csv");
  }


float measuredvbat = analogRead(VBATPIN);
measuredvbat *= 2;    // we divided by 2, so multiply back
measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
measuredvbat /= 1024; // convert to voltage
Serial.print("VBat: " ); Serial.println(measuredvbat);

scale.power_down();			        // put the ADC in sleep mode
delay(4000);
digitalWrite(LED_BUILTIN, LOW); // Show we're asleep
  //int sleepMS = Watchdog.sleep(4000);
scale.power_up();
  //digitalWrite(LED_BUILTIN, HIGH); // Show we're awake again

}




