//
// Created by cl d on 2024/7/10.
//

#ifndef TOYCOMPILER_CHUNK_H
#define TOYCOMPILER_CHUNK_H

#include "../common.h"
#include "value.h"

using namespace std;

typedef uint8_t INSTRUCTION_TYPE;

typedef enum {
    OP_RETURN,
    OP_CONSTANT,
    OP_NEGATE, //Negate
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_LESS,
    OP_GREATER,
    OP_EQUAL,
    OP_TRUE,
    OP_FALSE,
    OP_NIL
} OpCode;

static void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NIL:
            printf("nil");
            break;
        case VAL_NUMBER:
            printf("%g", AS_NUMBER(value));
            break;
    }
}

class Chunk {
public:
    int capacity;
    vector<INSTRUCTION_TYPE> instructions;
    vector<int> lines;
    vector<Value> constants;

    void writeChunk(INSTRUCTION_TYPE byte, int line);
    void freeChunk();
    int addConstant(Value value);
    int count();


public:
    Chunk();
    int disassembleInstruction(int offset);
    void disassembleChunk(const char *name);
};



#endif //TOYCOMPILER_CHUNK_H
