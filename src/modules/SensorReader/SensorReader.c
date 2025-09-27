#include "../SensorReader/SensorReader.h"

enum CMD selectCmd(int serializedInput)
{
    
};


int serializeInput(const int* L1, const int* L2, const int* L3, const int* L4,const int* AUX,int threshold) {
    

    
    int b1 = (*L1 > threshold) ? 1 : 0;  
    int b2 = (*L2 > threshold) ? 1 : 0;
    int b3 = (*L3 > threshold) ? 1 : 0;
    int b4 = (*L4 > threshold) ? 1 : 0;
    int aux = (*AUX > threshold) ? 1 : 0;

    
    int command = (aux*1)+((b1*8)+(b2*4)+(b3*2)+(b4*1))*10;
    
//aux will be the last digit denoted by 0 or 1
    return command;
}
