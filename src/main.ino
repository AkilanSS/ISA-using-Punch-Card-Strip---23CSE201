#include <Stepper.h>
const int stepsPerRevolution = 2048;

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

enum Direction { UP, DOWN, HALT, STEP, STEPB, MOVSTEPB, START };

enum Direction rollDir = HALT;
int ldrSensors[5] = {A0, A1, A2, A3, A4};
String cmd;
int stepsToMove = 0;
void setup() {
	Serial.begin(9600);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	cmd.reserve(16);
}

int SEARCH_STEP = 180;
int FINE_STEP = 20;

int programCounter = 0;

void loop() {
	debug_print();
	while (Serial.available() > 0) {
		char c = Serial.read();
		if (c == '\n') {
			cmd.trim();
			if (cmd == "START") {
				Serial.println("Command Received: START");
				rollDir = START;
			} else {
				Serial.println("Unknown command: " + cmd);
			}
			cmd = "";
		} else {
			cmd += c;
		}
	}

	if (rollDir == START) {
		debug_print();
		Serial.println("Moving from Row " + String(programCounter + 1));
		myStepper.setSpeed(5);
		myStepper.step(-180);

		int final_ldr0 = 0;
		int final_ldr1 = 0;
		int final_ldr2 = 0;
		int final_ldr3 = 0;
		int final_ldr4 = 0;

		const int NUM_READINGS = 2;
		for (int i = 0; i < NUM_READINGS; i++) {
			myStepper.step(-10);
			final_ldr0 |= (1023 - analogRead(A0) < 550 ? 1 : 0);
			final_ldr1 |= (1023 - analogRead(A1) < 550 ? 1 : 0);
			final_ldr2 |= (1023 - analogRead(A2) < 550 ? 1 : 0);
			final_ldr3 |= (1023 - analogRead(A3) < 550 ? 1 : 0);
			final_ldr4 |= (1023 - analogRead(A4) < 550 ? 1 : 0);

			delay(200);
		}
		String resultString = "";
		resultString += final_ldr0;
		resultString += final_ldr1;
		resultString += final_ldr2;
		resultString += final_ldr3;
		resultString += final_ldr4;

		Serial.println("Likely result for this row: " + resultString);
		programCounter++;
	}
}

void debug_print() {
	for (int i = 0; i < 5; i++) {
		if ((1023 - analogRead(ldrSensors[i])) < 550) {
			Serial.print("1");
			Serial.print(" ");
		} else {
			Serial.print("0");
			Serial.print(" ");
		}
	}

	for (int i = 0; i < 5; i++) {
		Serial.print(1023 - analogRead(ldrSensors[i]));
		Serial.print(" ");
	}
	Serial.println();
}