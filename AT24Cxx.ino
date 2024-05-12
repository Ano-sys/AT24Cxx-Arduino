#include <stdlib.h>
#include "EEPROM.h"

// max length the user can write in one instruction to the eeprom
const int str_size = 32;

// enumeration to later determine userfriendly which signal was sent by user
enum SIGNAL {
  READ,
  WRITE,
  CLEAR,
  NONE,
};

// struct which contains sent instruction by user
typedef struct _INSTRUCTION {
  SIGNAL signal;         // signal to determine for example a READ isntruction
  unsigned int addr;     // contains the addr from which the eeprom is read or writen from/to
  char value[str_size];  // contains the string the user wants to write to the eeprom
} INSTRUCTION;

void setup() {
  // call to EEPROM.ino, labels the default pins as output (10, 11, 12, 13) (Slaves and WP)
  EEPROM_SETUP();

  // Init a serial connection, for user inputs and device outputs
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");
}

void loop() {
  // wait for userinput
  while (Serial.available() == 0) {}
  // Read the sent instruction and align to a new INSTRUCTION struct variable
  INSTRUCTION *instr = read_serial();
  // if read_serial() could not read a proper instruction the user is prompted with following text
  if (instr == NULL) {
    Serial.println("False Instruction!  -  [INSTRUCTION ] [ADDRESS] [VALUE]");
  }
  // else the instruction is handled
  else {
    if (instr->signal == READ) {
      // store the return from eeprom of given address
      String ret = EEPROM_READ(instr->addr, (int)strtol(instr->value, NULL, 10));

      // print the user the result
      Serial.print("EEPROM_MEMORY = ");
      Serial.print(ret);

      Serial.println();
    } else if (instr->signal == WRITE) {
      // write string from user to eeprom
      EEPROM_WRITE(instr->addr, instr->value);
    } else if (instr->signal == CLEAR) {
      // clear the eeprom fields by user defined size
      EEPROM_CLEAR(instr->addr, (int)strtol(instr->value, NULL, 10));
    }
  }
  // clear malloced memory for struct INSTRUCTION
  free(instr);
}

INSTRUCTION *read_serial() {
  // Example: 'write 10 "Hello EEPROM"' -> writes "Hello EEPROM" to the eeprom starting at the 11th byte (Indexing starts by 0)
  INSTRUCTION *instr = (INSTRUCTION *)malloc(sizeof(INSTRUCTION));
  if (instr == NULL) { return NULL; }  // malloc failed

  // Begin read of userinput
  String input = Serial.readString();
  input.trim();
  String remain = input;
  // check if space is given and if space is correctly set between instruction and address
  int space = remain.indexOf(' ');
  if (space == -1 && space != 4 && space != 5) {
    return NULL;
  }
  instr->signal = NONE;
  // Check instruction/signal
  String instruction = remain.substring(0, space);
  if (strcasecmp(instruction.c_str(), "READ") == 0) {
    instr->signal = READ;
  } else if (strcasecmp(instruction.c_str(), "WRITE") == 0) {
    instr->signal = WRITE;
  } else if (strcasecmp(instruction.c_str(), "CLEAR") == 0) {
    instr->signal = CLEAR;
  } else {
    Serial.print("Not READ/WRITE!");
    return NULL;
  }

  // Check address
  remain = remain.substring(space + 1);

  space = remain.indexOf(' ');
  if (space == -1) {
    Serial.print("No space between Address and Value!\t-\t");
    return NULL;
  }
  // Differentiation between isntructions with string and instructions without string at the end of input
  if (instr->signal == READ || instr->signal == CLEAR) {
    // read last number mostly used as length to iterate throught eeprom
    instr->addr = (unsigned int)strtol(remain.substring(0, space).c_str(), NULL, 10);
    remain = remain.substring(space + 1);
    strncpy(instr->value, remain.c_str(), str_size - 1);
  }
  if (instr->signal == WRITE) {
    // read string as value which is later pushed to eeprom
    int value_start = remain.indexOf('"');
    if (value_start == -1) {
      Serial.print("No initial quotation mark for value!");
      return NULL;
    }
    int value_end = remain.indexOf('"', value_start + 1);
    if (value_end == -1) {
      Serial.print("No ending quotation mark for value!");
      return NULL;
    }

    if (space > value_start && space < value_end) {
      Serial.print("No Space between Address and Value!");
      return NULL;
    }

    instr->addr = (unsigned int)strtol(remain.substring(0, space).c_str(), NULL, 10);

    remain = remain.substring(value_start + 1);
    value_end = remain.indexOf('"');
    remain = remain.substring(0, value_end);

    strncpy(instr->value, remain.c_str(), str_size - 1);
    instr->value[str_size - 1] = '\0';
  }

  // and finally return the built struct
  return instr;
}