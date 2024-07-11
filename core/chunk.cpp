//
// Created by cl d on 2024/7/10.
//

#include "chunk.h"

static void printValue(Value value) {
    printf("%g", value);
}
static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->instructions[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants[constant]);
    printf("'\n");
    return offset + 2;
}


Chunk::Chunk() {
    this->instructions = vector<INSTRUCTION_TYPE>();
    this->capacity = 0;
    this->lines = vector<int>();
    this->constants = vector<Value>();
}

int Chunk::count() {
    return this->instructions.size();
}

void Chunk::writeChunk(INSTRUCTION_TYPE byte, int line) {
    this->instructions.push_back(byte);
    this->lines.push_back(line);
}

int Chunk::addConstant(Value value) {
    this->constants.push_back(value);
    return this->constants.size() - 1;
}

void Chunk::freeChunk() {
    this->instructions.clear();
    this->lines.clear();
}


void Chunk::disassembleChunk(const char *name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < this->instructions.size();) {
        offset = disassembleInstruction(offset);
    }
}

int Chunk::disassembleInstruction(int offset) {
    printf("%04d", offset);
    if (offset > 0 && this->lines[offset] == this->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", this->lines[offset]);
    }
    uint8_t instruction = this->instructions[offset];
    switch (instruction) {
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", this, offset);
        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
        case OP_ADD:
            return simpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
