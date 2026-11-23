#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

static void resetStack(VM* vm) {
    vm->stackTop = vm->stack;
}

void initVirtualMachine(VM* vm) {
    resetStack(vm);
}

void freeVirtualMachine(VM* vm) {
    
}

static InterpretResult run(VM* vm) {
    #define READ_BYTE() (*vm->ip++)
    #define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
    #define BINARY_OP(op) \
            do { \
                double b = pop(vm); \
                double a = pop(vm); \
                push(vm, a op b); \
            } while(false)

    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
            printf("          ");
            for (Value* slot = vm->stack; slot < vm->stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
        #endif    

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CST: 
                Value constant = READ_CONSTANT();
                push(vm, constant);
                break;
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUB: BINARY_OP(-); break;
            case OP_MUL: BINARY_OP(*); break;
            case OP_DIV: BINARY_OP(/); break;
            case OP_NEG: push(vm, -pop(vm)); break;
            case OP_RET: 
                printValue(pop(vm));
                printf("\n");
                return INTERPRET_OK;
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    compile(source);
    return INTERPRET_OK;
}

void push(VM* vm, Value value) {
    *vm->stackTop = value;
    vm->stackTop++;
}

Value pop(VM* vm) {
    vm->stackTop--;
    return *vm->stackTop;
}