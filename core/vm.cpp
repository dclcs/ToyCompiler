//
// Created by cl d on 2024/7/10.
//

#include "vm.h"

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
    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        std::stack<Value> tmp = this->stack;
        for (;!tmp.empty();) {
            printf("[ ");
            printf("%g", tmp.top());
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
                printf("%g", constant);
                printf("\n");
                break;
            }
            case OP_RETURN: {
                printf("%g\n", pop());
                return INTERPRET_OK;
            }
            case OP_NEGATE: {
                push(-pop());
                break;
            }
            case OP_ADD: {
                push(binary_op(OP_ADD));
                break;
            }
            case OP_SUBTRACT: {
                push(binary_op(OP_SUBTRACT));
                break;
            }
            case OP_MULTIPLY: {
                push(binary_op(OP_MULTIPLY));
                break;
            }
            case OP_DIVIDE: {
                push(binary_op(OP_DIVIDE));
                break;
            }
        }
    }
}

Value VM::binary_op(OpCode code) {
    Value a = pop();
    Value b = pop();
    switch (code) {
        case OP_ADD:
            return a + b;
        case OP_SUBTRACT:
            return a - b;
        case OP_DIVIDE:
            return a / b;
        case OP_MULTIPLY:
            return a * b;
    }
}


Value VM::read_constant() {
    INSTRUCTION_TYPE byte = read_byte();
    return this->chunks->constants[byte];
}