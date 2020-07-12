
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //this i meant for AVR I guess but everyone seems to use the same library on esp32 so not worried
#include <SoftwareSerial.h> //need to switch to hardware serial just installed extra library so code would build
/*
 * BT_RFID_V1.ino 
 * 11/27/2018
 * By JaydenN117
 * 
 * BT_RFID_V3.ino 
 * Last Updated
 * 4/1/2020 *
 * 
 * Bluetooth RFID reader for cattle tags
 * for rfidRW-E-TTL. It uses an arduino nano as
 * well as a 4 pin lcd and HC-05
 * 
 * Important Links
 * =============================================
 * About rfidRW-E-TTL module
 * http://www.priority1design.com.au/rfidrw-e-ttl.pdf
 * 
 * RFID code is inspired by code here
 * https://forum.arduino.cc/index.php?topic=253703.0
 *
 * Code for reading voltaged inspired by
 * https://startingelectronics.org/articles/arduino/measuring-voltage-with-arduino/
 * 
 * Bluetooth tools
 * http://www.dsdtech-global.com/2017/11/dsd-tech-bluetooth-config-tool.html
 * 
 *
 */
/*
 * Set pins below
 *
 *
 *
 * TOOL FOR GETTING I2C ADDRESS for LCD
 * https://playground.arduino.cc/Main/sourceblock_1/index.txt?action=sourceblock&num=1
 */
SoftwareSerial Rfid(3,4); //rfidRW-E-TTL software serial
LiquidCrystal_I2C lcd(0x27 ,16,2);//lcd use tool to get address replace 0x3F with address for your lcd if different
int buzzer = 5; //buzzer pin
int blueLED = 8; //bluetooth led light
int greenLED = 7; // ready led light
int redLED = 6; // scan led light
int btState = 9; //bluetooth state pin //need to remove from code
int vibrate = A1; //vibration motor pin 
int battery = A0; //use to detect voltage

/*
 * Other things to change based on wand
 * time,name, and wands bluetooth pin
 * if not configure it is HC_05 with 1234 as pin
 */
String wandNumber = "3";//wand number in rare case of two wands so theres different name
String btName = "BT_RFID_" + wandNumber; //name displayed on lcd wand number follows
String btPin = ""; //pin displayed on lcd
//custom boot lines
String line1 = "BT RFID V3";
String line2 = "";
//Some other things you can configure if necessary
boolean lcdSleepEnabled = true; //set false to keep lcd on
boolean indicators = true; //show bluetooth and battery on screen(really buggy voltage is all over with RFID module and bluetooth is not working quite right
boolean displayVoltage = false; //ahows voltage on lcd 
boolean quickBoot = false; //skips fancy startup
boolean bluetoothState = true; // Wand supports bluetooth state
int readings =100; //how many times read to average voltage 1000 more stable 10 updates very frequently
long sleepTime = 30000; // sleep time variable
//other variables controlled by code
boolean clearLCD = true; //lcd clear control during new scan
int state = 0; //bluetooth state value
long previousMillis; //part of sleep timer
char c;
float voltage; //battery voltage 
float voltageTotal; // voltage total used in averaging
int voltageReadings = 0; //how times voltage was read
float voltageAverage = -1; //average voltage starts at so program will know to display "?"
void setup() {
  
  start(); //start code
}

void loop() {
  if (bluetoothState){
    state = digitalRead(btState); //reads state pin
    digitalWrite(blueLED, state); //writes state to LED
  }
  voltage = ((analogRead(battery)*5.0)/1024)*2.0; //voltage
  voltageTotal+=voltage; //
  voltageReadings++;
  scan(); //run scan code
  if (sleepTimer() && lcdSleepEnabled){ //checks if time is over 30 and lcd sleep is enabled
    sleeplcd(); //if true sleep lcd
  }
  else{
    //no need to write indicators on lcd without backlight on
    if (indicators && clearLCD){ //if it is Ok to clearLCD we also know it is not currently printing so I check clearLCD
       batteryAndBluetooth(); //show bluetooth and battery information
    }
    if (displayVoltage && clearLCD){ //shows voltage
      showVoltage();
    }
  }
  if (voltageReadings>=readings){
    voltageAverage = voltageTotal/voltageReadings;
    voltageReadings = 0;
    voltageTotal = 0;
  }
}
