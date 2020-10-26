// This file acts as a header for the .ino file.  Since the .ino file is the hardware abstraction layer
// in Arduino world, it also contains prototypes for abstracted hardware calls in addition to prototypes
// for abstracted time functions which in other compilers would just be other .c and .h files

// An abstraction layer is a translation between one thing and another.  In C it would consist of a pair of files
// - one with code and one with just the names of available functions (prototypes).  The names of the functions as 
// advertised in the header file are chosen by you to be as standard in your world as possible.  That way, all
// your higher level application code can make use of the same standardly named functions.  This abstraction layer
// then "calls" the actual base code and returns values in your own standard way.  An abstraction layer typically 
// does very little beyond calling the base function and returning it's results.  This way all your nice working 
// code can remain untouched and this abstraction layer becomes essentially a part of the project for a particular 
// target processor or compiler. 
//
// This file is appropriately called Arduino_AL.h since this is the abstraction layer, however our associated code 
// file should be called Arduino_AL.cpp for clarity.  Because this is Arduino, it must have a .ino suffix in order 
// to be able to make use of built in (or easily included) base functions because they are all C++.  As a result, 
// the .ino file also takes on the role of defining the abstraction layer between Arduino and any library of C or 
// C++ code - specifically without the necessity to "Arduinofy" code which may have been perfectly fine and tested 
// where and how it was.
//
// So, follows are function prototypes for functions found in the .ino file whatever it might be called.  Each of
// these functions simply calls the Arduino base function and returns the results where appropriate.  Wrappers.

#ifndef __ARDUINOAL_H
#define __ARDUINOAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>              // Find integer types like "uint8_t"  
#include <stdbool.h>             // Find type "bool"

#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 2 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)

// defines related to hardware and relevant only to the hardware abstraction layer (this and .ino files)
#define EveChipSelect_PIN     D8  // PB1
#define EveAudioEnable_PIN    D1  // PD1
#define EvePDN_PIN            D0  // PB0
#define SDChipSelect_PIN      D3  // PD3
#define Button1_PIN           5  // PD5
#define Button2_PIN           6  // PD6 
#define Button3_PIN           7  // PD7
#define SDCardDetect_PIN      D4  // PD4

// In Arduino we lose access to these defines from outside the .ino, so they are redfined here.
// In order to prevent mysteries, these are defining these with hopefully unique names.
#define FILEREAD   0      
#define FILEWRITE  1 
#define FILEAPPEND 2

//#define Log(...)  { sprintf(LogBuf,__VA_ARGS__); DebugPrint(LogBuf); } // Stuff string and parms via sprintf and output
#define Log(...) // Liberate (a lot of) RAM by uncommenting this empty definition

//#define LogBufSize 64
extern char *LogBuf;         // The singular universal data array used for all things including logging

// Common program elements function prototypes
void MainLoop(void);
void GlobalInit(void);

// Hardware peripheral abstraction function prototypes
uint8_t ReadPin(uint8_t pin);
void SetPin(uint8_t pin, bool state);
void SD_Init(void);
void SPI_Enable(void);
void SPI_Disable(void);
void SPI_Write(uint8_t data);
void SPI_WriteByte(uint8_t data);
void SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length);
void SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length);

// These functions encapsulate Arduino core functions
void Init_Keys(void);
void DebugPrint(char *str);
void MyDelay(uint32_t DLY);
bool SaveTouchMatrix(void);
bool LoadTouchMatrix(void);
void Eve_Reset_HW(void);

// Function encapsulation of file operations - abstractions
void FileOpen(char *filename, uint8_t mode);
void FileClose(void);
uint8_t FileReadByte(void);
void FileReadBuf(uint8_t *data, uint32_t NumBytes);
void FileWriteByte(uint8_t data);
uint32_t FileSize(void);
uint32_t FilePosition(void);
bool FileSeek(uint32_t offset);
bool myFileIsOpen(void);

#ifdef __cplusplus
}
#endif

#endif
