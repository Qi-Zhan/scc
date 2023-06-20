#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "test.h"
#include "util.h"

//---------------------- Common Macros ----------------------
#ifndef DUMMY

// print every token
#define DEBUG_PRINT_TOKEN true
// print the AST
#define PRINT_AST true
// print the IR
#define EMIT_IR true
// print the ASM
#define EMIT_ASM true

#define TEST true

#define BOOTSTRAP false

#if BOOTSTRAP
void read() { printf("read() is not implemented in bootstrap mode\n"); }
#else
void read() { printf("read() is implemented in non-bootstrap mode\n"); }
#endif
#endif

//---------------------- Assembly----------------------

//---------------------- Pipeline----------------------

void compile(const char* buffer) {
    // scan
    Token* tokens = scanTokens(buffer);
    int i = 0;
    while (tokens[i].type != TOKEN_EOF) {
        if (DEBUG_PRINT_TOKEN) printToken(&tokens[i]);
        if (tokens[i].type == TOKEN_ERROR) {
            panic("TOEN_ERROR: %s\n", tokens[i].start);
        }
        i++;
    }
    // parse
    Program* program = parse(tokens);
    if (PRINT_AST) printProgram(program);
    // semantic analysis

    // ir gen

    // asm gen
}

static void repl() {
    char line[1024];
    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        compile(line);
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        panic("Could not open file \"%s\".\n", path);
    }
    fseek(file, 0L, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(filesize + 1);
    if (buffer == NULL) {
        panic("Not enough memory to read \"%s\".\n", path);
    }
    size_t bytes_read = fread(buffer, sizeof(char), filesize, file);
    if (bytes_read < filesize) {
        panic("Could not read file \"%s\".\n", path);
    }
    buffer[bytes_read] = '\0';
    fclose(file);
    return buffer;
}

static void runFile(char* filename) {
    char* buffer = readFile(filename);
    compile(buffer);
}

//---------------------- Main--------------------------
int main(int argc, char* argv[]) {
    if (TEST) test_parse();
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    return 0;
}