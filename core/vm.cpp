//
// Created by cl d on 2024/7/10.
//

#include "vm.h"
#include <stdarg.h>
#include <stdio.h>
#include "value.h"

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        default:         return false; // Unreachable.
    }
}

VM::VM() {
    this->stack = std::stack<Value>();
}

void VM::resetStack() {

}

void VM::freeVM() {}

void VM::push(Value value) {
    this->stack.push(value);
}

Value VM::pop() {
    Value top = stack.top();
    stack.pop();
    return top;
}

Value VM::peek(int distance) {
    std::stack<Value> tmp = this->stack;
    for (int i = 0 ; i < distance && !this->stack.empty(); i ++) {
        tmp.pop();
    }
    return tmp.top();
}

void VM::runtimeError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = this->ip - &(this->chunks->instructions[0]) - 1;
    int line = chunks->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

InterpretResult VM::interpret(const char* source) {
    Chunk chunk = Chunk();

    this->parser = new Parser();
    if (!parser->compile(source, &chunk)) {
        chunk.freeChunk();
        return INTERPRET_COMPILE_ERROR;
    }
    this->chunks = &chunk;
    this->ip = &this->chunks->instructions[0];
    InterpretResult result = run();
    chunk.freeChunk();
    return result;
}

INSTRUCTION_TYPE VM::read_byte() {
    return *ip++;
}

InterpretResult VM::run() {

#define BINARY_OP(valueType, op) \
    do {              \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while(false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        std::stack<Value> tmp = this->stack;
        for (;!tmp.empty();) {
            printf("[ ");
            printValue(tmp.top());
            printf(" ]");
            tmp.pop();
        }
        printf("\n");
        this->chunks->disassembleInstruction((int)(ip - &chunks->instructions[0]));
#endif
        INSTRUCTION_TYPE instruction = read_byte();
        switch (instruction) {
            case OP_CONSTANT: {
                Value constant = read_constant();
                push(constant);
                printValue(constant);
                printf("\n");
                break;
            }
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number");
                    return INTERPRET_RUNTIME_ERROR;
                }
                double value = -AS_NUMBER(pop());
                push(NUMBER_VAL(value));
                break;
            }
            case OP_ADD: {
                BINARY_OP(NUMBER_VAL, +);
                break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(NUMBER_VAL, -);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(NUMBER_VAL, *);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(NUMBER_VAL, -);
                break;
            }
            case OP_NOT: {
                push(BOOL_VAL(isFalsey(pop())));
                break;
            }

            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
            case OP_NIL: push(NIL_VAL); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;
        }
    }
#undef BINARY_OP
}



Value VM::read_constant() {
    INSTRUCTION_TYPE byte = read_byte();
    return this->chunks->constants[byte];
}