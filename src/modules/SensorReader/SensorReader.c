#include "SensorReader.h"

const uint16_t LDR_THRESHOLD = 500;

int serializeInput(int16_t L1, int16_t L2, int16_t L3, int16_t L4) {
    int8_t b1 = (L1 < LDR_THRESHOLD) ? 1 : 0;
    int8_t b2 = (L2 < LDR_THRESHOLD) ? 1 : 0;
    int8_t b3 = (L3 < LDR_THRESHOLD) ? 1 : 0;
    int8_t b4 = (L4 < LDR_THRESHOLD) ? 1 : 0;

    int commandValue = (b1 << 3) | (b2 << 2) | (b3 << 1) | b4;
    return commandValue;
}

enum CMD selectCmd(int serializedInput) {
    switch (serializedInput) {
        case 0:  return CMD_HALT;
        case 2:  return CMD_ADDI;  
        case 4:  return CMD_CPY;
        case 5:  return CMD_MULTR;  
        case 6:  return CMD_INC;
        case 7:  return CMD_MULTI;  
        case 8:  return CMD_MOV;
        case 9:  return CMD_DEC;    
        case 11: return CMD_JMP;
        case 12: return CMD_ADDR;   
        case 13: return CMD_CMP;
        case 15: return CMD_NOP;
        default:
            return CMD_INVALID;
    }
}