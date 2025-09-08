#include "modules/OutputProcessor/OutputProcessor.h"
#include "modules/SensorReader/SensorReader.h"
#include "modules/StepperControl/StepperControl.h"
#include "modules/LogicProcessor/LogicProcessor.h"

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
  pinMode(LED_BUILTIN, OUTPUT);
}

// The loop() function runs over and over again forever.
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on (HIGH is the voltage level)
  delay(1000);                      // Wait for a second (1000 milliseconds)
  digitalWrite(LED_BUILTIN, LOW);    // Turn the LED off by making the voltage LOW
  delay(1000);                      // Wait for a second
}