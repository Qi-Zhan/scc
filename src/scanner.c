//---------------------- Scanner ----------------------
#include "scanner.h"

#include "util.h"

void printToken(Token* token) {
    switch (token->type) {
        case TOKEN_STRUCT:
        case TOKEN_IF:
        case TOKEN_ELSE:
        case TOKEN_WHILE:
        case TOKEN_RETURN:
            // print aligned
            printf("keyword  ");
            break;
        case TOKEN_STRING:
        case TOKEN_NUMBER:
            printf("literal  ");
            break;
        case TOKEN_IDENTIFIER:
            printf("id       ");
            break;
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_EQUAL:
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG:
        case TOKEN_BANG_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_AT:
        case TOKEN_DOT:
            printf("operator ");
            break;
        case TOKEN_ERROR:
            printf("error    ");
            break;
        case TOKEN_TYPENAME:
            printf("type     ");
            break;
        default:
            printf("token    ");
    }
    printf("%.*s\n", token->length, token->start);
}

void printTokens(Token* tokens) {
    while (tokens->type != TOKEN_EOF) {
        printToken(tokens);
        tokens++;
    }
}

void initScanner(Scanner* scanner, const char* source) {
    scanner->start = source;
    scanner->current = source;
    scanner->line = 1;
}

String tokenToString(Token token) {
    String str;
    str.length = token.length;
    str.chars = malloc(str.length + 1);
    memcpy(str.chars, token.start, str.length);
    str.chars[str.length] = '\0';
    return str;
}

static char advance(Scanner* scanner) { return *scanner->current++; }

static bool isAtEnd(Scanner* scanner) { return *scanner->current == '\0'; }

char peek(Scanner* scanner) { return *scanner->current; }

char peekNext(Scanner* scanner) {
    if (isAtEnd(scanner)) return '\0';
    return scanner->current[1];
}

void skipWhitespace(Scanner* scanner) {
    while (true) {
        char c = peek(scanner);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(scanner);
                break;
            case '\n':
                scanner->line++;
                advance(scanner);
                break;
            case '/':
                if (peekNext(scanner) == '/') {
                    // A comment goes until the end of the line.
                    while (peek(scanner) != '\n' && !isAtEnd(scanner))
                        advance(scanner);
                    break;
                } else if (peekNext(scanner) == '*') {
                    advance(scanner);
                    while (true) {
                        if (peek(scanner) == '*' && peekNext(scanner) == '/') {
                            advance(scanner);
                            advance(scanner);
                            break;
                        }
                        if (peek(scanner) == '\n') scanner->line++;
                        if (isAtEnd(scanner)) {
                            panic("unclosed multiline comment at line %d\n",
                                  scanner->line);
                        }
                        advance(scanner);
                    }
                    break;
                } else {
                    return;
                }
            default:
                return;
        }
    }
}

static bool match(char expected, Scanner* scanner) {
    if (isAtEnd(scanner)) return false;
    if (*scanner->current != expected) return false;
    scanner->current++;
    return true;
}

Token makeToken(TokenType type, Scanner* scanner) {
    Token token;
    token.type = type;
    token.start = scanner->start;
    token.length = (int)(scanner->current - scanner->start);
    token.line = scanner->line;
    return token;
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

// check if the current id is a keyword
void convertKeyword(Token* token) {
    switch (token->length) {
        case 2:
            if (memcmp(token->start, "if", 2) == 0) token->type = TOKEN_IF;
        case 3:
            if (memcmp(token->start, "int", 3) == 0)
                token->type = TOKEN_TYPENAME;
            if (memcmp(token->start, "for", 3) == 0) 
                token->type = TOKEN_FOR;
        case 4:
            if (memcmp(token->start, "else", 4) == 0) 
                token->type = TOKEN_ELSE;
            if (memcmp(token->start, "bool", 4) == 0)
                token->type = TOKEN_TYPENAME;
            if (memcmp(token->start, "char", 4) == 0)
                token->type = TOKEN_TYPENAME;
            if (memcmp(token->start, "void", 4) == 0)
                token->type = TOKEN_TYPENAME;
            break;
        case 5:
            if (memcmp(token->start, "while", 5) == 0)
                token->type = TOKEN_WHILE;
            if (memcmp(token->start, "float", 5) == 0)
                token->type = TOKEN_TYPENAME;
            break;
        case 6:
            if (memcmp(token->start, "return", 6) == 0)
                token->type = TOKEN_RETURN;
            if (memcmp(token->start, "struct", 6) == 0)
                token->type = TOKEN_STRUCT;
            break;
    }
}

Token scanToken(Scanner* scanner) {
    skipWhitespace(scanner);
    scanner->start = scanner->current;
    if (isAtEnd(scanner)) return makeToken(TOKEN_EOF, scanner);
    char c = advance(scanner);
    if (isAlpha(c)) {
        while (isAlpha(peek(scanner)) || isDigit(peek(scanner)))
            advance(scanner);
        Token token = makeToken(TOKEN_IDENTIFIER, scanner);
        convertKeyword(&token);
        return token;
    }
    if (isDigit(c)) {
        while (isDigit(peek(scanner))) advance(scanner);
        if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
            advance(scanner);
            while (isDigit(peek(scanner))) advance(scanner);
        }
        return makeToken(TOKEN_NUMBER, scanner);
    }
    switch (c) {
        case '(':
            return makeToken(TOKEN_LEFT_PAREN, scanner);
        case ')':
            return makeToken(TOKEN_RIGHT_PAREN, scanner);
        case '{':
            return makeToken(TOKEN_LEFT_BRACE, scanner);
        case '}':
            return makeToken(TOKEN_RIGHT_BRACE, scanner);
        case '[':
            return makeToken(TOKEN_LEFT_BRACKET, scanner);
        case ']':
            return makeToken(TOKEN_RIGHT_BRACKET, scanner);
        case ';':
            return makeToken(TOKEN_SEMICOLON, scanner);
        case ',':
            return makeToken(TOKEN_COMMA, scanner);
        case '.':
            return makeToken(TOKEN_DOT, scanner);
        case '-':
            return makeToken(TOKEN_MINUS, scanner);
        case '+':
            return makeToken(TOKEN_PLUS, scanner);
        case '/':
            return makeToken(TOKEN_SLASH, scanner);
        case '*':
            return makeToken(TOKEN_STAR, scanner);
        case '@':
            return makeToken(TOKEN_AT, scanner);
        case '&':
            if (match('&', scanner)) {
                return makeToken(TOKEN_AND, scanner);
            } else {
                return makeToken(TOKEN_REF, scanner);
            }
        case '|':
            if (match('|', scanner)) {
                return makeToken(TOKEN_OR, scanner);
            } else {
                panic("invalid character '%c' at line %d\n", c, scanner->line);
            }
        case '!': {
            if (match('=', scanner)) {
                return makeToken(TOKEN_BANG_EQUAL, scanner);
            } else {
                return makeToken(TOKEN_BANG, scanner);
            }
        }
        case '=': {
            if (match('=', scanner)) {
                return makeToken(TOKEN_EQUAL_EQUAL, scanner);
            } else {
                return makeToken(TOKEN_EQUAL, scanner);
            }
        }
        case '<': {
            if (match('=', scanner)) {
                return makeToken(TOKEN_LESS_EQUAL, scanner);
            } else {
                return makeToken(TOKEN_LESS, scanner);
            }
        }
        case '>': {
            if (match('=', scanner)) {
                return makeToken(TOKEN_GREATER_EQUAL, scanner);
            } else {
                return makeToken(TOKEN_GREATER, scanner);
            }
        }
        case '"': {
            while (peek(scanner) != '"' && !isAtEnd(scanner)) {
                if (peek(scanner) == '\n') scanner->line++;
                advance(scanner);
            }
            if (isAtEnd(scanner)) {
                return makeToken(TOKEN_ERROR, scanner);
            }
            advance(scanner);
            return makeToken(TOKEN_STRING, scanner);
        }
        default:
            return makeToken(TOKEN_ERROR, scanner);
    }
    return makeToken(TOKEN_ERROR, scanner);
}

Token* scanTokens(const char* source) {
    Token* tokens = (Token*)malloc(sizeof(Token) * 100);
    Scanner scanner;
    initScanner(&scanner, source);
    int i = 0;
    while (!isAtEnd(&scanner)) {
        tokens[i++] = scanToken(&scanner);
    }
    tokens[i] = makeToken(TOKEN_EOF, &scanner);
    return tokens;
}

String tokenType(TokenType type) {
    switch (type) {
        case TOKEN_LEFT_PAREN:
            return makeString(")", 1);
        case TOKEN_RIGHT_PAREN:
            return makeString("(", 1);
        case TOKEN_LEFT_BRACE:
            return makeString("{", 1);
        case TOKEN_RIGHT_BRACE:
            return makeString("}", 1);
        case TOKEN_LEFT_BRACKET:
            return makeString("[", 1);
        case TOKEN_RIGHT_BRACKET:
            return makeString("]", 1);
        case TOKEN_COMMA:
            return makeString(",", 1);
        case TOKEN_DOT:
            return makeString(".", 1);
        case TOKEN_MINUS:
            return makeString("-", 1);
        case TOKEN_PLUS:
            return makeString("+", 1);
        case TOKEN_SEMICOLON:
            return makeString(";", 1);
        case TOKEN_SLASH:
            return makeString("/", 1);
        case TOKEN_STAR:
            return makeString("*", 1);
        case TOKEN_AT:
            return makeString("@", 1);
        case TOKEN_BANG:
            return makeString("!", 1);
        case TOKEN_BANG_EQUAL:
            return makeString("!=", 2);
        case TOKEN_EQUAL:
            return makeString("=", 1);
        case TOKEN_EQUAL_EQUAL:
            return makeString("==", 2);
        case TOKEN_GREATER:
            return makeString(">", 1);
        case TOKEN_GREATER_EQUAL:
            return makeString(">=", 2);
        case TOKEN_LESS:
            return makeString("<", 1);
        case TOKEN_LESS_EQUAL:
            return makeString("<=", 2);
        case TOKEN_IDENTIFIER:
            return makeString("identifier", 10);
        case TOKEN_STRING:
            return makeString("string", 6);
        case TOKEN_NUMBER:
            return makeString("number", 6);
        case TOKEN_AND:
            return makeString("&&", 2);
        case TOKEN_REF:
            return makeString("&", 1);
        case TOKEN_RETURN:
            return makeString("return", 6);
        default:
            panic("Unknown token type %d", type);
            break;
    }
    return makeString("unknown", 7);
}