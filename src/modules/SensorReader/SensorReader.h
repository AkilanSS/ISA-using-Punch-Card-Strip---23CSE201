#ifndef SENSORREADER_H
#define SENSORREADER_H

enum CMD{
    NOP,
    HALT,
    MOV,
    CPY,
    ADD,
    INC,
    DEC,
    MULT,
    JMP,
    CMP
};

int serializeInput(const int* L1, const int* L2, const int* L3, const int* L4, const int* AUX);
enum CMD selectCmd(int serializedInput);

#endif