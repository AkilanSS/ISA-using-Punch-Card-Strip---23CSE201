// #include "LogicProcessor.h"
// #include <stdio.h>    
// #include <stdbool.h>   
// #include <Arduino.h>   


// typedef enum { FETCH_OPCODE, FETCH_PARAMS } InstructionState;

// static InstructionState currentState;
// static int registers[5];
// static bool compareFlag;

// bool jumpFlag = false;
// int jumpAddress = 0;
// bool haltFlag = false;


// typedef enum {
//     NOP = 15, HALT = 0, MOV = 8, CPY = 4, ADD = 12, INC = 6,
//     DEC = 9, MULT = 5, JMP = 11, CMP = 13, INVALID_CMD
// } OpCode;

// static OpCode currentOpCode;
// static int params[2];
// static int paramCount;
// static int expectedParams;
// static int multiRowParamBuffer;

// static void executeInstruction();

// void LogicProcessor_init() {
//     for (int i = 0; i < 5; i++) {
//         registers[i] = 0;
//     }
//     currentState = FETCH_OPCODE;
//     compareFlag = false;
//     jumpFlag = false;
//     haltFlag = false;
//     paramCount = 0;
//     expectedParams = 0;
//     multiRowParamBuffer = 0;
//     Serial.println("registers cleared.");
// }

// void LogicProcessor_process(int rowData) {
//     if (haltFlag) return;

//     int data = rowData >> 1; // param/opcode
//     int aux = rowData & 1;   // aux flag

//     switch (currentState) {
//         case FETCH_OPCODE: {
//             OpCode opCodeMap[16] = {
//               HALT, INVALID_CMD, INVALID_CMD, INVALID_CMD, CPY, MULT, INC, INVALID_CMD,
//               MOV, DEC, INVALID_CMD, JMP, ADD, CMP, INVALID_CMD, NOP
//             };
//             currentOpCode = opCodeMap[data];
            
//             switch(currentOpCode) {
//                 case MOV: case CPY: case ADD: case MULT: case CMP:
//                     expectedParams = 2;
//                     break;
//                 case INC: case DEC: case JMP:
//                     expectedParams = 1;
//                     break;
//                 default:
//                     expectedParams = 0;
//                     break;
//             }

//             if (expectedParams > 0 && aux == 0) {
//                 paramCount = 0;
//                 multiRowParamBuffer = 0;
//                 currentState = FETCH_PARAMS;
//             } else {
//                 executeInstruction(); 
//             }
//             break;
//         }

//         case FETCH_PARAMS: {
//             multiRowParamBuffer = (multiRowParamBuffer << 4) | data;

//             if (aux == 1) { 
//                 params[paramCount] = multiRowParamBuffer;
//                 paramCount++;
//                 multiRowParamBuffer = 0;
//             }
//             if (paramCount == expectedParams) {
//                 executeInstruction();
//                 currentState = FETCH_OPCODE;
//             }
//             break;
//         }
//     }
// }

// static void executeInstruction() {
//     char outputBuffer[128]; 

//     Serial.print("Executing... ");
//     switch (currentOpCode) {
//         case HALT:
//             Serial.println("HALT");
//             haltFlag = true;
//             break;
//         case MOV: 
//             snprintf(outputBuffer, sizeof(outputBuffer), "MOV R%d, R%d", params[0], params[1]);
//             Serial.println(outputBuffer);
//             registers[params[0]] = registers[params[1]];
//             break;
//         case CPY: // R, Imm
//             snprintf(outputBuffer, sizeof(outputBuffer), "CPY R%d, %d", params[0], params[1]);
//             Serial.println(outputBuffer);
//             registers[params[0]] = params[1];
//             break;
//         case ADD: // R, R
//             snprintf(outputBuffer, sizeof(outputBuffer), "ADD R%d, R%d", params[0], params[1]);
//             Serial.println(outputBuffer);
//             registers[params[0]] += registers[params[1]];
//             break;
//         case INC: // R
//             snprintf(outputBuffer, sizeof(outputBuffer), "INC R%d", params[0]);
//             Serial.println(outputBuffer);
//             registers[params[0]]++;
//             break;
//         case DEC: // R
//             snprintf(outputBuffer, sizeof(outputBuffer), "DEC R%d", params[0]);
//             Serial.println(outputBuffer);
//             registers[params[0]]--;
//             break;
//         case CMP: // R, R
//             snprintf(outputBuffer, sizeof(outputBuffer), "CMP R%d, R%d", params[0], params[1]);
//             Serial.println(outputBuffer);
//             compareFlag = (registers[params[0]] == registers[params[1]]);
//             break;
//         case JMP: // Address
//             snprintf(outputBuffer, sizeof(outputBuffer), "JMP %d", params[0]);
//             Serial.println(outputBuffer);
//             if (compareFlag) {
//                 jumpFlag = true;
//                 jumpAddress = params[0];
//             }
//             break;
//         case NOP:
//             Serial.println("NOP");
//             break;
//         default:
//             Serial.println("INVALID INSTRUCTION");
//             break;
//     }
//     LogicProcessor_printRegisters();
// }

// void LogicProcessor_printRegisters() {
//     char regBuffer[128];
//     snprintf(regBuffer, sizeof(regBuffer), 
//              "  [REGS: E=%d A=%d B=%d C=%d D=%d] [CMP_FLAG: %d]",
//              registers[0], registers[1], registers[2], registers[3], registers[4],
//              compareFlag);
//     Serial.println(regBuffer);
// }