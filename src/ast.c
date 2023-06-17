#include "ast.h"

char* OptoString(Op op) {
    switch (op) {
        case OP_ADD:
            return "+";
        case OP_SUB:
            return "-";
        case OP_MUL:
            return "*";
        case OP_DIV:
            return "/";
        case OP_AND:
            return "&&";
        case OP_OR:
            return "||";
        case OP_NOT:
            return "!";
        case OP_NEG:
            return "-";
        case OP_EQ:
            return "==";
        case OP_NEQ:
            return "!=";
        case OP_LT:
            return "<";
        case OP_LTE:
            return "<=";
        case OP_GT:
            return ">";
        case OP_GTE:
            return ">=";
        case OP_AT:
            return "@";
        case OP_DOT:
            return ".";
        case OP_REF:
            return "&";
        case OP_DEREF:
            return "*";
        case OP_SUBSCRIPT:
            return "[]";
        case OP_ASSIGN:
            return "=";
        case OP_CALL:
            return "call";
        default:
            panic("line %d: unknown operator\n", 0);
    }
    return NULL;
}

void printProgram(Program* program) {
    for (int i = 0; i < program->count; i++) {
        printDecl(program->declarations[i]);
    }
}

void printExpr(Expr* expr) {
    switch (expr->type) {
        case EXPR_BINARY:
            printf("(");
            printExpr(expr->binary.left);
            printf(" %s ", OptoString(expr->binary.op));
            printExpr(expr->binary.right);
            printf(")");
            break;
        case EXPR_UNARY:
            printf("(");
            printf("%s", OptoString(expr->unary.op));
            printExpr(expr->unary.right);
            printf(")");
            break;
        case EXPR_LITERAL:
            switch (expr->literal.type) {
                case TYPE_INT:
                    printf("%d", expr->literal.value.intVal);
                    break;
                case TYPE_FLOAT:
                    printf("%f", expr->literal.value.floatVal);
                    break;
                case TYPE_STRING:
                    printf("\"%s\"", expr->literal.value.stringVal.chars);
                    break;
                case TYPE_BOOL:
                    printf("%s",
                           expr->literal.value.boolVal ? "true" : "false");
                    break;
                case TYPE_IDENTIFIER:
                    printf("%s", expr->literal.value.idVal.chars);
                    break;
                default:
                    panic("unknown literal type %d\n", expr->literal.type);
            }
            break;
        case EXPR_VARIABLE:
            printf("%s", expr->variable.name.chars);
            break;
        case EXPR_CALL:
            printf("%s(", expr->call.name.chars);
            for (int i = 0; i < expr->call.argumentsCount; i++) {
                printExpr(expr->call.arguments[i]);
                if (i < expr->call.argumentsCount - 1) {
                    printf(", ");
                }
            }
            printf(")");
            break;
        default:
            panic("unknown expression type %d\n", expr->type);
    }
}

void printStmt(Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            printExpr(stmt->expr.expression);
            break;
        case STMT_BLOCK:
            printf("{\n");
            for (int i = 0; i < stmt->block.count; i++) {
                printStmt(stmt->block.statements[i]);
            }
            printf("}");
            break;
        case STMT_IF:
            printf("if (");
            printExpr(stmt->ifStmt.condition);
            printf(") ");
            printStmt(stmt->ifStmt.thenBranch);
            if (stmt->ifStmt.elseBranch) {
                printf(" else ");
                printStmt(stmt->ifStmt.elseBranch);
            }
            break;
        case STMT_WHILE:
            printf("while (");
            printExpr(stmt->whileStmt.condition);
            printf(") ");
            printStmt(stmt->whileStmt.body);
            break;
        case STMT_RETURN:
            printf("return ");
            printExpr(stmt->returnStmt.value);
            break;
        default:
            panic("unknown statement type %d\n", stmt->type);
    }
}

void printDecl(Decl* decl) {
    switch (decl->type) {
        case DECL_FUNCTION:
            printf("fn %s(", decl->function.name.chars);
            for (int i = 0; i < decl->function.count; i++) {
                printf("%s", decl->function.parameters[i]->name.chars);
                if (i < decl->function.count - 1) {
                    printf(", ");
                }
            }
            printf(") ");
            printStmt(decl->function.body);
            break;
        case DECL_VARIABLE:
            printf("var %s", decl->variable.name.chars);
            if (decl->variable.initializer) {
                printf(" = ");
                printExpr(decl->variable.initializer);
            }
            break;
        case DECL_STRUCT:
            printf("struct %s {\n", decl->record.name.chars);
            panic("structs not implemented yet\n");
            printf("}");
            break;
        default:
            panic("unknown declaration type %d\n", decl->type);
    }
}

Op getOp(Token* token, bool isBinary) {
    switch (token->type) {
        case TOKEN_PLUS:
            return OP_ADD;
        case TOKEN_MINUS:
            if (isBinary) {
                return OP_SUB;
            } else {
                return OP_NEG;
            }
        case TOKEN_STAR:
            if (isBinary) {
                return OP_MUL;
            } else {
                return OP_DEREF;
            }
        case TOKEN_SLASH:
            return OP_DIV;
        case TOKEN_EQUAL_EQUAL:
            return OP_EQ;
        case TOKEN_BANG_EQUAL:
            return OP_NEQ;
        case TOKEN_LESS:
            return OP_LT;
        case TOKEN_LESS_EQUAL:
            return OP_LTE;
        case TOKEN_GREATER:
            return OP_GT;
        case TOKEN_GREATER_EQUAL:
            return OP_GTE;
        case TOKEN_AT:
            return OP_AT;
        case TOKEN_DOT:
            return OP_DOT;
        case TOKEN_LEFT_BRACKET:
            return OP_SUBSCRIPT;
        case TOKEN_EQUAL:
            return OP_ASSIGN;
        case TOKEN_BANG:
            return OP_NOT;
        case TOKEN_LEFT_PAREN:
            return OP_CALL;
        case TOKEN_REF:
            return OP_REF;
        default:
            panic("line %d: unknown operator %d\n", token->line, token->type);
    }    
    return OP_ERROR;
}

char* sprintExpr(Expr* expr) {
    char buffer[1024];
    switch (expr->type) {
        case EXPR_BINARY:
            sprintf(buffer, "(%s %s %s)", sprintExpr(expr->binary.left),
                    OptoString(expr->binary.op), sprintExpr(expr->binary.right));
            break;
        case EXPR_UNARY:
            sprintf(buffer, "(%s %s)", OptoString(expr->unary.op),
                    sprintExpr(expr->unary.right));
            break;
        case EXPR_LITERAL:
            switch (expr->literal.type) {
                case TYPE_INT:
                    sprintf(buffer, "%d", expr->literal.value.intVal);
                    break;
                case TYPE_FLOAT:
                    sprintf(buffer, "%f", expr->literal.value.floatVal);
                    break;
                case TYPE_STRING:
                    sprintf(buffer, "\"%s\"", expr->literal.value.stringVal.chars);
                    break;
                case TYPE_BOOL:
                    sprintf(buffer, "%s",
                            expr->literal.value.boolVal ? "true" : "false");
                    break;
                default:
                    panic("unknown literal type %d\n", expr->literal.type);
            }
            break;
        case EXPR_VARIABLE:
            sprintf(buffer, "%s", expr->variable.name.chars);
            break;
        case EXPR_CALL:
            sprintf(buffer, "%s(", expr->call.name.chars);
            for (int i = 0; i < expr->call.argumentsCount; i++) {
                sprintf(buffer, "%s%s", buffer, sprintExpr(expr->call.arguments[i]));
                if (i < expr->call.argumentsCount - 1) {
                    sprintf(buffer, "%s, ", buffer);
                }
            }
            sprintf(buffer, "%s)", buffer);
            break;
        default:
            panic("unknown expression type %d\n", expr->type);
    }
    return strdup(buffer);
}