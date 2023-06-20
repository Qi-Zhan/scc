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

static void test_parse_stmt() {
    Parser* parser = malloc(sizeof(Parser));
    // if stmt
    {
        char* buffer = "if (a == 1) { return 1; }";
        initParser(parser, scanTokens(buffer));
        Stmt* stmt = statement(parser);
        assert(stmt->type == STMT_IF);
        assert(stmt->ifStmt.condition->type == EXPR_BINARY);
        assert(stmt->ifStmt.thenBranch->type == STMT_BLOCK);
        assert(stmt->ifStmt.elseBranch == NULL);
    }
    // if else
    {
        char buffer[] = "if (a == 1) { return 1; } else { return 2; }";
        initParser(parser, scanTokens(buffer));
        Stmt* stmt = statement(parser);
        assert(stmt->type == STMT_IF);
        assert(stmt->ifStmt.condition->type == EXPR_BINARY);
        assert(stmt->ifStmt.thenBranch->type == STMT_BLOCK);
        assert(stmt->ifStmt.elseBranch->type == STMT_BLOCK);
    }
    // while
    {
        char buffer[] = "while (a == 1) { return 1; }";
        initParser(parser, scanTokens(buffer));
        Stmt* stmt = statement(parser);
        assert(stmt->type == STMT_WHILE);
        assert(stmt->whileStmt.condition->type == EXPR_BINARY);
        assert(stmt->whileStmt.body->type == STMT_BLOCK);
    }
}

static void test_parse_fun_def() {
    char* buffer = "int main(int i, int j) { int c = i * j + 3; return c; }";
    Program* program = parse_(buffer);
    assert(program->count == 1);
    assert(program->declarations[0]->type == DECL_FUNCTION);
    assert(strcmp(program->declarations[0]->function.name.chars, "main") == 0);
    assert(strcmp(program->declarations[0]->function.returnType.chars, "int") ==
           0);
}

const char* test_parse_exp_buf[] = {
    "!1 + 2 * 3", 
    "(-1 + 2) * 3 - -4", 
    "1 + 2 + f . g . h * 3 * 4",
    "1 * * 2",
    "a[1 + 2 * 3] = 4",  
    "aac3(&b)",
    "cal(add(a,b), minus(a,b))"
};

const char* test_parse_exp_result[] = {
    "((! 1) + (2 * 3))",
    "((((- 1) + 2) * 3) - (- 4))",
    "((1 + 2) + ((((f . g) . h) * 3) * 4))",
    "(1 * (* 2))",
    "((a [] (1 + (2 * 3))) = 4)",
    "aac3((& b))",
    "cal(add(a, b), minus(a, b))"
};

// test pratt parser
static void test_parse_exp() {
    Parser* parser = malloc(sizeof(Parser));
    for (size_t i = 0; i < sizeof(test_parse_exp_buf) / sizeof(char*); i++) {
        initParser(parser, scanTokens(test_parse_exp_buf[i]));
        Expr* expr = expression(parser);
        char* buf = sprintExpr(expr);
        assert(strcmp(buf, test_parse_exp_result[i]) == 0);
    }
}

void test_parse() {
    printf("Testing parse...\n");
    test_parse_exp();
    test_parse_var_def();
    test_parse_fun_def();
    test_parse_stmt();
    printf("\033[0;32mAll unit tests passed!\033[0m\n");
}
