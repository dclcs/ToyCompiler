//
// Created by cl d on 2024/7/10.
//

#ifndef TOYCOMPILER_CHUNK_H
#define TOYCOMPILER_CHUNK_H

#include "../common.h"
using namespace std;

typedef uint8_t INSTRUCTION_TYPE;
typedef double Value;

typedef enum {
    OP_RETURN,
    OP_CONSTANT,
    OP_NEGATE, //Negate
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE
} OpCode;

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
