#ifndef SENSORREADER_H
#define SENSORREADER_H
#include "../SensorReader/SensorReader.h"
enum CMD{
    NOP=15,
    HALT=0,
    MOV=8,
    CPY=4,
    ADD=12,
    INC=10,
    DEC=9,
    MULT=5,
    JMP=11,
    CMP=13,
    INVALID
};

int serializeInput(const int* L1, const int* L2, const int* L3, const int* L4, const int* AUX);
CMD selectCmd(int serializedInput){
 CMD opCodeMap[16]={
    CMD::HALT,
    CMD::INVALID,
    CMD::INVALID,
    CMD::INVALID,
    CMD::CPY,
    CMD::MULT,
    CMD::INC,
    CMD::INVALID,
    CMD::MOV,
    CMD::DEC,
    CMD::INVALID,
    CMD::JMP,
    CMD::ADD,
    CMD::CMP,
    CMD::INVALID,
    CMD::NOP,
 };
 if (serializedInput<0 || serializedInput>15){
    return CMD::INVALID;
 }
 return opCodeMap[serializedInput];
};

#endif

