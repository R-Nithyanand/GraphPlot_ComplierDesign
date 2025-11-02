#include "symtab.h"
#include "ast.h"

Variable variables[MAX_VARS];
int      var_count = 0;
Function functions[MAX_FUNCS];
int      func_count = 0;

/* ---------- variables --------------------------------------------------- */
double* lookupVariable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i].value;
        }
    }
    return NULL;
}

void storeVariable(const char *name, double value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            variables[i].value = value;
            return;
        }
    }
    if (var_count < MAX_VARS) {
        strncpy(variables[var_count].name, name, 49);
        variables[var_count].name[49] = '\0';
        variables[var_count].value = value;
        var_count++;
    }
}

ASTNode* lookupFunction(const char *name) {
    for (int i = 0; i < func_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return functions[i].ast;
        }
    }
    return NULL;
}

void storeFunction(const char *name, ASTNode *ast) {
    for (int i = 0; i < func_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            freeAST(functions[i].ast);
            functions[i].ast = ast;
            return;
        }
    }
    if (func_count < MAX_FUNCS) {
        strncpy(functions[func_count].name, name, 49);
        functions[func_count].name[49] = '\0';
        functions[func_count].ast = ast;
        func_count++;
    }
}

void listVariables() {
    if (var_count == 0) {
        printf("No variables defined.\n");
        return;
    }
    printf("\n\033[1;36mVariables:\033[0m\n");
    for (int i = 0; i < var_count; i++) {
        printf("  \033[1;33m%s\033[0m = %.4f\n", variables[i].name, variables[i].value);
    }
    printf("\n");
}

void listFunctions() {
    if (func_count == 0) {
        printf("No functions defined.\n");
        return;
    }
    printf("\n\033[1;36mFunctions:\033[0m\n");
    for (int i = 0; i < func_count; i++) {
        printf("  \033[1;33m%s\033[0m(x)\n", functions[i].name);
    }
    printf("\n");
}

void showFunction(const char *name) {
    ASTNode *ast = lookupFunction(name);
    if (!ast) {
        printf("Function '%s' not found.\n", name);
        return;
    }
    printf("\n\033[1;36m╔════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;36m║ Function '%s' AST:                    ║\033[0m\n", name);
    printf("\033[1;36m╚════════════════════════════════════════╝\033[0m\n\n");
    printASTPretty(ast, "", 0);
    printf("\n");
}