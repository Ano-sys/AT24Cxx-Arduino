#include <Wire.h>
#include <string.h>

// READ/WRITE Instruction
#define rw_I 80
// Security Register Access Instruction
#define sr_I 88

// Setter
void EEPROM_SET_A0(int);
void EEPROM_SET_A1(int);
void EEPROM_SET_A2(int);
void EEPROM_SET_WP(int);

// Functions
void EEPROM_SETUP();
void EEPROM_WRITE(unsigned int, byte);
void EEPROM_WRITE(unsigned int, String);
byte EEPROM_READ(unsigned int);
String EEPROM_READ(unsigned int, size_t);
void EEPROM_CLEAR(unsigned int, size_t);
//void EEPROM_CLEAR(byte, size_t);
