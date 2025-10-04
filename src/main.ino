#include <Stepper.h>
#include "./modules/SensorReader/SensorReader.h"

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
  bool zero_flag;
};

struct Instruction {
  enum CMD opcode;
  int operand1;
  int operand2;
};

enum State { HALT, MANUAL_STEP_F, MANUAL_STEP_B, RUNNING };
State currentState = HALT;
String cmd;
int stepsToMove = 0;

CPUState cpuState; //here we create a virtual cpu with 5 registers to operate on

bool findNextHole();
bool isAnyHoleDetected();
void executeInstruction(Instruction* instr, CPUState* cpu);
void debug_print();

void setup() {
  Serial.begin(9600);
  cmd.reserve(16);
  myStepper.setSpeed(MOTOR_SPEED);
  Serial.println("Instruction Tape Interpreter Initialized.");
  Serial.println("Commands: start, stop, mov <steps>, movb <steps>, debug");
  Serial.println(">>> Manually align the FIRST row (opcode) under the sensors before starting.");
}

int binToDec(String bin) {  //helper function for converting string, this is used for larger row covering operands to number
  return strtol(bin.c_str(), NULL, 2);
}

String getRegisterName(int regId) { //printing routine for displaying in serial monitor
    switch(regId) {
        case 1: return "REG_A";
        case 2: return "REG_B";
        case 3: return "REG_C";
        case 4: return "REG_D";
        case 5: return "REG_E";
        default: return "UNKNOWN_REG";
    }
}

String readRowDataString() { //returns the row for debugging
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
            Serial.println("system error happened, cannot fetch operand, will also halt");
            currentState = HALT;
            return -1;
        }
    } while (!endOfOperand && currentState != HALT);
    return binToDec(fullOperandData);
}

Instruction decodeInstruction() {
    Instruction decodedInstr = {CMD_NOP, 0, 0};

    int val1 = 1023 - analogRead(ldrSensors[0]); //subtracted with 1023 to invert the adc signal
    int val2 = 1023 - analogRead(ldrSensors[1]);
    int val3 = 1023 - analogRead(ldrSensors[2]);
    int val4 = 1023 - analogRead(ldrSensors[3]);

    int opcodeValue = serializeInput(val1, val2, val3, val4);
    enum CMD command = selectCmd(opcodeValue);
    decodedInstr.opcode = command;

    Serial.println();
    Serial.print("PC: " + String(cpuState.programCounter) + "  Opcode Val: " + String(opcodeValue));

    switch (command) {
        case CMD_NOP:
        case CMD_HALT:
            Serial.println(" CMD: " + String(command == CMD_HALT ? "HALT" : "NOP"));
            break;
        case CMD_MOV:
        case CMD_ADDR:  //these are for reg-reg parameter opcodes
        case CMD_CMP:
        case CMD_MULTR:
            decodedInstr.operand1 = fetchOperandValue();
            decodedInstr.operand2 = fetchOperandValue();
            break;
        case CMD_CPY: //seperats switch case ladder because it acts in imm values
        case CMD_ADDI:
        case CMD_MULTI:
            decodedInstr.operand1 = fetchOperandValue();
            decodedInstr.operand2 = fetchOperandValue();
            break;
        case CMD_INC:  //these are single operand op codes
        case CMD_DEC:
        case CMD_JMP:
            decodedInstr.operand1 = fetchOperandValue();
            break;
        case CMD_INVALID:
            Serial.println("invalid command");
            currentState = HALT;
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
            Serial.println("execution stopped, could not find next row");
            currentState = HALT;
        }
    }
}

void executeInstruction(Instruction* instr, CPUState* cpu) {
    Serial.print("Executing: ");
    int reg1_idx, reg2_idx;

    switch (instr->opcode) {
        case CMD_NOP:
            Serial.println("NOP no operation");
            break;
        case CMD_HALT:
            Serial.println("HALT stopping");
            currentState = HALT;
            break;
        case CMD_CPY:
            reg1_idx = instr->operand1 - 1;
            cpu->registers[reg1_idx] = instr->operand2;
            Serial.println("CPY " + String(instr->operand2) + " -> " + getRegisterName(instr->operand1));
            break;
        case CMD_MOV:
            reg1_idx = instr->operand1 - 1;
            reg2_idx = instr->operand1 - 1;
            cpu->registers[reg2_idx] = cpu->registers[reg1_idx];
            Serial.println("MOV " + getRegisterName(instr->operand1) + " -> " + getRegisterName(instr->operand2));
            break;
        case CMD_ADDR:
            reg1_idx = instr->operand1 - 1;
            reg2_idx = instr->operand1 - 1;
            cpu->registers[reg2_idx] += cpu->registers[reg1_idx];
            Serial.println("ADDR " + getRegisterName(instr->operand1) + " -> " + getRegisterName(instr->operand2));
            break;
        case CMD_ADDI:
            reg1_idx = instr->operand1 - 1;
            cpu->registers[reg1_idx] += instr->operand2;
            Serial.println("ADDI " + String(instr->operand2) + " -> " + getRegisterName(instr->operand1));
            break;
        case CMD_MULTR:
            reg1_idx = instr->operand1 - 1;
            reg2_idx = instr->operand1 - 1;
            cpu->registers[reg2_idx] *= cpu->registers[reg1_idx];
            Serial.println("MULTR " + getRegisterName(instr->operand1) + " -> " + getRegisterName(instr->operand2));
            break;
        case CMD_MULTI:
            reg1_idx = instr->operand1 - 1;
            cpu->registers[reg1_idx] *= instr->operand2;
            Serial.println("MULTI " + String(instr->operand2) + " -> " + getRegisterName(instr->operand1));
            break;
        case CMD_INC:
            reg1_idx = instr->operand1 - 1;
            cpu->registers[reg1_idx]++;
            Serial.println("INC " + getRegisterName(instr->operand1));
            break;
        case CMD_DEC:
            reg1_idx = instr->operand1 - 1;
            cpu->registers[reg1_idx]--;
            Serial.println("DEC " + getRegisterName(instr->operand1));
            break;
        case CMD_JMP:
            if (cpu->zero_flag) {
                cpu->programCounter = instr->operand1 - 1;
                Serial.println("JMP to Address " + String(instr->operand1));
            } else {
                Serial.println("JMP ignored");
            }
            break;
        case CMD_CMP:
            reg1_idx = instr->operand1 - 1;
            reg2_idx = instr->operand1 - 1;
            cpu->zero_flag = (cpu->registers[reg1_idx] == cpu->registers[reg2_idx]);
            Serial.println("CMP " + getRegisterName(instr->operand1) + ", " + getRegisterName(instr->operand2) + " (ZeroFlag=" + String(cpu->zero_flag) + ")");
            break;
        case CMD_INVALID:
            Serial.println("Invalid op code. will halt.");
            currentState = HALT;
            break;
        default:
            Serial.println("Unknown Opcode (" + String(instr->opcode) + "). Stopping");
            currentState = HALT;
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
    if (isAnyHoleDetected()) {
      myStepper.step(CENTERING_ADJUSTMENT_STEPS);
      return true; 
    }
  }
  Serial.println("exceeded max searching steps");
  return false;
}

bool isAnyHoleDetected() {
  for (int i = 0; i < 5; i++) {
    if ((1023 - analogRead(ldrSensors[i])) < LIGHT_THRESHOLD) {
      return true; 
    }
  }
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
        cpuState.zero_flag = false;
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
        Serial.println("command not found: " + cmd);
      }
      cmd = "";
    } else {
      cmd += c;
    }
  }
}

void debug_print() {
    Serial.println("\n - - debug mode");
    Serial.println("current state: " + String(currentState == HALT ? "HALT" : "RUNNING"));
    Serial.println("program counter: " + String(cpuState.programCounter));
    Serial.println("flag: " + String(cpuState.zero_flag));
    Serial.println("register :");
    for (int i = 0; i < 5; i++) {
        Serial.println("  " + getRegisterName(i + 1) + ": " + String(cpuState.registers[i]));
    }
    Serial.print("ldr raw values: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(1023 - analogRead(ldrSensors[i]));
        Serial.print(" ");
    }
    Serial.println("\n--------------------");
}