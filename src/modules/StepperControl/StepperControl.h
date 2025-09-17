#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <stdbool.h>

enum Direction
{
    Forward,
    Reverse
};

float optimalTheta(int totalRows, float sizeofRows, float discRadius);
void actuateStepper(int optimalTheta, _Bool stepFlag, enum Direction direction, int step);


#endif