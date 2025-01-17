//
// Created by cl d on 2024/7/11.
//

#include "scanner.h"
#include <string.h>

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

Scanner::Scanner(const char* source) {
    this->current = source;
    this->start = source;
    this->line = 1;
}

Token Scanner::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);
    char c = advance();
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();
    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(
                    match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(
                    match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(
                    match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(
                    match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string();
    }
    return errorToken("Unexpected character. ");
}

char Scanner::advance() {
    current++;
    return current[-1];
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (*current != expected) return false;
    current++;
    return true;
}

char Scanner::peek() {
    return *current;
}

char Scanner::peekNext() {
    if (isAtEnd()) return '\0';
    return current[1];
}


bool Scanner::isAtEnd() {
    return *current == '\0';
}

void Scanner::skipWhitespace() {
    for(;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                this->line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}


Token Scanner::number() {
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }
    return makeToken(TOKEN_NUMBER);
}

Token Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() != '\n') line ++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");
    advance();
    return makeToken(TOKEN_STRING);
}

Token Scanner::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

TokenType Scanner::identifierType() {
    switch (start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND); //and
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS); // class
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE); // else
        case 'f':
            if (current - start > 1) {
                switch (start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE); // false
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR); // for
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN); //fun
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (current - start > 1) {
                switch (start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(int start, int length, const char *rest, TokenType type) {
    if (this->current - this->start == start + length && memcmp(this->start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}


Token Scanner::makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = start;
    token.length = (int)(current - start);
    token.line = line;
    return token;
}

Token Scanner::errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int) strlen(message);
    token.line = line;
    return token;
}