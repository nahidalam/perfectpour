// sketch to read x, y, & z values of accelerometer

#include <math.h>   
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "Timer.h"

int xMid, yMid, zMid, xRng, yRng, zRng;
int xr = 0, yr = 0, zr = 0;    
float x = 0;
float y = 0;
float z = 0;
float tiltx, tilty, tiltz;
int redLED = 8;
String state = "initial";
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int start = A5;
String goodPour = "yes";
String pressed = "false";
Timer t;


const int X_Pin = 1;  // arduino pin for x 
const int Y_Pin = 2;  // arduino pin for y 
const int Z_Pin = 3;  // arduino pin for z 

// x 493 to 161  midpoint (hi+low)/2 = 327  mid - low =  166
// y 518 to 184  midpoint (hi+low)/2 = 351  mid - low =  167
// z 473 to  147 midpoint (hi+low)/2 = 310  mid - low =  163
// X 159 ,501	Y  182 ,521	Z 142 ,485

void setup()
{    
    digitalWrite(start, HIGH);
    Serial.begin(9600);    // open serial port to laptop to print values

    //calibrate_accelerometer();  // uncomment to calibrate, comment to run

    // read calibration values from EEprom - won't work until calibrated
    xMid = EEPROM.read(1000) << 8; 
    xMid = xMid | EEPROM.read(1001);
    xRng = EEPROM.read(1002) << 8;
    xRng = xRng | EEPROM.read(1003);
    yMid = EEPROM.read(1004) << 8; 
    yMid = yMid | EEPROM.read(1005);
    yRng = EEPROM.read(1006) << 8;
    yRng = yRng | EEPROM.read(1007);
    zMid = EEPROM.read(1008) << 8; 
    zMid = zMid | EEPROM.read(1009);
    zRng = EEPROM.read(1010) << 8;
    zRng = zRng | EEPROM.read(1011);

    // print calibration values. x, y, z pairs
    // they should look like ~300 ~160  ~300 ~160   ~300 ~160   
    
    Serial.println("Calibratrion constants: xMid, xRng, yMid, yRng, zMid, zRng");
    Serial.print(xMid);
    Serial.print("\t"); 
    Serial.print(xRng); 
    Serial.print("\t\t");
    Serial.print(yMid);
    Serial.print("\t"); 
    Serial.print(yRng); 
    Serial.print("\t\t");
    Serial.print(zMid); 
    Serial.print("\t"); 
    Serial.println(zRng);
    
    // set up the LCD's number of columns and rows: 
     lcd.begin(16, 2);
      int after6 = t.after(6500, switchTo90);
      int after15 = t.after(15000, finish);
    delay(2000);

}

void switchTo90() {
   digitalWrite(redLED, HIGH);
}

void finish() {
  state = "finish";
  lcd.clear();
if(goodPour == "yes") lcd.print("good pour!"); 
}

void loop() 
{   
    t.update();
int startReading = analogRead(start);
Serial.println(startReading);
delay(250);
    readAccelerometer();
    Serial.print(tiltx); // use "tiltx", "tilty" etc in your program
    Serial.print("\t");  // "\t" is tab character
    Serial.print(tilty); 
    Serial.print("\t"); 
    Serial.println(tiltz);  
    
   if((startReading > 220)  || (startReading < 200)) {
    pressed = "true"; 
         lcd.clear();
     
     lcd.print("tilt glass");
      lcd.setCursor(0, 1);
      lcd.print("45 degrees");
   }
   
   if ((pressed == "true")) {
isInRange(tiltx);
   }
   

    delay(50);           // delay for serial monitor only
}

void  readAccelerometer(){
    xr = analogRead(X_Pin);      // read analog pin for x raw
    //     Serial.print(xr );       
    //     Serial.print("  ");
    yr = analogRead(Y_Pin);      // read analog pin for y raw
    //     Serial.print(yr ); 
    //     Serial.print(" ");     
    zr = analogRead(Z_Pin);      // read analog pin for z raw   
    //     Serial.println(zr );              
    
    x = (xr - xMid) / (float)xRng;
    //    Serial.print("   "); 
    y = (yr - yMid) / (float)yRng;
    //    Serial.print("         "); 
    z = (zr - zMid) / (float)zRng;

    tiltx = atan2( sqrt((y*y) + (z*z)), x);
    tilty = atan2( sqrt((x*x) + (z*z)), y);
    tiltz = atan2( sqrt((y*y) + (x*x)), z);
}

boolean isInRange(double x) {
 if(state == "above") { // red is high, yellow is low
  if(tiltx < 1) {
    state = "below";
    lcd.clear();
    lcd.print("too slow");
    goodPour = "no";
  }
 else if((tiltx > 1) && (tiltx < 2)) {
 state = "correct";
 lcd.clear();
 lcd.print("in the zone");
 }
 lcd.clear();
 lcd.print("too fast");
 }
 if(state == "below") { // red is low, yellow is high
  if(tiltx > 2) {
        state = "above";
          lcd.clear();
        lcd.print("too fast");
        goodPour = "no";
        
  }
else if((tiltx > 1) && (tiltx < 2)) {
state = "correct";
 lcd.clear();
 lcd.print("in the zone");
} 
lcd.clear();
lcd.print("too slow");
 }
 if(state == "correct") { // red is low, yellow is low
 if(tiltx > 2) {
 state = "above";
 goodPour = "no";
 }
 if(tiltx < 1){
  state = "below";
  goodPour = "no";
 }
  lcd.clear();
 lcd.print("in the zone");
 }

if(state == "initial") {
  if(tiltx > 2) {
 state = "above";
 goodPour = "no";
 }
 if(tiltx < 1){
  state = "below";
  goodPour = "no";
 } 
}
  
}

void calibrate_accelerometer(){
    int maxX = 0, maxY = 0, maxZ = 0, minX = 32000, minY = 32000, minZ = 32000;

    long lastFound;
    Serial.println("Calibrating Your Accelerometer");
    Serial.println("Gently move the accelerometer to the extreme positions of the");
    Serial.println("x, y & z axis. You will need to flip the sensor over ");
    Serial.println("for the z axis extreme.");
    Serial.println();
    Serial.println("Gently tilt the sensor around at the axes extremes.");
    Serial.println("Look for minimum values around 160 and maxiumum values around 500,");
    Serial.println("for each axis.");
    Serial.println(" ");
    Serial.println("The calibration routine will end when a new min value or max value");
    Serial.println("hasn't been found in 30 seconds, so just gently hold the board motionless"); 
    Serial.println("when you feel that min. values and max. values have been found");
    Serial.println("for each axis.");
    Serial.println(" ");
    Serial.println("Calibration routine will start in 10 seconds.");
    Serial.println(" ");
    Serial.println("Move as gently as possible please.");
    delay(10000);
    Serial.println("Starting calibration");
    lastFound = millis();
    while(millis() - lastFound < 20000){
        xr = analogRead(X_Pin);            // Read analog Pins and update max and min
        if (xr > maxX){
            maxX = xr;
            lastFound = millis();
        }
        if (xr < minX){
            minX = xr;
            lastFound = millis();
        }

        yr = analogRead(Y_Pin); 
        if (yr > maxY){
            maxY = yr;
            lastFound = millis();
        }
        if (yr < minY){
            minY = yr;
            lastFound = millis();
        }

        zr = analogRead(Z_Pin);  
        if (zr > maxZ){
            maxZ = zr;
            lastFound = millis();
        }
        if (zr < minZ){
            minZ = zr;
            lastFound = millis();
        }

        Serial.print("X "); 
        Serial.print(minX);
        Serial.print(" ,"); 
        Serial.print(maxX); 
        Serial.print("\t"); 
        Serial.print("Y ");
        Serial.print(minY); 
        Serial.print(" ,"); 
        Serial.print(maxY); 
        Serial.print("\t");       
        Serial.print("Z "); 
        Serial.print(minZ); 
        Serial.print(" ,"); 
        Serial.println(maxZ);  
        delay(100);
    }
    Serial.println("Storing Calibration in EEPROM");

    int xMid = (minX + maxX) / 2;   // calculate midpoints and range
    int xRng = xMid - minX;         // from axis extremes   

    int yMid = (minY + maxY) / 2;
    int yRng = yMid - minY;

    int zMid = (minZ + maxZ) / 2;
    int zRng = zMid - minZ;

    EEPROM.write(1000, highByte(xMid)); 
    EEPROM.write(1001, lowByte(xMid));
    EEPROM.write(1002, highByte(xRng)); 
    EEPROM.write(1003, lowByte(xRng));     
    EEPROM.write(1004, highByte(yMid)); 
    EEPROM.write(1005, lowByte(yMid));
    EEPROM.write(1006, highByte(yRng)); 
    EEPROM.write(1007, lowByte(yRng));
    EEPROM.write(1008, highByte(zMid)); 
    EEPROM.write(1009, lowByte(zMid));
    EEPROM.write(1010, highByte(zRng)); 
    EEPROM.write(1011, lowByte(zRng));

    Serial.println("End Calibration.");
    delay(2000);
}





