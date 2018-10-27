// weather station with clock
// http://nicuflorica.blogspot.ro/2015/07/afisaj-cu-cristale-lichide-ktm-s1201.html
// sketch by niq_ro from http://www.tehnic.go.ro
// http://arduinotehniq.blogspot.com
// http://nicuflorica.blogspot.ro/
// vers. 1m1b - 03.07.2015 - Craiova, Romania
// vers. clock, DS18B20 thermometer & voltmeter - 27/10/2018

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 10  // pin masura DS18B20
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
float temperatura;

#include <Wire.h>
#include "RTClib.h" // from https://github.com/adafruit/RTClib
//#include <Encoder.h> // from http://www.pjrc.com/teensy/td_libs_Encoder.html

RTC_DS1307 RTC; // Tells the RTC library that we're using a DS1307 RTC

#define meniu 9
#define plus 2
#define minus 3

//Encoder knob(plus,minus); //encoder connected to pins 2 and 3 (and ground)

//the variables provide the holding values for the set clock routine
int setyeartemp; 
int setmonthtemp;
int setdaytemp;
int sethourstemp;
int setminstemp;
int setsecs = 0;
int maxday; // maximum number of days in the given month
int TimeOut = 10;
int TimeOutCounter;

// These variables are for the push button routine
int buttonstate = 0; //flag to see if the button has been pressed, used internal on the subroutine only
int pushlengthset = 2000; // value for a long push in mS
int pushlength = pushlengthset; // set default pushlength
int pushstart = 0;// sets default push value for the button going low
int pushstop = 0;// sets the default value for when the button goes back high

int knobval; // value for the rotation of the knob
boolean buttonflag = false; // default value for the button flag

//based on 
/* Example 49.1
 Testing Arduino and KTM-S1201 LCD module
 Based on code by Jeff Albertson Ver 1.0
 Modifications and code normalisation by Robert W. Mech rob@mechsoftware.com
 http://tronixstuff.com/2013/03/11/arduino-and-ktm-s1201-lcd-modules/
 Modified by John Boxall 11/March/2013  */
  
/////////////////////////////////To use the LCD you need all the lines from here////////////////////////////////////////////////////////////////////////////
#include "KTMS1201.h" // necessary header file

// Constants for functions to ease use of display printing
static int anCount=39; // one less than actual since 0 based indexes.
static byte anConst[39]= {_A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M,_N,_O,_P,_Q,_R,_S,_T,_U,_V,_W,_X,_Y,_Z,_1,_2,_3,_4,_5,_6,_7,_8,_9,_0,_,_DASH,_DEGREE};
static byte anHumanConst[39]= {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6','7','8','9','0',' ','-','*'};

/////////////////////////////////... to here, and the five lines in void setup() ////////////////////////////////////////////////////////////////////////////

void ktmPrnNumb(int n, byte p,byte d,byte l)
/*
  ktmPrnNumb
  This function demonstrates how to output data and place decimal
  points in a specific place. 
*/
{
  // n is the number to be printed 
  // p is the position for lsb from right starting at 0 and 
  // d in the number of digits to the right of the decimal point
  // l is length number of digits in n
  int nt;
  int i;
  int j=1;
  int r=0;
  digitalWrite(LcdCnD, HIGH); //Put in command mode
  digitalWrite(LcdnCs, LOW); //Select LCD
  delay(1);
  ktmshiftOut(_Decode);
  ktmshiftOut(_LoadPtr+p*2);
  digitalWrite(LcdCnD, LOW);    //Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1) ;
  nt = n;
  for (i=0;i<l;i++)//display numbers (reverse order)
  {
    ktmshiftOut(nt%10);
    nt=nt/10;
  }
  j=p*2+d*2;
  if (d>0)  //   Set Decimal point
  {
    digitalWrite(LcdCnD, HIGH); //Put in command mode
    digitalWrite(LcdnCs, LOW); //Select LCD
    delay(1);
    ktmshiftOut(_NoDecode);
    ktmshiftOut(_LoadPtr+j);
    ktmshiftOut(_SetDp);
    delay(1);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);
}

void ktmInit()
/*
   ktmInit
   This function sets the display up for commands. 
*/
{
  pinMode(LcdSck,OUTPUT);
  pinMode(LcdSi,OUTPUT);
  pinMode(LcdCnD,OUTPUT);
  pinMode(LcdnRes,OUTPUT);
  pinMode(LcdnBus,INPUT);
  pinMode(LcdnCs,OUTPUT);
  // run these once at startup/reset to initialise the LCD module
  digitalWrite(LcdCnD, HIGH); // Put in copmmand mode
  digitalWrite(LcdnCs, HIGH); // deselect KTM-S1201
  digitalWrite(LcdSck, HIGH);
  digitalWrite(LcdSi,  HIGH);
  digitalWrite(LcdnRes, LOW); // reset lcd
  delay(10);
  digitalWrite(LcdnRes, HIGH);
  delay(10);
  // Setup command mode
  ktmCommand(_Mode);
  ktmCommand(_USync);
}

void ktmWriteString(String ktmMessage)
/*
    ktmWriteString(<Message>)
    This function will take a text string and convert it into a message for 
    the display.  You only need to send a message it will space out all other
    characters.
*/
{
   byte TextString[12]={_,_,_,_,_,_,_,_,_,_,_,_}; // Ultimate buffer of characters to write.
   String message=ktmMessage;
   message+="            "; // pad to 12
   message=message.substring(0,12); // Make sure we only have 12 characters
   message.toUpperCase();
   // Iterate through human readable array and stuff TextString with hex values. 
   for(int i=0;i<12;i++){ // Outer loop is the message String
     for(int j=0;j<anCount;j++){ // Inner loop is maximum values of hex arrays.
         if(message[i]==anHumanConst[j]){ // If we match the human readable character populate the text string with constant. 
           TextString[i]=anConst[j];
         }
     }
   }
  digitalWrite(LcdCnD, LOW);    //Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1);
  for (int i = 11; i >= 0; i --)//Put in KTM-S1201 Right Character First
  {
    ktmshiftOut(TextString[i]);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);                    //always delay after LcdnCs change
}

void ktmCommand(byte _cmd)
// necessary for other LCD functions
{
  digitalWrite(LcdCnD, HIGH);
  delay(1);
  digitalWrite(LcdnCs, LOW);
  delay(1);
  ktmshiftOut(_cmd);
  delay(1);
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);
}

void ktmshiftOut(byte val)
// necessary for other LCD functions
// same as shiftout command but invert Sck
{
  int i;
  for (i=0;i<8;i++)
  {
    digitalWrite(LcdSi,!!(val & (1<< (7-i))));
    digitalWrite(LcdSck,LOW);
    delay(1);
    digitalWrite(LcdSck,HIGH);
  }
  delay(1);
}

void ktmWriteHex(int hexVal,int pos)
/*
  ktmWriteHex
  Writes any value in any position. 
*/
{
   byte TextString[12]={_,_,_,_,_,_,_,_,_,_,_,_}; // Ultimate buffer of characters to write.
  // Iterate through human readable array and stuff TextString with hex values. 
  TextString[pos]=hexVal;
  digitalWrite(LcdCnD, LOW);    //Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1);
  for (int i = 11; i >= 0; i --)//Put in KTM-S1201 Right Character First
  {
    ktmshiftOut(TextString[i]);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);                    //always delay after LcdnCs change
}

#define pinmasura A2 // pin masura tensiune
float R2 = 15;  // rezistenta intre borna plus si intrarea analogica
float R1 = 4.7;  // rezistenta intre intrare analogica si masa (GND)
//  +12V---| 15k |---- Analog In ---| 4k7 |------| GND
int treapta = 0; // valoare prag tensiune
int trepte = 0; 
float tensiune = 0;  // tensiune masurata
float referinta = 5.;  // tensiune referinta




void setup()
{
  ktmInit(); // Runs inital reset and prepares the display for commands. 
  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
  ktmCommand(_ClearDsp); 

// Start up the library
sensors.begin();

    Wire.begin();
    RTC.begin();
  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 
// part code from http://tronixstuff.wordpress.com/
Wire.beginTransmission(0x68);
Wire.write(0x07); // move pointer to SQW address
//Wire.write(0x10); // sends 0x10 (hex) 00010000 (binary) to control register - turns on square wave
Wire.write(0x13); // sends 0x13 (hex) 00010011 (binary) to control register - turns on square wave at 32kHz
Wire.endTransmission();
// end part code from http://tronixstuff.wordpress.com/

pinMode(meniu,INPUT);//push button on encoder connected to D9 (and GND)
digitalWrite(meniu,HIGH); //Pull D9 high
pinMode(plus,INPUT);//push button on encoder connected to D2 (and GND)
digitalWrite(plus,HIGH); //Pull D2 high
pinMode(minus,INPUT);//push button on encoder connected to D3 (and GND)
digitalWrite(minus,HIGH); //Pull D3 high

Serial.begin(9600);
Serial.println("test for niq_ro");
Serial.println("------------------");
}

void loop()
{
sensors.requestTemperatures(); // Send the command to get temperatures
temperatura = sensors.getTempCByIndex(0);
int t2 = 10*temperatura; 

trepte = 0;
for(int j1=0; j1<10; j1++)
{
treapta = analogRead(pinmasura);  
trepte = trepte + treapta;
}
tensiune = (float)((R1+R2) * trepte * referinta / R1 / 1024.0);
Serial.print("Tensiune pe borna auxiliara = ");
Serial.print(tensiune/10);
Serial.println("V");

ktmCommand(_ClearDsp);
// temperatue and humidity on display
temphum (t2,tensiune);
for(int j=0; j<250; j++){
//temphum (t2+0.5,has);

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    if (pushlength <pushlengthset) {
      ShortPush ();   
    }
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength >pushlengthset) {
 //      lc.clearDisplay(0); // Clear display register 
         DateTime now = RTC.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setclock();
         pushlength = pushlengthset;
       };
}

DateTime now = RTC.now();
int ora0 = now.hour();
int minut0 = now.minute();
int second0 = now.second();

int ora7 = ora0;
int minut7 = minut0;
int second7 = second0;

//ktmCommand(_ClearDsp);
ora (ora0, minut0, second0);

//int ora7, minut7, second7;

for(int j=0; j<4; j++)
{
DateTime now = RTC.now();
int ora0 = now.hour();
int minut0 = now.minute();
int second0 = now.second();

//ktmCommand(_ClearDsp);
//ora (ora0, minut0, second0);

delay(500);

// serial monitor
  Serial.print(now.hour(), DEC);
  Serial.print(" = ");
  Serial.print(ora0);
  Serial.print(" = ");
  Serial.print(ora7);
  Serial.println(" ? ");
  
  Serial.print(now.minute(), DEC);
  Serial.print(" = ");
  Serial.print(minut0);
  Serial.print(" = ");
  Serial.print(minut7);
  Serial.println(" ? ");
  
  Serial.print(now.second(), DEC);
  Serial.print(" = ");
  Serial.print(second0);
  Serial.print(" = ");
  Serial.print(second7);
  Serial.println(" ? ");
  Serial.println(" ------------------- ");

if (ora7 != ora0) orah(now.hour());
if (minut7 != minut0) oram(now.minute());
if (second7 != second0) oras(now.second());


ora7 = ora0;
minut7 = minut0;
second7 = second0;
  Serial.print(ora7);
  Serial.print(":");
    Serial.print(minut7);
    Serial.print(":");
      Serial.println(second7);


    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    if (pushlength <pushlengthset) {
      ShortPush ();   
ora7 = 0;
minut7 = 0;
second7 = 0;
    }
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength >pushlengthset) {
    //     lc.clearDisplay(0); // Clear display register 
         DateTime now = RTC.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setclock();
         pushlength = pushlengthset;
       };
delay (100);
}
//delay(400);
}


void temphum (int temp, int umidit)
{
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp); 
//ktmWriteString("    *C   RH");
//ktmWriteString("    *C   *o");
ktmWriteString("    *C    U");
ktmPrnNumb(umidit,2,1,3); 
ktmPrnNumb(temp,8,1,3); 
}


void ora (int ora1, int minut1, int second1) 
{
//  ktmInit(); // Runs inital reset and prepares the display for commands. 
//  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
//  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
//  ktmCommand(_ClearDsp); 

ktmWriteString("    -  -");
//ktmPrnNumb(umidit,3,0,2); 
ktmPrnNumb(ora1,8,0,2); 
if (minut1 <10) ktmPrnNumb(0,6,0,1); 
ktmPrnNumb(minut1,5,0,2); 
if (second1 <10) ktmPrnNumb(0,3,0,1); 
ktmPrnNumb(second1,2,0,2); 

//delay(300);
}


void orah (int ora1) 
{
//  ktmInit(); // Runs inital reset and prepares the display for commands. 
//  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
//  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
//  ktmCommand(_ClearDsp); 

//ktmWriteString("    -  -");
//ktmPrnNumb(umidit,3,0,2); 
ktmPrnNumb(ora1,8,0,2); 
delay(300);
}


void oram (int minut1) 
{
//  ktmInit(); // Runs inital reset and prepares the display for commands. 
//  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
//  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
//  ktmCommand(_ClearDsp); 

//ktmWriteString("    -  -");
//if (minut1 <10) ktmPrnNumb(0,6,0,1); 
ktmPrnNumb(minut1,5,0,2); 
delay(300);
}


void oras (int second1) 
{
//  ktmInit(); // Runs inital reset and prepares the display for commands. 
//  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
//  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
//  ktmCommand(_ClearDsp); 

//ktmWriteString("    -  -");
//ktmPrnNumb(umidit,3,0,2); 
//if (second1 <10) ktmPrnNumb(0,3,0,1); 
ktmPrnNumb(second1,2,0,2); 
delay(300);
}




void data (int ziua1, int luna1, int anul1) 
{

//  ktmInit(); // Runs inital reset and prepares the display for commands. 
//  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
//  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
//  ktmCommand(_ClearDsp); 

ktmWriteString("   -  -");
//ktmPrnNumb(umidit,3,0,2); 
ktmPrnNumb(ziua1,9,0,2); 
if (luna1 <10) ktmPrnNumb(0,7,0,1); 
ktmPrnNumb(luna1,6,0,2); 
ktmPrnNumb(anul1,1,0,4); 


delay(500);
}


//sets the clock
void setclock (){
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp);
ktmWriteString("YEAR ");
   setyear ();
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp);
ktmWriteString("MONTH ");
   setmonth ();
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp);
ktmWriteString("DAY ");
   setday ();
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp);
ktmWriteString("HOURS ");
   sethours ();
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp);
ktmWriteString("MINUTES");
   setmins ();
ktmCommand(_NoDecode); 
ktmCommand(_ClearDsp);
   
   RTC.adjust(DateTime(setyeartemp,setmonthtemp,setdaytemp,sethourstemp,setminstemp,setsecs));

DateTime now = RTC.now();
int ora0 = now.hour();
int minut0 = now.minute();
int second0 = now.second();

//ktmCommand(_ClearDsp);
ora (ora0, minut0, second0);

   delay (500); 
}

// subroutine to return the length of the button push.
int getpushlength () {
  buttonstate = digitalRead(meniu);  
       if(buttonstate == LOW && buttonflag==false) {     
              pushstart = millis();
              buttonflag = true;
          };
          
       if (buttonstate == HIGH && buttonflag==true) {
         pushstop = millis ();
         pushlength = pushstop - pushstart;
         buttonflag = false;
       };
       return pushlength;
}

// The following subroutines set the individual clock parameters
int setyear () {
// ktmCommand(_NoDecode); 
// ktmCommand(_ClearDsp);

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setyeartemp;
    }

//    knob.write(0);
    delay (50);
//    knobval=knob.read();
    if (digitalRead(minus) == LOW) { //bit of software de-bounce
      knobval = -1;
      delay (50);
    }
    if (digitalRead(plus) == LOW) {
      knobval = 1;
      delay (50);
    }
    setyeartemp=setyeartemp + knobval;
    knobval = 0;
    if (setyeartemp < 2018) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2040;
    }
     if (setyeartemp > 2040) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2018;
    }
Serial.println(setyeartemp);

//ktmWriteString("YEAR ");
ktmPrnNumb(setyeartemp,3,0,4); 

    setyear();    
}

  
int setmonth () {
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setmonthtemp;
    }
//print2display("        ");
  //  knob.write(0);
    delay (50);
  //  knobval=knob.read();
    if (digitalRead(minus) == LOW) {
      knobval = -1;
      delay(50);
    }
    if (digitalRead(plus) == LOW) {
      knobval = 1;
      delay(50);
    }
    setmonthtemp=setmonthtemp + knobval;
    knobval = 0;
    if (setmonthtemp < 1) {// month must be between 1 and 12
      setmonthtemp = 12;
    }
    if (setmonthtemp > 12) {
      setmonthtemp=1;
    }
ktmPrnNumb(setmonthtemp,3,0,2); 
     setmonth();
}

int setday () {
  if (setmonthtemp == 4 || setmonthtemp == 5 || setmonthtemp == 9 || setmonthtemp == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (setmonthtemp ==2 && setyeartemp % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (setmonthtemp ==2 && setyeartemp % 4 !=0) {
    maxday = 28;
  }

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setdaytemp;
    }
//print2display("        ");
 //   knob.write(0);
    delay (50);
 //   knobval=knob.read();
    if (digitalRead(minus) == LOW) {
      knobval = -1;
      delay(50);
    }
    if (digitalRead(plus) == LOW) {
      knobval = 1;
      delay(50);
    }
    setdaytemp=setdaytemp+ knobval;
    knobval = 0;
    if (setdaytemp < 1) {
      setdaytemp = maxday;
    }
    if (setdaytemp > maxday) {
      setdaytemp = 1;
    }
 ktmPrnNumb(setdaytemp,3,0,2); 
     setday();
}

int sethours () {

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstemp;
    }
//print2display("        ");
 //   knob.write(0);
    delay (50);
//    knobval=knob.read();
    if (digitalRead(minus) == LOW) {
      knobval = -1;
      delay(50);
    }
    if (digitalRead(plus) == LOW) {
      knobval = 1;
      delay(50);
    }
    sethourstemp=sethourstemp + knobval;
    knobval = 0;
    if (sethourstemp < 1) {
      sethourstemp = 23;
    }
    if (sethourstemp > 23) {
      sethourstemp=1;
    }
ktmPrnNumb(sethourstemp,3,0,2); 
    sethours();
}

int setmins () {

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setminstemp;
    }
//print2display("        ");
 //   knob.write(0);
    delay (50);
//    knobval=knob.read();
    if (digitalRead(minus) == LOW) {
      knobval = -1;
      delay(50);
    }
    if (digitalRead(plus) == LOW) {
      knobval = 1;
      delay(50);
    }
    setminstemp=setminstemp + knobval;
    knobval = 0;
    if (setminstemp < 0) {
      setminstemp = 59;
    }
    if (setminstemp > 59) {
      setminstemp=0;
    }
ktmPrnNumb(setminstemp,1,0,2); 
    setmins();
}


void ShortPush () {
//for(int j=0; j<2 ; j++){
DateTime now = RTC.now();
int ziua0 = now.day();
int luna0 = now.month();
int anul0 = now.year();
/*
// serial monitor
  Serial.print(now.day(), DEC);
  Serial.print(":");
  Serial.print(now.month(), DEC);
  Serial.print(":");
  Serial.print(now.year(), DEC);
  Serial.print(" -> ");
  Serial.print(ziua0);
  Serial.println(".");
  Serial.print(luna0);
  Serial.print(".");
  Serial.print(anul0);
  Serial.println("------------------");
*/

data (ziua0, luna0, anul0);
/*
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    if (pushlength <pushlengthset) {
      ShortPush ();   
    }
*/
       //This runs the setclock routine if the knob is pushed for a long time
 delay (2000);
 ktmCommand(_ClearDsp);
// DateTime now = RTC.now();
int ora0 = now.hour();
int minut0 = now.minute();
int second0 = now.second();

ora (ora0, minut0, second0);

}

