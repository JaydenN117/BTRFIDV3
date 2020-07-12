//holds commands

/*
 * Arrays for bluetooth logos first one is little logo
 * other 4 part of large logo
 */

byte bluetoothLogo[8] ={
  B00100,
  B10110,
  B01101,
  B00110,
  B01101,
  B10110,
  B00100,
};

byte bigLogo1[8] = {//top left
  B00001,
  B00001,
  B00001,
  B00001,
  B00101,
  B00011,
  B00001,
};
byte bigLogo2[8] = {//top right
  B00000,
  B00000,
  B10000,
  B01000,
  B00100,
  B01000,
  B10000,

}; 
byte bigLogo3[8] ={//bottom left
  B00011,
  B00101,
  B00001,
  B00001,
  B00001,
  B00001,
  B00000,
}; 
byte bigLogo4[8] = { //bottom right
  B01000,
  B00100,
  B01000,
  B10000,
  B00000,
  B00000,
  B00000,
};

byte batteyFull[8] = { //indicates full battery
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte batteyGood[8] = { //indicates fair battery
  B01110,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte batteyLow[8] = { //indicates low battery
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
};
byte batteyEmpty[8] = { //indicates empty battery
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
};
void start() {
  
  pinMode(buzzer,OUTPUT); //set buzzer to output
  //set LEDS to outputs
  pinMode(redLED,OUTPUT);
  pinMode(blueLED,OUTPUT);
  pinMode(greenLED,OUTPUT);

  pinMode(btState,INPUT); //bluetooth state pin as input
  
  pinMode(vibrate,OUTPUT); //vibration motor
  pinMode(battery,INPUT);
  Rfid.begin(9600);
  Serial.begin(9600);
  lcd.init();
  lcd.setBacklight((uint8_t)1);
  //all lights on
  if (!quickBoot){
    digitalWrite(blueLED,HIGH);
    digitalWrite(greenLED,HIGH);
    digitalWrite(redLED,HIGH);
    
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    delay(1000);
    
    //all lights off
    digitalWrite(blueLED,LOW);
    digitalWrite(greenLED,LOW);
    digitalWrite(redLED,LOW);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(btName);
    lcd.setCursor(0,1);
    lcd.print("passkey " + btPin); //whatever passkey you set bluetooth module to
    bigBtLogo(); //print big bluetooth logo on right side of lcd
    
    digitalWrite(blueLED,HIGH);
    tone(buzzer,500); delay(500); noTone(buzzer);
    digitalWrite(blueLED,LOW);
    digitalWrite(greenLED,HIGH);
    tone(buzzer,500); delay(500); noTone(buzzer);
    digitalWrite(greenLED,LOW);
    digitalWrite(redLED,HIGH);
    digitalWrite(vibrate,HIGH);
    tone(buzzer,500); delay(500); noTone(buzzer);
    digitalWrite(redLED,LOW);
    tone(buzzer,1500); delay(200); noTone(buzzer);
    digitalWrite(vibrate,LOW);
  }
  digitalWrite(greenLED,HIGH);
  reset();
}
void btLogo() {
  lcd.createChar(0,bluetoothLogo);
  lcd.setCursor(14,0);
  lcd.write(byte(0));
}
void bigBtLogo() {
  lcd.createChar(1,bigLogo1); //create lcd character for top left
  lcd.createChar(2,bigLogo2); //create lcd character for top right
  lcd.createChar(3,bigLogo3); //create lcd character for bottom left
  lcd.createChar(4,bigLogo4); //create lcd character for bottom right
  
  lcd.setCursor(14,0);
  lcd.write(byte(1)); //print top left
  lcd.write(byte(2)); //print top right
  lcd.setCursor(14,1);
  lcd.write(byte(3)); //print bottom left
  lcd.write(byte(4)); //print bottom right
}
void scan(){
  if (Rfid.available()>0){
    digitalWrite(greenLED,LOW); //green led off
    digitalWrite(redLED,HIGH); // red led on
    if (clearLCD == true){
      reset();
      clearLCD = false; //dont clear again
      lcd.setCursor(0,1);
    }
    c = Rfid.read();
    if (c == '_') return; //ignore underscore
    
    Serial.write(c); //write c to Serial(Bluetooth);
    
    if (c == '\r'){  //if carriage return  
      Serial.write('\n'); //new line
      clearLCD = true;//clear for next reading
        
      //buzzer to alert user of scan
      digitalWrite(vibrate,HIGH); //start vibration motor
      tone(buzzer,1000); delay(500); noTone(buzzer);
      tone(buzzer,200); delay(200); noTone(buzzer);  
      digitalWrite(vibrate,LOW); //stop vibration motor 
      digitalWrite(greenLED,HIGH); //green led on
      digitalWrite(redLED,LOW); //red led off
    }else{
       lcd.write(c); // write c to lcd dont need extra stuff
    }
}

}
/*
 * Resets timer as well as clears screen for next 
 * numbers
 */
void reset(){
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  lcd.setBacklight((uint8_t)1); //turn on backlight
  lcd.clear(); //clear lcd
  lcd.setCursor(0,0); //cursor top left
  lcd.print("RFID #"); //print RFID #
  if (indicators){
    batteryAndBluetooth();
  }
  if (displayVoltage){
    showVoltage();
  }
  lcd.setCursor(0,1); //cursor bottom left
  
}
/*
 * Clears and turns off backlight on lcd
 */
void sleeplcd(){
  //lcd.clear(); //clear lcd
  lcd.setBacklight((uint8_t)0); // turn backlight off
}
/*
 * Check if time since last scan is over sleeptime 
 * if it is return true
 */
boolean sleepTimer(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > sleepTime) {
    return true;
  }
  return false;
}

void batteryAndBluetooth(){
  
  //bluetooth symbol
  if (state == HIGH){
    btLogo();
  }else if (state == LOW){
    lcd.setCursor(14,0);
    lcd.print(" ");
  }
  //these voltages work well with poover usb rechargable 9v batteries but not very good with a standard 9v
  if(voltageAverage>7.3){
    lcd.createChar(5,batteyFull);
    lcd.setCursor(15,0);
    lcd.write(byte(5)); //print at top right
    return;
  } 
  if(voltageAverage>7.0){
    lcd.createChar(5,batteyGood);
    lcd.setCursor(15,0);
    lcd.write(byte(5)); //print at top right
    return;
  }
  if(voltageAverage>6.5){
    lcd.createChar(5,batteyLow);
    lcd.setCursor(15,0);
    lcd.write(byte(5)); //print at top right
    return;
  }
  if (voltageAverage < 0){
    lcd.setCursor(15,0);
    lcd.print("?");
    return;
  }
  else{
    //empty battery  
    lcd.createChar(5,batteyEmpty);
    lcd.setCursor(15,0);
    lcd.write(byte(5)); //print at top right
  }
}
void showVoltage(){
  //simple command to print voltage on lcd
  if(indicators){
    lcd.setCursor(9,0);//go before bluetooth and battery levels
  }
  else{
    lcd.setCursor(12,0);//go to edge because theres nothing in way
  }
  if (voltageAverage > 0) {
    lcd.print(voltageAverage,1); //print voltage formated like 8.1
    lcd.print("v"); //add v 8.1v
    lcd.setCursor(0,0);
  }
}
/*
 * So I am super new to the esp32 looks like tone doesnt work
 * so I am making my own
 */
