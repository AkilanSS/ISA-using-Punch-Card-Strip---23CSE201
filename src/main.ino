#include <Stepper.h>

const int LIGHT_THRESHOLD = 550;
const int ADVANCE_PAST_ROW_STEPS = -170;
const int CREEP_STEPS = -5;
const int CENTERING_STEPS = -20;
const int MAX_SEARCH_STEPS = 195;
const int MOTOR_SPEED = 5;


const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int ldrSensors[5] = {A0, A1, A2, A3, A4};

enum State { HALT, MANUAL_STEP_F, MANUAL_STEP_B, READING_LOOP };
enum InstructionState {FETCH_OPCODE, FETCH_PARAMS};

bool boolFlag = false;
bool waitFlag = true;
bool jumpFlag = false;

State currentState = HALT;
String cmd;
int stepsToMove = 0;
int programCounter = 0;


void setup() {
  Serial.begin(9600);
  cmd.reserve(16); 
  myStepper.setSpeed(MOTOR_SPEED);
  Serial.println("Paper Tape Reader Initialized.");
  Serial.println("Commands: start, stop, mov <steps>, movb <steps>, debug");
  Serial.println("Position the first row under the sensors and type START.");
}

void loop() {
  handleSerialCommands();
  switch (currentState) {
    case READING_LOOP:
    read_current_row_label:
      Serial.println("------------------------------------");
      Serial.print("PC: " + String(programCounter) + " | reading current row -> ");
      readAndPrintRowData();

      if (findNextRow()) { 
        Serial.println("row centered");
        programCounter++;
        delay(500);
      } else {
        Serial.println("\nFAIL: Search timed out for row " + String(programCounter + 1) + ".");
        Serial.println("Assuming PC " + String(programCounter + 1) + " is -> 00000");
        programCounter++;
        Serial.println("Re-synchronizing by searching for the next data row...");
        
        bool foundNextDataRow = false;
        for (int i = 0; i < 6; i++) {
            if (findNextRow()) {
                foundNextDataRow = true; 
                break;
            } else {
                Serial.println("FAILSAFE: Search timed out for row " + String(programCounter + 1) + ".");
                Serial.println("Assuming PC " + String(programCounter + 1) + " is -> 00000");
                programCounter++;
            }
        }
        
        if (foundNextDataRow) {
            Serial.println("Re-sync successful. aligned.");
            programCounter++;
            goto read_current_row_label; 
        } else {
            Serial.println("Likely end of tape. Halting.");
            currentState = HALT;
        }
      }
      break;

    case MANUAL_STEP_F:
      myStepper.step(stepsToMove);
      currentState = HALT;
      break;

    case MANUAL_STEP_B:
      myStepper.step(-stepsToMove);
      currentState = HALT;
      break;

    case HALT:
      break;
  }
}


bool isAnyHoleDetected() {
  for (int i = 0; i < 5; i++) {
    if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
      return true;
    }
  }
  return false;
}

void readAndPrintRowData() {
  String resultString = "";
  for (int i = 0; i < 5; i++) {
    if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
      resultString += "1";
    } else {
      resultString += "0";
    }
  }
  Serial.println(resultString);
}

bool findNextRow() {
  Serial.println("Moving into inter-row gap");
  myStepper.step(ADVANCE_PAST_ROW_STEPS);
  delay(100);
  Serial.println("Searching for next row edge");
  int stepsSearched = 0;
  while (stepsSearched < MAX_SEARCH_STEPS) {
    if (isAnyHoleDetected()) {
      Serial.print("Edge detected after ");
      Serial.print(stepsSearched);
      myStepper.step(CENTERING_STEPS);
      return true;
    }
    myStepper.step(CREEP_STEPS);
    stepsSearched += abs(CREEP_STEPS);
    delay(20);
  }
  return false;
}

void handleSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      cmd.trim();
      if (cmd == "start") {
        Serial.println("command received: START");
        programCounter = 0;
        currentState = READING_LOOP;
      } else if (cmd == "stop") {
        Serial.println("command received: STOP");
        currentState = HALT;
      } else if (cmd.startsWith("mov ")) {
        stepsToMove = cmd.substring(4).toInt();
        Serial.println("command: manual move forward " + String(stepsToMove) + " steps.");
        currentState = MANUAL_STEP_F;
      } else if (cmd.startsWith("movb ")) {
        stepsToMove = cmd.substring(5).toInt();
        Serial.println("command: manual move backward " + String(stepsToMove) + " steps.");
        currentState = MANUAL_STEP_B;
      } else if (cmd == "debug") {
          debug_print();
      }
      else {
        Serial.println("Unknown command: " + cmd);
      }
      cmd = "";
    } else {
      cmd += c; 
    }
  }
}


void debug_print() {
  Serial.print("Binary: ");
  for (int i = 0; i < 5; i++) {
    if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
      Serial.print("1 ");
    } else {
      Serial.print("0 ");
    }
  }
  Serial.print("Raw: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(1023 - analogRead(ldrSensors[i]));
    Serial.print(" ");
  }
  Serial.println();
}


/*

#include <Stepper.h>
#include "src/modules/SensorReader/SensorReader.h"
#include "src/modules/LogicProcessor/LogicProcessor.h"

// --- Hardware & Movement Constants ---
const int LIGHT_THRESHOLD = 550;
const int ADVANCE_PAST_ROW_STEPS = -170;
const int CREEP_STEPS = -5;
const int CENTERING_STEPS = -20;
const int MAX_SEARCH_STEPS = 195;
const int MOTOR_SPEED = 10;

const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int ldrSensors[5] = {A0, A1, A2, A3, A4};

// --- State Management ---
enum State { HALTED, RUNNING, MANUAL_STEP_F, MANUAL_STEP_B };
State currentState = HALTED;

String cmd;
int stepsToMove = 0;
int programCounter = 0;

void setup() {
    Serial.begin(9600);
    cmd.reserve(16);
    myStepper.setSpeed(MOTOR_SPEED);
    LogicProcessor_init();
    Serial.println("\nPaper Tape Reader Initialized.");
    Serial.println("Commands: run, stop, stepf <steps>, stepb <steps>, regs");
    Serial.println("Position the tape and type 'run' to begin execution.");
}

void loop() {
    handleSerialCommands();

    if (currentState == RUNNING && !haltFlag) {
        Serial.println("------------------------------------");
        Serial.print("PC: " + String(programCounter) + " | ");
        
        // 1. Read the physical row data
        int rowData = readAndSerializeRow(ldrSensors, LIGHT_THRESHOLD);
        Serial.print("Read Row Data: ");
        // Print binary for debugging
        for (int i = 4; i >= 0; i--) { Serial.print((rowData >> i) & 1); }
        Serial.println();

        // 2. Process the data with the CPU
        LogicProcessor_process(rowData);
        
        // 3. Check for flags from the CPU
        if (jumpFlag) {
            Serial.println("JUMP triggered! Seeking to row " + String(jumpAddress));
            seekToRow(jumpAddress);
            jumpFlag = false; // Reset the flag
        } else if (haltFlag) {
            Serial.println("HALT signal received. Stopping execution.");
            currentState = HALTED;
        } else {
            // 4. Move to the next physical row
            if (findNextRow()) {
                programCounter++;
            } else {
                Serial.println("End of tape or read error. Halting.");
                currentState = HALTED;
            }
        }
        delay(200); // Small delay between cycles
    }
}

// Moves the tape until the target row is under the sensor
void seekToRow(int targetRow) {
    int currentRow = programCounter;
    // Simple seek: move forward or backward one row at a time
    while(currentRow != targetRow) {
        if (currentRow < targetRow) {
            if(findNextRow()) {
                currentRow++;
            } else {
                Serial.println("Seek failed, could not find row. Halting.");
                currentState = HALTED;
                return;
            }
        }
        // Backward seeking would need reverse motion logic
    }
    programCounter = targetRow;
    Serial.println("Seek complete. PC is now " + String(programCounter));
}


bool findNextRow() {
    myStepper.step(ADVANCE_PAST_ROW_STEPS);
    delay(50);
    int stepsSearched = 0;
    while (stepsSearched < MAX_SEARCH_STEPS) {
        // Check if any sensor detects a hole (the edge of a row)
        for (int i = 0; i < 5; i++) {
            if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
                myStepper.step(CENTERING_STEPS);
                return true;
            }
        }
        myStepper.step(CREEP_STEPS);
        stepsSearched += abs(CREEP_STEPS);
        delay(10);
    }
    return false;
}

void handleSerialCommands() {
    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') {
            cmd.trim();
            if (cmd == "run") {
                Serial.println("CMD: RUNNING PROGRAM");
                LogicProcessor_init(); // Reset CPU on every run
                programCounter = 0;
                currentState = RUNNING;
            } else if (cmd == "stop") {
                Serial.println("CMD: HALTED");
                currentState = HALTED;
            } else if (cmd.startsWith("stepf ")) {
                stepsToMove = cmd.substring(6).toInt();
                myStepper.step(stepsToMove);
            } else if (cmd.startsWith("stepb ")) {
                stepsToMove = cmd.substring(6).toInt();
                myStepper.step(-stepsToMove);
            } else if (cmd == "regs") {
                LogicProcessor_printRegisters();
            }
            cmd = "";
        } else {
            cmd += c;
        }
    }
}
*/