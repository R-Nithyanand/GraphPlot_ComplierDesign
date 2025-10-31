#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ast.h"

ASTNode* createNumberNode(double value) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_NUMBER; n->value = value; n->left = n->right = NULL;
    return n;
}

ASTNode* createVarNode() {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_VAR; n->left = n->right = NULL;
    return n;
}

ASTNode* createOpNode(char op, ASTNode *left, ASTNode *right) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_OP; n->op = op; n->left = left; n->right = right;
    return n;
}

ASTNode* createFuncNode(const char *func, ASTNode *child) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_FUNC;
    strcpy(n->func, func);
    n->left = child; n->right = NULL;
    return n;
}

double evaluate(ASTNode *node, double x) {
    if (!node) return 0;

    switch (node->type) {
        case NODE_NUMBER: return node->value;
        case NODE_VAR: return x;
        case NODE_OP:
            switch (node->op) {
                case '+': return evaluate(node->left, x) + evaluate(node->right, x);
                case '-': return evaluate(node->left, x) - evaluate(node->right, x);
                case '*': return evaluate(node->left, x) * evaluate(node->right, x);
                case '/': return evaluate(node->left, x) / evaluate(node->right, x);
                case '^': return pow(evaluate(node->left, x), evaluate(node->right, x));
                case '~': return -evaluate(node->left, x);
            }
            break;
        case NODE_FUNC:
            if (strcmp(node->func, "sin") == 0) return sin(evaluate(node->left, x));
            if (strcmp(node->func, "cos") == 0) return cos(evaluate(node->left, x));
            if (strcmp(node->func, "tan") == 0) return tan(evaluate(node->left, x));
            if (strcmp(node->func, "exp") == 0) return exp(evaluate(node->left, x));
            if (strcmp(node->func, "log") == 0) return log(evaluate(node->left, x));
            if (strcmp(node->func, "sqrt") == 0) return sqrt(evaluate(node->left, x));
            if (strcmp(node->func, "abs") == 0) return fabs(evaluate(node->left, x));
            if (strcmp(node->func, "ln") == 0) return log(evaluate(node->left, x));
            if (strcmp(node->func, "asin") == 0) return asin(evaluate(node->left, x));
            if (strcmp(node->func, "acos") == 0) return acos(evaluate(node->left, x));
            if (strcmp(node->func, "atan") == 0) return atan(evaluate(node->left, x));
            if (strcmp(node->func, "sinh") == 0) return sinh(evaluate(node->left, x));
            if (strcmp(node->func, "cosh") == 0) return cosh(evaluate(node->left, x));
            if (strcmp(node->func, "tanh") == 0) return tanh(evaluate(node->left, x));
            break;
    }
    return 0;
}

void freeAST(ASTNode *node) {
    if (!node) return;
    freeAST(node->left);
    freeAST(node->right);
    free(node);
}

int validateAST(ASTNode *node) {
    if (!node) return 1;
    
    // Check for division by zero with constant
    if (node->type == NODE_OP && node->op == '/') {
        if (node->right && node->right->type == NODE_NUMBER && 
            node->right->value == 0.0) {
            fprintf(stderr, "Error: Division by zero detected\n");
            return 0;
        }
    }
    
    // Check for log of negative/zero
    if (node->type == NODE_FUNC && strcmp(node->func, "log") == 0) {
        if (node->left && node->left->type == NODE_NUMBER && 
            node->left->value <= 0) {
            fprintf(stderr, "Error: log of non-positive number\n");
            return 0;
        }
    }
    
    return validateAST(node->left) && validateAST(node->right);
}