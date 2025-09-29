#include <Stepper.h>
const int stepsPerRevolution = 2048;

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int ldrSensors[5] = {A0, A1, A2, A3, A4};

enum Direction { UP, DOWN, HALT, STEP, STEPB };
enum Direction rollDir;

enum StateMachine {
	FOUND_FUNCTION,
	SEEK_FUNCTION,
	SEEK_DATA,
	FOUND_DATA,
	SEEK_PARAM,
	FOUND_PARAM,
	SEEK_REGISTER,
	FOUND_REGISTER,
	
}

String cmd;

void setup() {
	Serial.begin(9600);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	cmd.reserve(16);

	rollDir = HALT;
}

void loop() {

}