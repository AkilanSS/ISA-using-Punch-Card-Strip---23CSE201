#ifndef SENSORREADER_H
#define SENSORREADER_H
#include "../SensorReader/SensorReader.h"
enum CMD {
  NOP = 15,
  HALT = 0,
  MOV = 8,
  CPY = 4,
  ADD = 12,
  INC = 10,
  DEC = 9,
  MULT = 5,
  JMP = 11,
  CMP = 13,
  INVALID
};

int serializeInput(const int *L1, const int *L2, const int *L3, const int *L4,
                   const int *AUX);
enum CMD selectCmd(int serializedInput) {
  enum CMD opCodeMap[16] = {
      HALT, INVALID, INVALID, INVALID,
      CPY,  MULT,    INC,     INVALID,
      MOV,  DEC,     INVALID, JMP,
      ADD,  CMP,     INVALID, NOP,
  };
  if (serializedInput < 0 || serializedInput > 15) {
    return INVALID;
  }
  return opCodeMap[serializedInput];
};

#endif
