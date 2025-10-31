#ifndef AST_H
#define AST_H

typedef enum { NODE_NUMBER, NODE_VAR, NODE_OP, NODE_FUNC } NodeType;

typedef struct ASTNode {
    NodeType type;
    double value;
    char op;
    char func[10];
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode* createNumberNode(double value);
ASTNode* createVarNode();
ASTNode* createOpNode(char op, ASTNode *left, ASTNode *right);
ASTNode* createFuncNode(const char *func, ASTNode *child);
double evaluate(ASTNode *node, double x);
void freeAST(ASTNode *node);

#endif
