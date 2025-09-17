#include <Stepper.h>

const int stepsPerRevolution = 2048;

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
Stepper myStepper2 = Stepper(stepsPerRevolution, 8, 9, 10, 11);
void setup() {
  // Nothing to do (Stepper Library sets pins as outputs)
}

void loop() {
  myStepper2.setSpeed(10);
  myStepper2.step(-stepsPerRevolution);
  delay(1000);

  myStepper.setSpeed(10);
  myStepper.step(-stepsPerRevolution);
  delay(1000);
}