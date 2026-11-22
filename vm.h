#ifndef np_vm_h
#define np_vm_h

#include "chunk.h"

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVirtualMachine(VM* vm);
void freeVirtualMachine(VM* vm);
InterpretResult interpret(VM* vm, Chunk* chunk);

#endif