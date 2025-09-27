#include <Stepper.h>

const int stepsPerRevolution = 2048;

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

enum Direction { UP, DOWN, HALT, STEP, STEPB };

enum Direction rollDir = HALT;

String cmd;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  cmd.reserve(16);
}

void loop() {
  Serial.print(1023 - analogRead(A0));
  Serial.print(" ");
  Serial.print(1023 - analogRead(A1));
  Serial.print(" ");
  Serial.print(1023 - analogRead(A2));
  Serial.print(" ");
  Serial.print(1023 - analogRead(A3));
  Serial.print(" ");
  Serial.print(1023 - analogRead(A4));
  Serial.println(" ");

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      if (cmd == "HALT") {
        rollDir = HALT;
        Serial.println("State changed to: HALT");
      } else if (cmd == "UP") {
        rollDir = UP;
        Serial.println("State changed to: UP");
      } else if (cmd == "DOWN") {
        rollDir = DOWN;
        Serial.println("State changed to: DOWN");
      } else if (cmd == "MOV") {
        rollDir = STEP;
        Serial.println("State changed to: STEP");
      } else if (cmd == "MOVB") {
        rollDir = STEPB;
        Serial.println("State changed to: STEP");
      }
      cmd = "";
    } else {

      cmd += c;
    }
  }

  switch (rollDir) {
    case UP:
      myStepper.setSpeed(10);
      myStepper.step(-stepsPerRevolution);
      break;

    case DOWN:
      myStepper.setSpeed(10);
      myStepper.step(stepsPerRevolution);
      break;

    case STEP:
      myStepper.setSpeed(10);
      myStepper.step(100);
      rollDir = HALT;
      break;
    case STEPB:
      myStepper.setSpeed(10);
      myStepper.step(-100);
      rollDir = HALT;
      break;

    case HALT:
      break;
  }
}