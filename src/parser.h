#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

typedef struct {
    Token* tokens;
    int current;
    int previous;
    Expr* left;
} Parser;

void initParser(Parser* parser, Token* tokens);

Program* parse(Token* tokens);

Decl* declaration(Parser* parser);

Expr* expression(Parser* parser);

Stmt* statement(Parser* parser);

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // ||
    PREC_AND,         // && 
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

typedef Expr* (*ParseFn)(Parser* parser);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Expr* binary(Parser* parser);

Expr* grouping(Parser* parser);

Expr* atom(Parser* parser);

Expr* unary(Parser* parser);

Expr* call(Parser* parser);

#endif