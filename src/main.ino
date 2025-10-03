#include <Stepper.h>

const int LIGHT_THRESHOLD = 550;
const int SEARCH_INCREMENT_STEPS = -10;
const int CENTERING_ADJUSTMENT_STEPS = -15;
const int JUMP_OUT_OF_HOLE_STEPS = -150;
const int MAX_SEARCH_STEPS = 250;
const int MOTOR_SPEED = 2;

const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int ldrSensors[5] = {A0, A1, A2, A3, A4};

struct CPUState {
  int registers[5];
  int programCounter;
};

struct Instruction {
  int opcode;
  int operand1;
  int operand2;
};

enum State { HALT, MANUAL_STEP_F, MANUAL_STEP_B, RUNNING };
State currentState = HALT;
String cmd;
int stepsToMove = 0;

CPUState cpuState;

bool findNextHole();
void executeInstruction(Instruction* instr, CPUState* cpu);

void setup() {
  Serial.begin(9600);
  cmd.reserve(16);
  myStepper.setSpeed(MOTOR_SPEED);
  Serial.println("Instruction Tape Interpreter Initialized.");
  Serial.println("Commands: start, stop, mov <steps>, movb <steps>, debug");
  Serial.println(">>> Manually align the FIRST row (opcode) under the sensors before starting.");
}

int binToDec(String bin) {
  return strtol(bin.c_str(), NULL, 2);
}

String getRegisterName(int regId) {
    switch(regId) {
        case 1: return "REG_A";
        case 2: return "REG_B";
        case 3: return "REG_C";
        case 4: return "REG_D";
        case 5: return "REG_E";
        default: return "UNKNOWN_REG";
    }
}

String readRowDataString() {
  String resultString = "";
  for (int i = 0; i < 5; i++) {
    if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
      resultString += "1";
    } else {
      resultString += "0";
    }
  }
  return resultString;
}

int fetchOperandValue() {
    String fullOperandData = "";
    bool endOfOperand = false;
    do {
        if (findNextHole()) {
            cpuState.programCounter++;
            String rawRow = readRowDataString();
            fullOperandData += rawRow.substring(0, 4);
            if (rawRow.charAt(4) == '1') {
                endOfOperand = true;
            }
        } else {
            Serial.println("Error: Failed to fetch operand.");
            currentState = HALT;
            return -1;
        }
    } while (!endOfOperand && currentState != HALT);
    return binToDec(fullOperandData);
}

Instruction decodeInstruction() {
    Instruction decodedInstr = {0, 0, 0};
    String rawOpcodeRow = readRowDataString();
    decodedInstr.opcode = binToDec(rawOpcodeRow.substring(0, 4));

    Serial.println("------------------------------------");
    Serial.print("PC: " + String(cpuState.programCounter) + " | " + rawOpcodeRow + " -> ");

    switch (decodedInstr.opcode) {
        case 15:
        case 0:
            Serial.println("OP: " + String(decodedInstr.opcode == 0 ? "HALT" : "NOP"));
            break;
        case 8:
        case 12:
        case 13:
        case 5:
            Serial.println("OP: Two Register");
            decodedInstr.operand1 = fetchOperandValue();
            decodedInstr.operand2 = fetchOperandValue();
            break;
        case 4:
        case 2:
        case 7:
            Serial.println("OP: Register, Immediate");
            decodedInstr.operand1 = fetchOperandValue();
            decodedInstr.operand2 = fetchOperandValue();
            break;
        case 6:
        case 9:
        case 11:
            Serial.println("OP: Single Operand");
            decodedInstr.operand1 = fetchOperandValue();
            break;
        default:
            Serial.println("DATA / UNKNOWN");
            break;
    }
    return decodedInstr;
}

void processCycle() {
    Instruction currentInstruction = decodeInstruction();

    if (currentState == HALT) return;
    
    executeInstruction(&currentInstruction, &cpuState);
    
    if (currentState == RUNNING) {
        if (findNextHole()) {
            cpuState.programCounter++;
            delay(500);
        } else {
            Serial.println("Execution HALTED. Could not find the next instruction.");
            currentState = HALT;
        }
    }
}

void executeInstruction(Instruction* instr, CPUState* cpu) {
    Serial.print("=> EXECUTING: ");
    int reg1_idx, reg2_idx;

    auto toIdx = [](int regId) { return regId - 1; };

    switch (instr->opcode) {
        case 15:
            Serial.println("NOP. Doing nothing.");
            break;
        case 0:
            Serial.println("HALT. Stopping execution.");
            currentState = HALT;
            break;
        case 4:
            reg1_idx = toIdx(instr->operand1);
            cpu->registers[reg1_idx] = instr->operand2;
            Serial.println("CPY " + String(instr->operand2) + " into " + getRegisterName(instr->operand1));
            break;
        case 2:
            reg1_idx = toIdx(instr->operand1);
            cpu->registers[reg1_idx] += instr->operand2;
             Serial.println("ADDI " + String(instr->operand2) + " to " + getRegisterName(instr->operand1));
            break;
        default:
            Serial.println("Instruction with opcode " + String(instr->opcode) + " not yet implemented.");
            break;
    }
}

void loop() {
  handleSerialCommands();

  switch (currentState) {
    case RUNNING:
      processCycle();
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

bool findNextHole() {
  myStepper.step(JUMP_OUT_OF_HOLE_STEPS);
  int stepsSearched = abs(JUMP_OUT_OF_HOLE_STEPS);

  while (stepsSearched < MAX_SEARCH_STEPS) {
    myStepper.step(SEARCH_INCREMENT_STEPS);
    stepsSearched += abs(SEARCH_INCREMENT_STEPS);
    delay(20); 

    for (int i = 0; i < 5; i++) {
        if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
            myStepper.step(CENTERING_ADJUSTMENT_STEPS);
            return true; 
        }
    }
  }
  Serial.println("FAILSAFE: Search exceeded MAX_SEARCH_STEPS.");
  return false;
}

void handleSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      cmd.trim();
      if (cmd == "start") {
        Serial.println("Command Received: START");
        cpuState.programCounter = 0;
        for(int i = 0; i < 5; i++) { cpuState.registers[i] = 0; }
        currentState = RUNNING;
      } else if (cmd == "stop") {
        Serial.println("Command Received: STOP");
        currentState = HALT;
      } else if (cmd.startsWith("mov ")) {
        stepsToMove = cmd.substring(4).toInt();
        currentState = MANUAL_STEP_F;
      } else if (cmd.startsWith("movb ")) {
        stepsToMove = cmd.substring(5).toInt();
        currentState = MANUAL_STEP_B;
      } else if (cmd == "debug") {
          debug_print();
      } else {
        Serial.println("Unknown command: " + cmd);
      }
      cmd = "";
    } else {
      cmd += c;
    }
  }
}

void debug_print() {
    Serial.println("\n--- DEBUG INFO ---");
    Serial.println("Current State: " + String(currentState == HALT ? "HALT" : "RUNNING"));
    Serial.println("PC: " + String(cpuState.programCounter));
    Serial.println("Registers:");
    for (int i = 0; i < 5; i++) {
        Serial.println("  " + getRegisterName(i + 1) + ": " + String(cpuState.registers[i]));
    }
    Serial.print("LDR Raw (Inverted): ");
    for (int i = 0; i < 5; i++) {
        Serial.print(1023 - analogRead(ldrSensors[i]));
        Serial.print(" ");
    }
    Serial.println("\n--------------------");
}