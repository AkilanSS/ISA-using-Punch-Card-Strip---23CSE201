#include "modules/OutputProcessor/OutputProcessor.h"
#include "modules/SensorReader/SensorReader.h"
#include "modules/StepperControl/StepperControl.h"

#define NUM_REGISTERS 16

int registers[NUM_REGISTERS];
int programCounter;

void setup()
{
  for (int i = 0; i < NUM_REGISTERS; i++)
  {
    registers[i] = 0;
  }

  programCounter = 0;
}

void loop()
{
}