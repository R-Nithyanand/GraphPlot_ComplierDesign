#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {
    NODE_NUMBER,
    NODE_VAR,
    NODE_OP,
    NODE_FUNC,
    NODE_IDENTIFIER,
    NODE_FUNC2,
    NODE_DERIVATIVE
} NodeType;

typedef struct ASTNode {
    NodeType type;
    double   value;     // for NUMBER
    char     op;        // for OP   ('+', '-', '*', '/', '^', '~')
    char     func[20];  // for FUNC / FUNC2
    char     name[50];  // for IDENTIFIER
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *arg2;   // only for FUNC2
} ASTNode;

/* ---- creation ----------------------------------------------------------- */
ASTNode* createNumberNode(double value);
ASTNode* createVarNode(void);
ASTNode* createIdentifierNode(const char *name);
ASTNode* createOpNode(char op, ASTNode *left, ASTNode *right);
ASTNode* createFuncNode(const char *func, ASTNode *child);
ASTNode* createFunc2Node(const char *func, ASTNode *arg1, ASTNode *arg2);
ASTNode *createDerivative(const char *func, ASTNode *child);

/* ---- evaluation / utilities -------------------------------------------- */
double   evaluate(ASTNode *node, double x);
void     freeAST(ASTNode *node);
int      validateAST(ASTNode *node);
void     printAST(ASTNode *node, int indent);
void     printASTPretty(ASTNode *node, const char *prefix, int is_left);
ASTNode* optimizeAST(ASTNode *node);

#endif /* AST_H */