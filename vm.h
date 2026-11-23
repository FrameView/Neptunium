#ifndef np_vm_h
#define np_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVirtualMachine(VM* vm);
void freeVirtualMachine(VM* vm);
InterpretResult interpret(const char* source);
void push(VM* vm, Value value);
Value pop(VM* vm);

#endif