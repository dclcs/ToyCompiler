//
// Created by cl d on 2024/7/11.
//

#ifndef TOYCOMPILER_PARSER_H
#define TOYCOMPILER_PARSER_H

#include "scanner.h"
#include "chunk.h"

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

class Parser;
typedef void (*ParseFn)(Parser *);
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

class Parser {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;

    Scanner* scanner;
    Chunk* compilingChunk;

    ParseRule *rules;
private:
    void errorAt(Token* token, const char* message);
    void errorAtCurrent(const char* message);
    void error(const char* message);

    void advance();
    void consume(TokenType type, const char* message);
    void expression();
    void parsePrecedence(Precedence precedence);
    ParseRule* getRule(TokenType type);
    static void grouping(Parser *parser);
    static void unary(Parser *parser);
    static void binary(Parser *parser);
    static void number(Parser *parser);
    static void literal(Parser* parser);
    void emitByte(uint8_t byte);
    void emitBytes(uint8_t byte1, uint8_t byte2);
    void emitReturn();
    void emitConstant(Value value);
    uint8_t makeConstant(Value value);
public:
    bool compile(const char* source, Chunk* chunk);
    void endCompiler();
    Chunk* currentChunk();
};


#endif //TOYCOMPILER_PARSER_H
