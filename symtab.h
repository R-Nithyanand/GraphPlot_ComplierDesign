#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

#define MAX_VARS  100
#define MAX_FUNCS 50

typedef struct {
    char   name[50];
    double value;
} Variable;

typedef struct {
    char      name[50];
    ASTNode  *ast;
} Function;

/* exported tables (used by parser & evaluator) */
extern Variable variables[MAX_VARS];
extern int      var_count;
extern Function functions[MAX_FUNCS];
extern int      func_count;

/* lookup / store */
double*   lookupVariable(const char *name);
void      storeVariable(const char *name, double value);
ASTNode*  lookupFunction(const char *name);
void      storeFunction(const char *name, ASTNode *ast);
void      listVariables();
void      listFunctions();
void      showFunction(const char *name);

#endif /* SYMTAB_H */