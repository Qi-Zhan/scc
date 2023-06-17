#include "test.h"

static Program* parse_(const char* buffer) {
    Token* tokens = scanTokens(buffer);
    Program* program = parse(tokens);
    return program;
}

static void test_parse_var_def() {
    char* buffer = "int a;";
    Program* program = parse_(buffer);
    assert(program->count == 1);
    assert(program->declarations[0]->type == DECL_VARIABLE);
    assert(strcmp(program->declarations[0]->variable.type.chars, "int") == 0);
    assert(strcmp(program->declarations[0]->variable.name.chars, "a") == 0);
    char* buffer2 = "int a = 1;";
    Program* program2 = parse_(buffer2);
    assert(program2->count == 1);
    assert(program2->declarations[0]->type == DECL_VARIABLE);
    assert(program2->declarations[0]->variable.initializer->type ==
           EXPR_LITERAL);
    assert(program2->declarations[0]->variable.initializer->literal.type ==
           TYPE_INT);
}

static void test_parse_fun_def() {
    char* buffer = "int main(int i, int j) { return 0; }";
    Program* program = parse_(buffer);
    assert(program->count == 1);
    assert(program->declarations[0]->type == DECL_FUNCTION);
    assert(strcmp(program->declarations[0]->function.name.chars, "main") == 0);
    assert(strcmp(program->declarations[0]->function.returnType.chars, "int") ==
           0);
    printProgram(program);
}

// test pratt parser
static void test_parse_exp() {
    Parser* parser = malloc(sizeof(Parser));
    {
        char* buffer = "!1 + 2 * 3";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(expr->type == EXPR_BINARY);
        assert(strcmp(buf, "((! 1) + (2 * 3))") == 0);
    }
    {
        char* buffer = "-1 + 2";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(expr->type == EXPR_BINARY);
        assert(strcmp(buf, "((- 1) + 2)") == 0);
    }
    {
        char* buffer = "(-1 + 2) * 3 - -4";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(expr->type == EXPR_BINARY);
        assert(strcmp(buf, "((((- 1) + 2) * 3) - (- 4))") == 0);
    }
    {
        char* buffer = "1 + 2 + f . g . h * 3 * 4";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(strcmp(buf, "((1 + 2) + ((((f . g) . h) * 3) * 4))") == 0);
    }
    {
        char* buffer = "1 * * 2";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(strcmp(buf, "(1 * (* 2))") == 0);
    }
    {
        char* buffer = "a[1 + 2 * 3] = 4";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(strcmp(buf, "((a [] (1 + (2 * 3))) = 4)") == 0);
    }
    {
        char* buffer = "aac3(&b)";
        initParser(parser, scanTokens(buffer));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(strcmp(buf, "(aac3 call (& b))") == 0);
    }
}

void test_parse() {
    printf("Testing parse...\n");
    test_parse_exp();
    test_parse_var_def();
    test_parse_fun_def();
    printf("\033[0;32mAll tests passed!\033[0m\n");
}
