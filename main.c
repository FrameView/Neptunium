//Neptunium birth date: 19/11/2026

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"


int main(int argc, const char* argv[]) {
    VM vm;
    initVirtualMachine(&vm);

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CST, 60);
    writeChunk(&chunk, constant, 60);

    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CST, 60);
    writeChunk(&chunk, constant, 60);

    writeChunk(&chunk, OP_ADD, 60);

    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CST, 60);
    writeChunk(&chunk, constant, 60);

    writeChunk(&chunk, OP_DIV, 60);
    writeChunk(&chunk, OP_NEG, 60);
    writeChunk(&chunk, OP_RET, 60);

    disassembleChunk(&chunk, "test chunk");

    interpret(&vm, &chunk);

    freeVirtualMachine(&vm);
    freeChunk(&chunk);
    return 0;
}