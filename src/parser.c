#include "parser.h"
//---------------------- Parser-----------------------
// recursive descent + Pratt parser for expression

static Expr* parsePrecedence(Parser* parser, Precedence precedence);
static Stmt* block(Parser* parser);

static ParseRule rules[] = {
    /*
    TOKEN                   PREFIX     INFIX   PRECEDENCE
    */
    [TOKEN_LEFT_PAREN]    = {grouping, binary,   PREC_CALL},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,     PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,     PREC_NONE}, 
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,     PREC_NONE},
    [TOKEN_LEFT_BRACKET]  = {NULL,     binary,   PREC_CALL},
    [TOKEN_RIGHT_BRACKET] = {NULL,     NULL,     PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,     PREC_NONE},
    [TOKEN_DOT]           = {NULL,     binary,   PREC_CALL},
    [TOKEN_MINUS]         = {unary,    binary,   PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     binary,   PREC_TERM},
    [TOKEN_SLASH]         = {NULL,     binary,   PREC_FACTOR},
    [TOKEN_STAR]          = {unary,    binary,   PREC_FACTOR},
    [TOKEN_REF]           = {unary,    NULL,     PREC_UNARY},
    [TOKEN_BANG]          = {unary,    NULL,     PREC_UNARY},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL,     binary,   PREC_ASSIGNMENT},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary ,  PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
    [TOKEN_AND]           = {NULL,     binary,   PREC_AND},
    [TOKEN_OR]            = {NULL,     binary,   PREC_OR},
    [TOKEN_IDENTIFIER]    = {atom,     NULL,     PREC_NONE},
    [TOKEN_STRING]        = {atom,     NULL,     PREC_NONE},
    [TOKEN_NUMBER]        = {atom,     NULL,     PREC_NONE},
    [TOKEN_TRUE]          = {atom,     NULL,     PREC_NONE},
    [TOKEN_FALSE]         = {atom,     NULL,     PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,     PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,     PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,     PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,     PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,     PREC_NONE},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,     PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,     PREC_NONE},
};

void setLeft(Parser* parser, Expr* expr) {
    parser->left = expr;
}

Expr* getLeft(Parser* parser) {
    assertMsg(parser->left, "parser->left is NULL");
    Expr* left = parser->left;
    parser->left = NULL; // reset
    return left;
}

ParseRule* curRule(Parser* parser) {
    return &rules[parser->tokens[parser->current].type];
}

ParseRule* preRule(Parser* parser) {
    return &rules[parser->tokens[parser->previous].type];
}

void initParser(Parser* parser, Token* tokens) {
    parser->tokens = tokens;
    parser->current = 0;
    parser->previous = 0;
}

// return the current token and advance the parser.
static Token advance(Parser* parser) {
    Token token = parser->tokens[parser->current];
    parser->previous = parser->current;
    parser->current++;
    return token;
}

// if the current token is of the given type, advance it and return it.
// else panic with the given message.
static Token eat(Parser* parser, TokenType type, const char* message) {
    if (parser->tokens[parser->current].type == type) {
        advance(parser);
        return parser->tokens[parser->previous];
    }
    panic("line %d: %s\n", parser->tokens[parser->current].line, message);
    return parser->tokens[parser->current];  // unreachable
}

// if the current token is of the given type, advance it and return true.
// else return false.
static bool match(Parser* parser, TokenType type) {
    if (parser->tokens[parser->current].type == type) {
        advance(parser);
        return true;
    }
    return false;
}

ParseRule* getRule(TokenType type) { return &rules[type]; }

// parse literal, which is previous token.
Expr* atom(Parser* parser) {
    Token token = parser->tokens[parser->previous];
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_LITERAL;
    String str = tokenToString(token);
    switch (token.type) {
        case TOKEN_NUMBER:
            // if contains '.', then it's a float, else it's an int
            if (strchr(str.chars, '.')) {
                expr->literal.type = TYPE_FLOAT;
                expr->literal.value.floatVal = atof(str.chars);
            } else {
                expr->literal.type = TYPE_INT;
                expr->literal.value.intVal = atoi(str.chars);
            }
            break;
        case TOKEN_STRING:
            expr->literal.type = TYPE_STRING;
            expr->literal.value.stringVal = str;
            break;
        case TOKEN_TRUE:
            expr->literal.type = TYPE_BOOL;
            expr->literal.value.boolVal = true;
            break;
        case TOKEN_FALSE:
            expr->literal.type = TYPE_BOOL;
            expr->literal.value.boolVal = false;
            break;
        case TOKEN_IDENTIFIER:
            expr->variable.name = str;
            expr->type = EXPR_VARIABLE;
            break;
        default:
            panic("line %d: expected literal, but get %s: type %s\n",
                  token.line, tokenToString(token).chars,
                  tokenType(token.type).chars);
    }
    return expr;
}

Expr* grouping(Parser* parser) {
    Token op = parser->tokens[parser->previous];
    Expr* expr = expression(parser);
    switch (op.type) {
        case TOKEN_LEFT_PAREN:
            eat(parser, TOKEN_RIGHT_PAREN, "expected ')' after expression");
            return expr;
        default:
            panic("line %d: expected '(' or '[', but get %s: type %s\n",
                  op.line, tokenToString(op).chars, tokenType(op.type).chars);
    }
    return expr; // unreachable
}

Expr* unary(Parser* parser) {
    Token op= parser->tokens[parser->previous];
    Expr* expr = parsePrecedence(parser, PREC_UNARY);
    Expr* new = malloc(sizeof(Expr));
    new->type = EXPR_UNARY;
    new->unary.op = getOp(&op, false);
    new->unary.right = expr;
    return new;
}

Expr* binary(Parser* parser) {
    Token op = parser->tokens[parser->previous];
    Expr* expr = malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->binary.left = getLeft(parser);
    expr->binary.op = getOp(&op, true);
    ParseRule* rule = getRule(op.type);
    if (op.type == TOKEN_LEFT_PAREN) {
        expr->binary.right = parsePrecedence(parser, PREC_ASSIGNMENT);
        eat(parser, TOKEN_RIGHT_PAREN, "expected ')' after expression");
        return expr;
    } 
    if (op.type == TOKEN_LEFT_BRACKET) {
        expr->binary.right = parsePrecedence(parser, PREC_ASSIGNMENT);
        eat(parser, TOKEN_RIGHT_BRACKET, "expected ']' after expression");
        return expr;
    }
    expr->binary.right = parsePrecedence(parser, rule->precedence + 1);
    return expr;
}

// parse function declaration
static Decl* functionDecl(Parser* parser, Token type, Token name) {
    Decl* decl = malloc(sizeof(Decl));
    decl->type = DECL_FUNCTION;
    decl->function.returnType = tokenToString(type);
    decl->function.name = tokenToString(name);
    decl->function.parameters = NULL;
    decl->function.count = 0;
    if (!match (parser, TOKEN_RIGHT_PAREN)) {
        decl->function.parameters = malloc(sizeof(Param*) * 10);
        do {
            Token paramType = advance(parser);
            Token paramName = eat(parser, TOKEN_IDENTIFIER,
                                  "expected identifier after type in parameter");
            Param* param = malloc(sizeof(Param));
            param->type = tokenToString(paramType);
            param->name = tokenToString(paramName);
            decl->function.parameters[decl->function.count++] = param;
        } while (match(parser, TOKEN_COMMA));
        eat(parser, TOKEN_RIGHT_PAREN, "expected ')' after parameters");
    }
    decl->function.body = block(parser);
    return decl;
}

// parse declaration
Decl* declaration(Parser* parser) {
    if (match(parser, TOKEN_STRUCT)) {
        panic("struct declaration not implemented yet\n");
    }
    Token type = advance(parser);
    Token name =
        eat(parser, TOKEN_IDENTIFIER, "expected identifier after type in declaration");
    if (match(parser, TOKEN_LEFT_PAREN)) {
        return functionDecl(parser, type, name);
    } else {
        // variable declaration
        Decl* decl = malloc(sizeof(Decl));
        decl->type = DECL_VARIABLE;
        decl->variable.type = tokenToString(type);
        decl->variable.name = tokenToString(name);
        decl->variable.initializer = NULL;
        // non initialized variable
        if (match(parser, TOKEN_SEMICOLON)) {
            return decl;
        }
        eat(parser, TOKEN_EQUAL, "expected '=' after variable declaration");
        decl->variable.initializer = expression(parser);
        eat(parser, TOKEN_SEMICOLON, "expected ';' after variable declaration");
        return decl;
    }
}

/*
 * parse expression with given precedence.
 * precedence is used to determine which rule to use.
 * if the precedence of the current token is lower than the given precedence,
 * then the current token is not an operator, so we just return the current
 * token as an expression.
 * else we parse the expression according to the infix rule of the current
 * token.
 */
static Expr* parsePrecedence(Parser* parser, Precedence precedence) {
    Token previous = advance(parser);
    ParseFn prefixRule = preRule(parser)->prefix;
    if (prefixRule == NULL) {
        panic("line %d: expected expression, but get %s: type %s\n",
              previous.line, tokenToString(previous).chars,
              tokenType(previous.type).chars);
    }
    Expr* expr = prefixRule(parser);
    setLeft(parser, expr);

    while (precedence <= curRule(parser)->precedence) {
        Token current = advance(parser);
        ParseFn infixRule = getRule(current.type)->infix;
        assertMsg(infixRule != NULL, "infix rule should not be NULL");
        expr = infixRule(parser);
        setLeft(parser, expr);
    }

    return expr;
}

// parse expression
Expr* expression(Parser* parser) {
    return parsePrecedence(parser, PREC_ASSIGNMENT);
}

static Stmt* block(Parser* parser) {
    eat(parser, TOKEN_LEFT_BRACE, "expected '{' before block");
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_BLOCK;
    stmt->block.count = 0;
    stmt->block.statements = malloc(sizeof(Stmt*) * 10);
    while (!match(parser, TOKEN_RIGHT_BRACE)) {
        stmt->block.statements[stmt->block.count] = statement(parser);
        stmt->block.count++;
    }
    return stmt;
}

static Stmt* ifStmt(Parser* parser) {
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_IF;
    stmt->ifStmt.condition = expression(parser);
    stmt->ifStmt.thenBranch = statement(parser);
    stmt->ifStmt.elseBranch = NULL;
    if (match(parser, TOKEN_ELSE)) {
        stmt->ifStmt.elseBranch = statement(parser);
    }
    return stmt;
}

static Stmt* whileStmt(Parser* parser) {
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_WHILE;
    stmt->whileStmt.condition = expression(parser);
    stmt->whileStmt.body = block(parser);
    return stmt;
}

static Stmt* exprStmt(Parser* parser) {
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_EXPRESSION;
    stmt->expr.expression = expression(parser);
    return stmt;
}

static Stmt* returnStmt(Parser* parser) {
    eat(parser, TOKEN_RETURN, "expected 'return' before return statement");
    Stmt* stmt = malloc(sizeof(Stmt));
    stmt->type = STMT_RETURN;
    stmt->returnStmt.value = expression(parser);
    eat(parser, TOKEN_SEMICOLON, "expected ';' after return statement");
    return stmt;
}

// parse statement
Stmt* statement(Parser* parser) {
    switch (parser->tokens[parser->current].type) {
        case TOKEN_LEFT_BRACE:
            return block(parser);
        case TOKEN_IF:
            return ifStmt(parser);
        case TOKEN_WHILE:
            return whileStmt(parser);
        case TOKEN_RETURN:
            return returnStmt(parser);
        // TODO: type or struct declaration, not consider type alias
        case TOKEN_TYPENAME:
        case TOKEN_STRUCT: {
            Decl* decl = declaration(parser);
            Stmt* stmt = malloc(sizeof(Stmt));
            stmt->type = STMT_DECL;
            stmt->decl.decl = decl;
            return stmt;
        }
        // case TOKEN_BREAK:
        //     return breakStmt(parser);
        // case TOKEN_CONTINUE:
        //     return continueStmt(parser);
        default:
            return exprStmt(parser);
    }
}

// parse program
Program* parse(Token* tokens) {
    Parser* parser = malloc(sizeof(Parser));
    initParser(parser, tokens);
    Program* program = malloc(sizeof(Program));
    program->declarations = malloc(sizeof(Decl) * 1024);
    program->count = 0;
    while (parser->tokens[parser->current].type != TOKEN_EOF) {
        program->declarations[program->count] = declaration(parser);
        program->count++;
    }
    return program;
}
