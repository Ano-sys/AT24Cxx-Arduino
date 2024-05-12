#include "EEPROM.h"

int EEPROM_A0 = 10;
int EEPROM_A1 = 11;
int EEPROM_A2 = 12;
int EEPROM_WP = 13;
int EEPROM_SIZE = 256;

// ------------------Setter functions------------------

void EEPROM_SET_A0(int pin){
  EEPROM_A0 = pin;
}

void EEPROM_SET_A1(int pin){
  EEPROM_A1 = pin;
}

void EEPROM_SET_A2(int pin){
  EEPROM_A2 = pin;
}

void EEPROM_SET_WP(int pin){
  EEPROM_WP = pin;
}

void EEPROM_SET_SIZE(int size){
  EEPROM_SIZE = size;
}

// ------------------SETUP function------------------

// Set set pins as output and initialize them with low
void EEPROM_SETUP(){
  pinMode(EEPROM_A0, OUTPUT);
  pinMode(EEPROM_A1, OUTPUT);
  pinMode(EEPROM_A2, OUTPUT);

  pinMode(EEPROM_WP, OUTPUT);

  // set address pins to 0, 0, 0 and disable write protection
  digitalWrite(EEPROM_A0, LOW);
  digitalWrite(EEPROM_A1, LOW);
  digitalWrite(EEPROM_A2, LOW);
  digitalWrite(EEPROM_WP, LOW);

  // Init a wire session
  Wire.begin();
}

// ------------------WRITE functions------------------

// Writes a byte to the eeprom
void EEPROM_WRITE(unsigned int dAddr, byte dVal){
  Wire.beginTransmission(rw_I);
  Wire.write(dAddr >> 8);
  Wire.write(dAddr & 0xFF);
  Wire.write(dVal);
  Wire.endTransmission();
  
  delay(10);
}

// Writes a String [byte by byte] to the eeprom 
void EEPROM_WRITE(unsigned int dAddr, String dStr){
  // output what is written
  Serial.print("Writing: \"");
  Serial.print(dStr);
  Serial.println("\"");
  int starting_point = dAddr;
  int length = check_length(dAddr, (int)dStr.length());
  //int byte_counter = 0;
  for (int i = 0; i < length; i++){
    // if the connection to the eeprom is not given break the loop
    Serial.print(dStr[i]);
    Serial.print(" - Address: ");
    Serial.print(dAddr);
    Wire.beginTransmission(rw_I);
    Wire.write(dAddr >> 8);
    Wire.write(dAddr & 0xFF);
    Wire.write(dStr[i]);
    Wire.endTransmission();
    //if(!Wire.available()){ break; }

    Serial.println();
    dAddr++;
    //byte_counter++;
    delay(10);
  }
  Serial.println("Checking if everything has been writen correctly...");
  Serial.print("Starting at Byte: ");
  Serial.print(starting_point);
  Serial.print(", Ending at Byte: ");
  Serial.println(starting_point + length);

  check_write(dStr, starting_point, length);
}

// ------------------READ functions------------------

byte EEPROM_READ(unsigned int dAddr){
  byte ret = 0;

  Wire.beginTransmission(rw_I);
  Wire.write((int)dAddr >> 8);
  Wire.write((int)dAddr & 0xFF);
  Wire.endTransmission(false);

  Wire.requestFrom(rw_I, 1);

  if(Wire.available()){
    ret = Wire.read();
  }
  return ret;
}

// Reads bytes in length of length following given address
String EEPROM_READ(unsigned int dAddr, size_t length){
  String str = "";

  length = check_length(dAddr, length);

  Wire.beginTransmission(rw_I);
  Wire.write(dAddr >> 8);
  Wire.write(dAddr & 0xFF);
  Wire.endTransmission(false);

  Wire.requestFrom(rw_I, length);

  while(Wire.available()){
    str += (char)Wire.read();
  }
  if(str.length() < length - 1){
    str = "NOT AVAILABLE!";
  }
  //Wire.endTransmission();

  return str;
}

// ------------------CLEAR functions------------------

void EEPROM_CLEAR(unsigned int address, size_t length){
  byte c = 0;
  
  length = check_length(address, length);

  for(int i = 0; i < (int)length; i++){
    Serial.print("Clearing Block: ");
    Serial.println(address);
    EEPROM_WRITE(address, c);
    address++;
    delay(10);
  }
}

int check_length(int address, int length){
  if((int)(length + address) > EEPROM_SIZE){
    return EEPROM_SIZE - address;
  }
  return length;
}

void check_write(String toWrite, unsigned int start, unsigned int length){
  String ret = EEPROM_READ(start, length);
  
  for(int i = 0; i < (int)toWrite.length(); i++){
    if(ret[i] != toWrite[i]){
      Serial.println("Something went wrong by writing the string to the EEPROM! Try again!");
      return;
    }
  }
  Serial.print("Successfully wrote: \"");
  Serial.print(toWrite);
  Serial.println("\"");
}
