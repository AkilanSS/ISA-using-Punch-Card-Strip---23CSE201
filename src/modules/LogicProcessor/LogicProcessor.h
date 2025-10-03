#ifndef LOGICPROCESSOR_H
#define LOGICPROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif

extern bool jumpFlag;      
extern int jumpAddress;    
extern bool haltFlag;      

void LogicProcessor_init();

void LogicProcessor_process(int rowData);

void LogicProcessor_printRegisters();

#ifdef __cplusplus
}
#endif

#endif