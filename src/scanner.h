#ifndef SCANNER_H
#define SCANNER_H

#include "util.h"

typedef enum {
    TOKEN_LEFT_PAREN,     // (
    TOKEN_RIGHT_PAREN,    // )
    TOKEN_LEFT_BRACE,     // {
    TOKEN_RIGHT_BRACE,    // }
    TOKEN_LEFT_BRACKET,   // [
    TOKEN_RIGHT_BRACKET,  // ]
    TOKEN_COMMA,          // ,
    TOKEN_DOT,            // .
    TOKEN_MINUS,          // -
    TOKEN_PLUS,           // +
    TOKEN_SEMICOLON,      // ;
    TOKEN_SLASH,          // /
    TOKEN_STAR,           // *
    TOKEN_BANG,           // !
    TOKEN_BANG_EQUAL,     // !=
    TOKEN_EQUAL,          // =
    TOKEN_EQUAL_EQUAL,    // ==
    TOKEN_GREATER,        // >
    TOKEN_GREATER_EQUAL,  // >=
    TOKEN_LESS,           // <
    TOKEN_LESS_EQUAL,     // <=
    TOKEN_AT,             // @
    TOKEN_AND,            // &&
    TOKEN_OR,             // ||
    TOKEN_REF,            // &
    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    // Keywords.
    TOKEN_STRUCT,
    TOKEN_IF,
    TOKEN_FOR,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_STATIC,
    // encode int, float, ...
    TOKEN_TYPENAME,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_TYPEDEF,
    TOKEN_CONST,
    TOKEN_ENUM,
    TOKEN_UNION,
    TOKEN_ERROR,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

void initScanner(Scanner* scanner, const char* source);

Token* scanTokens(const char* source);

void printToken(Token* token);

void printTokens(Token* tokens);

String tokenToString(Token token);

String tokenType(TokenType type);
#endif