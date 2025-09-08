#ifndef SENSORREADER_H
#define SENSORREADER_H

enum CMD{
    MOV,
    CPY,
    JMP,
    ADD,
    SUB
};

int serializeInput(const int* L1, const int* L2, const int* L3, const int* L4, const int* AUX);
enum CMD selectCmd(int serializedInput);

#endif