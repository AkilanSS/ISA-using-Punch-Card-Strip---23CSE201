#ifndef LOGICPROCESSOR_H
#define LOGICPROCESSOR_H
#include "../SensorReader/SensorReader.h"

void processCmd(enum CMD cmd);
void updateMOV(int registerA, int registerB);


#endif