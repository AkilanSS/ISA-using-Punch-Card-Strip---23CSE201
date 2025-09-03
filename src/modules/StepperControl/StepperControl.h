#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

enum Direction
{
    Forward,
    Reverse
};

float optimalTheta(int totalRows, float sizeofRows, float discRadius);
void actuateStepper(int optimalTheta, bool stepFlag = true, Direction direction = Forward, int step = 0);


#endif