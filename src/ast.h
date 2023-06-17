#ifndef AST_H
#define AST_H

#include "scanner.h"

//---------------------- AST---------------------------
// Grammar:
// program        → declaration* EOF ;
// declaration    → varDecl | funDecl | ;
// varDecl        → type IDENTIFIER ( "=" expression )? ";" ;
// funDecl        → type IDENTIFIER "(" parameters? ")" block ;
// parameters     → IDENTIFIER ( "," IDENTIFIER )* ;
// statement      → exprStmt | ifStmt | whileStmt | block ;
// exprStmt       → expression ";" ;
// ifStmt         → "if" "(" expression ")" statement ( "else" statement )? ;
// whileStmt      → "while" "(" expression ")" statement ;
// block          → "{" declaration* statement* "}" ;
// expression     → assignment ;
// assignment     → IDENTIFIER "=" assignment | logic_or ;
// logic_or       → logic_and ( "or" logic_and )* ;
// logic_and      → equality ( "and" equality )* ;
// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
// comparison     → addition ( ( ">" | ">=" | "<" | "<=" ) addition )* ;
// addition       → multiplication ( ( "-" | "+" ) multiplication )* ;
// multiplication → unary ( ( "/" | "*" ) unary )* ;
// unary          → ( "!" | "-" ) unary | call ;
// call           → primary ( "(" arguments? ")" )* ;
// arguments      → expression ( "," expression )* ;
// primary        → int | float | string | "true" | "false" | "(" expression ")"
//                  | IDENTIFIER ;
// type           → "int" | "float" | "string" | "bool" | "void"

typedef struct Expr Expr;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_NEG,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE,
    OP_OR,
    OP_AND,
    OP_AT,
    OP_DOT,
    OP_REF,
    OP_DEREF,
    OP_ASSIGN,
    OP_SUBSCRIPT,
    OP_CALL,
    OP_ERROR,
} Op;

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_IDENTIFIER,
} Type;

struct Expr {
    enum {
        EXPR_ASSIGNMENT,
        EXPR_BINARY,
        EXPR_CALL,
        EXPR_GROUPING,
        EXPR_LITERAL,
        EXPR_UNARY,
        EXPR_VARIABLE,
    } type;
    union {
        struct {
            String name;
            Expr* value;
        } assignment;
        struct {
            Expr* left;
            Op op;
            Expr* right;
        } binary;
        struct {
            String name;
            Expr** arguments;
            int argumentsCount;
        } call;
        struct {
            union literal {
                int intVal;
                float floatVal;
                String stringVal;
                bool boolVal;
            } value;
            Type type;
        } literal;
        struct {
            Op op;
            Expr* right;
        } unary;
        struct {
            String name;
        } variable;
    };
};

#define assign(value) ((value).assignment)

typedef struct Stmt Stmt;

typedef struct Decl Decl;

struct Stmt {
    enum { STMT_BLOCK, STMT_EXPRESSION, STMT_IF, STMT_RETURN, STMT_WHILE } type;
    union {
        struct {
            Decl** declarations;
            int declarationsCount;
            Stmt** statements;
            int statementsCount;
        } block;
        struct {
            Expr* expression;
        } expression;
        struct {
            Expr* condition;
            Stmt* thenBranch;
            Stmt* elseBranch;
        } ifStmt;
        struct {
            Expr* value;
        } returnStmt;
        struct {
            Expr* condition;
            Stmt* body;
        } whileStmt;
    };
};

struct Decl {
    enum {
        DECL_FUNCTION,
        DECL_VARIABLE,
        DECL_STRUCT,
    } type;
    union {
        struct {
            String name;
            String* params;
            String* parameterTypes;
            int parametersCount;
            String returnType;
            Stmt* body;
        } function;
        struct {
            String name;
            String type;
            Expr* initializer;
        } variable;
        struct {
            String name;
            Decl** fields;
            int fieldsCount;
        } record;
    };
};

typedef struct Program Program;

struct Program {
    Decl** declarations;
    int count;
};

void printProgram(Program* program);
void printExpr(Expr* expr);
void printStmt(Stmt* stmt);
void printDecl(Decl* decl);

char* sprintExpr(Expr* expr);

Op getOp(Token* token, bool isBinary);

#endif