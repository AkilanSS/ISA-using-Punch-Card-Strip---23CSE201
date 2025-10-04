#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


enum CMD {
    CMD_HALT    = 0,    // 0000
    CMD_ADDI    = 2,    // 0010
    CMD_CPY     = 4,    // 0100
    CMD_MULTR   = 5,    // 0101
    CMD_INC     = 6,    // 0110
    CMD_MULTI   = 7,    // 0111
    CMD_MOV     = 8,    // 1000
    CMD_DEC     = 9,    // 1001
    CMD_JMP     = 11,   // 1011
    CMD_ADDR    = 12,   // 1100
    CMD_CMP     = 13,   // 1101
    CMD_NOP     = 15,   // 1111
    
    CMD_INVALID
};
// Declarations remain the same
int serializeInput(int16_t L1, int16_t L2, int16_t L3, int16_t L4);
enum CMD selectCmd(int serializedInput);

#ifdef __cplusplus
}
#endif
#endif