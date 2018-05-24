#include <GE7000_Interface.h>
#include <GE7000_Parallel.h>
#include <GE7000_Serial_Async.h>
#include <GE7000_Serial_Sync.h>
#include <GE7000_Serial_SPI.h>
#include <Noritake_VFD_GE7000.h>

// ****************************************************
// ****************************************************
// Uncomment one of the communication interfaces below.
//
//GE7000_Serial_Async interface(38400,3, 5, 7); // BAUD RATE,SIN,BUSY,RESET
//GE7000_Serial_Sync interface(3, 5, 6, 7); // SIN,BUSY,SCK,RESET
//GE7000_Serial_SPI interface(3, 5, 6, 7, 8); // SIN,BUSY,SCK,RESET,CS
//
//Parallel interface only works on GE256X64B-7032B module
//GE7000_Parallel interface('R', 8,9,10,11, 0,1,2,3,4,5,6,7); // Module Pin#3=RESET; BUSY,RESET,WR,RD,D0-D7
//GE7000_Parallel interface('B', 8,9,10,11, 0,1,2,3,4,5,6,7); // Module Pin#3=BUSY; BUSY,RESET,WR,RD,D0-D7
//GE7000_Parallel interface('N', 8,9,10,11, 0,1,2,3,4,5,6,7); // Module Pin#3=nothing; BUSY,RESET,WR,RD,D0-D7
//
// ****************************************************
// ****************************************************

Noritake_VFD_GE7000 vfd;

void setup() {
  _delay_ms(500);           // wait for device to power up
  vfd.begin(256, 64);       // 256x64 module
  // Enter the 4-digit model class number
  // E.g. 7040 for GU140X16G-7040A
  vfd.interface(interface); // select which interface to use
  vfd.isModelClass(7933);
  vfd.GE7000_reset();       // reset module
  vfd.GE7000_init();        // initialize module
  
  // Demonstrates the difference between fixed
  // and proportional fonts.
  vfd.GE7000_setFontStyle(false, false);
  vfd.println("fixed: lorem ipsum");
  vfd.GE7000_setFontStyle(true, false);
  vfd.print("proportional: lorem ipsum");
}

void loop() {
}

