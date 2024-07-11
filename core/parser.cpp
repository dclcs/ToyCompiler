//
// Created by cl d on 2024/7/11.
//

#include "parser.h"


void Parser::advance() {
    this->previous = this->current;
    for(;;) {
        this->current = scanner->scanToken();
        if (this->current.type != TOKEN_ERROR) break;
        errorAtCurrent(current.start);
    }
}

void Parser::consume(TokenType type, const char *message) {
    if (this->current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

ParseRule *Parser::getRule(TokenType type) {
    return &rules[type];
}

bool Parser::compile(const char *source, Chunk *chunk) {
    this->scanner = new Scanner(source);

    ParseRule rules[] = {
        [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
        [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
        [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
        [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
        [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
        [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
        [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
        [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
        [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
        [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
        [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
        [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
        [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
        [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
        [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
        [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
        [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
        [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
        [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
        [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
        [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
        [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
        [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
        [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
        [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
        [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
        [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
        [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
    };
    this->rules = rules;
    this->compilingChunk = chunk;

    this->hadError = false;
    this->panicMode = false;
    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression");
    endCompiler();
#ifdef DEBUG_PRINT_CODE
    if (!hadError) {
        currentChunk()->disassembleChunk("code");
    }
#endif
    return !hadError;
}

void Parser::parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    prefixRule(this);
    while (precedence <= getRule(current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(previous.type)->infix;
        infixRule(this);
    }
}

void Parser::expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

void Parser::grouping(Parser *parser) {
    parser->expression();

    parser->consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

void Parser::unary(Parser *parser) {
    TokenType operatorType = parser->previous.type;
    parser->parsePrecedence(PREC_UNARY);
    switch (operatorType) {
        case TOKEN_MINUS: parser->emitByte(OP_NEGATE);
            break;
        default:
            break;
    }
}

void Parser::number(Parser *parser) {
    double value = strtod(parser->previous.start, nullptr);
    parser->emitConstant(value);
}

void Parser::binary(Parser *parser) {
    TokenType operatorType = parser->previous.type;
    ParseRule* rule = parser->getRule(operatorType);
    parser->parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_PLUS:          parser->emitByte(OP_ADD); break;
        case TOKEN_MINUS:         parser->emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:          parser->emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:         parser->emitByte(OP_DIVIDE); break;
        default:
            break; // Unreachable.
    }
}


void Parser::emitByte(uint8_t byte) {
    this->currentChunk()->writeChunk(byte, previous.line);
}

void Parser::emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void Parser::emitReturn() {
    emitByte(OP_RETURN);
}

void Parser::emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8_t Parser::makeConstant(Value value) {
    int constant = currentChunk()->addConstant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk");
        return 0;
    }
    return (uint8_t)constant;
}

void Parser::endCompiler() {
    emitByte(OP_RETURN);
}

Chunk *Parser::currentChunk() {
    return compilingChunk;
}

void Parser::errorAtCurrent(const char *message) {
    errorAt(&this->current, message);
}

void Parser::error(const char *message) {
    errorAt(&previous, message);
}

void Parser::errorAt(Token *token, const char *message) {
    if (panicMode) return;
    panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    hadError = true;
}