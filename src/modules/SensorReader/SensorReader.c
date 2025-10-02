#ifndef SENSORREADER_H
#define SENSORREADER_H
#include "../SensorReader/SensorReader.h"

#include <stdint.h>

enum CMD {
  NOP = 15,
  HALT = 0,
  MOV = 8,
  CPY = 4,
  ADD = 12,
  INC = 10,
  DEX = 9,
  MULT = 5,
  JMP = 11,
  CMP = 13,
  INVALID
};

enum CMD selectCmd(int serializedInput) {
  enum CMD opCodeMap[16] = {
      HALT, INVALID, INVALID, INVALID,
      CPY,  MULT,    INC,     INVALID,
      MOV,  DEX,     INVALID, JMP,
      ADD,  CMP,     INVALID, NOP,
  };
  if (serializedInput < 0 || serializedInput > 15) {
    return INVALID;
  }
  return opCodeMap[serializedInput];
};

int serializeInput(int16_t L1, int16_t L2, int16_t L3, int16_t L4, int16_t AUX) {
    
    const uint16_t threshold = 500;
    int8_t b1 = (L1 < threshold) ? 1 : 0;  
    int8_t b2 = (L2 < threshold) ? 1 : 0;
    int8_t b3 = (L3 < threshold) ? 1 : 0;
    int8_t b4 = (L4 < threshold) ? 1 : 0;
    int8_t aux = (AUX < threshold) ? 1 : 0;

    int command = b1 * 10000 + b2 * 1000 + b3 * 100 + b4 * 10 + aux;
    return command;
}

#endif
