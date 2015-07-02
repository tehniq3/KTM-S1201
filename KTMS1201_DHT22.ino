// adapted sketch by niq_ro from http://www.tehnic.go.ro
// 02.07.2015 - Craiova, Romania
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

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include "DHT.h"
#define DHTPIN A2     // what pin we're connected to A2
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// if is just sensor:
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);


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
  digitalWrite(LcdCnD, LOW);  	//Put in data mode
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
  digitalWrite(LcdCnD, LOW);  	//Put in data mode
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
  digitalWrite(LcdCnD, LOW);  	//Put in data mode
  digitalWrite(LcdnCs, LOW); 
  delay(1);
  for (int i = 11; i >= 0; i --)//Put in KTM-S1201 Right Character First
  {
    ktmshiftOut(TextString[i]);
  }
  digitalWrite(LcdnCs, HIGH); //deselect LCD to display data
  delay(1);                    //always delay after LcdnCs change
}



void setup()
{
  ktmInit(); // Runs inital reset and prepares the display for commands. 
  ktmCommand(_NoBlink);  // Optional _SBlink to blink display. 
  ktmCommand(_DispOn);   // Turn on display
  ktmCommand(_NoDecode); 
  ktmCommand(_ClearDsp); 

 // Initialize DHT sensor
  dht.begin();

}

void loop()
{
//  ktmWriteString("- Hello -");
//ktmCommand(_ClearDsp);
//ktmPrnNumb(12345,0,4,5);

// Reading temperature or humidity 
 int has = dht.readHumidity();
 float te = dht.readTemperature(); 
// int te = dht.readTemperature(); 
 delay(250);
 int t2 = 10*te; 

ktmWriteString("    *C   RH");
ktmPrnNumb(has,3,0,2); 
ktmPrnNumb(t2,8,1,3); 

delay(3000);
}
