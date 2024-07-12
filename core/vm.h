//
// Created by cl d on 2024/7/10.
//

#ifndef TOYCOMPILER_VM_H
#define TOYCOMPILER_VM_H

#include "../common.h"
#include "scanner.h"
#include "parser.h"

using namespace std;
#define STACK_MAX 256
typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
}InterpretResult;

class VM {
public:
    Chunk* chunks;
    stack<Value> stack;
    INSTRUCTION_TYPE* ip;
    Parser *parser;
public:
    VM();
    void freeVM();
    void push(Value value);
    Value pop();
    Value peek(int distance);
    InterpretResult interpret(const char* source);
    void resetStack();
    InterpretResult run();
    INSTRUCTION_TYPE read_byte();
    Value read_constant();
    void runtimeError(const char* format, ...);
};


#endif //TOYCOMPILER_VM_H
