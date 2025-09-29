#ifndef SENSORREADER_H
#define SENSORREADER_H



#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum CMD {

    MOV,
    CPY,
    ADD,
    INC,
    DEC,
    MULT,
    JMP,
    CMP
};

int serializeInput(int16_t L1, int16_t L2, int16_t L3, int16_t L4, int16_t AUX);
enum CMD selectCmd(int serializedInput);

#ifdef __cplusplus
}
#endif
#endif