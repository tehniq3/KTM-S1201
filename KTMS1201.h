/*  Header file for Arduino and KTM-S1201 LCD module
    Originally Written by Jeff Albertson Ver 1.0
    Modifications and code normalisation by Robert W. Mech rob@mechsoftware.com
    Modified by John Boxall 11/March/2013
        
    These are the pins that are output on the KTM-S1201

    LCD pin number
    Pin 1  LcdVcc    +5V dc power
    pin 2  LcdVss    GND
    pin 3  LcdnSck   Not Serial clock
    pin 4  LcdSi     Serial data
    pin 5  LcdCnD    Select Command or Data mode 
    pin 6  LcdnRes   Reset LCD    
    pin 7  LcdnBus   LCD not busy
    pin 8  LcdnCs    LCD not Chip select
    pin 9  Pot wiper  Vlc LCD Contrast Connect POT between pin 9 and GND
    pin 10 NC
*/

// Here we set which LCD pins will connect to which Arduino pin
#define LcdSck  4 //Not Serial clock
#define LcdSi   5 //Serial data in
#define LcdCnD  6 //Command or Not data in
#define LcdnRes 7 //Not reset low = reset
#define LcdnBus 9// Not Busy output from LCD not used if delay(1) after LcdnCs changes
#define LcdnCs  8 //Not Chip select Low = LCD selected


//  Put a delay(1) after every LcdnCs change or check LcdnBus
#define t 100
#define t1 10

// The Not Chip select pin is the only unique
// Pin on the LCD all other pins can be paralleled
// for more ktm-s1201 displays
#define _Mode 0x40
#define _USync 0x31
#define _FBlink 0x1b
#define _SBlink 0x1a
#define _NoBlink 0x18
#define _DispOn 0x11
#define _Decode 0x15
#define _NoDecode 0x14
#define _LoadPtr 0xE0
#define _SetDp 0xb8   // Set decimal point (Dp)
#define _ClearDp 0x9f // Clear decimal point (Dp)
#define _ClearDsp 0x20 // Clear display memory

// To set Dp you must be in NoDecode mode and point to character (LoadPtr)
//      ---   0x01
//0x10 |   |  0x02
//0x20  ---  
//0x40 |   |  0x04
//0x80  --- o 0x08 

#define _cet 0x31 // Celsius 
#define _ 0x0     // Space
#define _A  0x77  //
#define _B  0xF4  //
//#define _C  0xE0  //      changed by niq_ro
#define _C  0xD1  //
#define _D  0xE6  //
#define _E  0xF1  //
#define _F  0x71  //
#define _G  0xD5  //
#define _H  0x74  //
#define _I  0x04  //
#define _J  0x86  //
#define _K  0x70  //
#define _L  0xD0  //
#define _M  0x76  //
#define _N  0x64  //
#define _O  0xE4  //
#define _P  0x73  //
#define _Q  0xDF  //
#define _R  0x60  //
#define _S  0xB5  //
#define _T  0xF0  //
#define _U  0xC4  //
#define _V  0xD6  //
#define _W  0xF6  //
#define _X  0x56  //
//#define _Y  0x72  //
#define _Y  0xB6  //     changed by niq_ro
#define _Z  0xEB  //
#define _1  0x06  //
#define _2  0xE3  //
#define _3  0xA7  //
#define _4  0x36  //
#define _5  0xB5  //
#define _6  0xF4  //
#define _7  0x07  //
#define _8  0xF7  //
#define _9  0x37  //
#define _0  0xD7  //
#define _DASH  0x20  //
#define _DEGREE  0x33  //

