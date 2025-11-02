#include "ast.h"
#include "symtab.h"


ASTNode* createNumberNode(double value) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_NUMBER;
    n->value = value;
    n->left = n->right = n->arg2 = NULL;
    return n;
}

ASTNode* createVarNode() {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_VAR;
    n->left = n->right = n->arg2 = NULL;
    return n;
}

ASTNode* createIdentifierNode(const char *name) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_IDENTIFIER;
    strncpy(n->name, name, 49);
    n->name[49] = '\0';
    n->left = n->right = n->arg2 = NULL;
    return n;
}

ASTNode* createOpNode(char op, ASTNode *left, ASTNode *right) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_OP;
    n->op = op;
    n->left = left;
    n->right = right;
    n->arg2 = NULL;
    return n;
}

ASTNode* createFuncNode(const char *func, ASTNode *child) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_FUNC;
    strncpy(n->func, func, 19);
    n->func[19] = '\0';
    n->left = child;
    n->right = n->arg2 = NULL;
    return n;
}

ASTNode* createFunc2Node(const char *func, ASTNode *arg1, ASTNode *arg2) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_FUNC2;
    strncpy(n->func, func, 19);
    n->func[19] = '\0';
    n->left = arg1;
    n->right = arg2;
    n->arg2 = NULL;
    return n;
}

ASTNode *createDerivative(const char *func, ASTNode *child) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = NODE_DERIVATIVE;
    strncpy(n->func, func, 19);
    n->func[19] = '\0';
    n->left = child;
    n->right = n->arg2 = NULL;
    return n;
}

double derivative(ASTNode *expr, double x) {
    double h = 1e-5;
    return (evaluate(expr, x + h) - evaluate(expr, x - h));
}

double evaluate(ASTNode *node, double x) {
    if (!node) return 0;

    switch (node->type) {
        case NODE_NUMBER:
            return node->value;
        
        case NODE_VAR:
            return x;
        
        case NODE_IDENTIFIER: {
            double *val = lookupVariable(node->name);
            if (val) return *val;
            
            ASTNode *func = lookupFunction(node->name);
            if (func) return evaluate(func, x);
            
            fprintf(stderr, "\033[1;31mError: Undefined identifier '%s'\033[0m\n", node->name);
            return NAN;
        }
        
        case NODE_OP:
            switch (node->op) {
                case '+': return evaluate(node->left, x) + evaluate(node->right, x);
                case '-': return evaluate(node->left, x) - evaluate(node->right, x);
                case '*': return evaluate(node->left, x) * evaluate(node->right, x);
                case '/': {
                    double denom = evaluate(node->right, x);
                    if (fabs(denom) < 1e-10) return NAN;
                    return evaluate(node->left, x) / denom;
                }
                case '^': return pow(evaluate(node->left, x), evaluate(node->right, x));
                case '~': return -evaluate(node->left, x);
            }
            break;
        
        case NODE_FUNC: {
            double arg = evaluate(node->left, x);
            if (strcmp(node->func, "sin") == 0) return sin(arg);
            if (strcmp(node->func, "cos") == 0) return cos(arg);
            if (strcmp(node->func, "tan") == 0) return tan(arg);
            if (strcmp(node->func, "exp") == 0) return exp(arg);
            if (strcmp(node->func, "log") == 0) return (arg > 0) ? log10(arg) : NAN;
            if (strcmp(node->func, "sqrt") == 0) return (arg >= 0) ? sqrt(arg) : NAN;
            if (strcmp(node->func, "abs") == 0) return fabs(arg);
            if (strcmp(node->func, "ln") == 0) return (arg > 0) ? log(arg) : NAN;
            if (strcmp(node->func, "asin") == 0) return asin(arg);
            if (strcmp(node->func, "acos") == 0) return acos(arg);
            if (strcmp(node->func, "atan") == 0) return atan(arg);
            if (strcmp(node->func, "sinh") == 0) return sinh(arg);
            if (strcmp(node->func, "cosh") == 0) return cosh(arg);
            if (strcmp(node->func, "tanh") == 0) return tanh(arg);
            if (strcmp(node->func, "ceil") == 0) return ceil(arg);
            if (strcmp(node->func, "floor") == 0) return floor(arg);
            break;
        }
        
        case NODE_FUNC2: {
            double arg1 = evaluate(node->left, x);
            double arg2 = evaluate(node->right, x);
            if (strcmp(node->func, "max") == 0) return (arg1 > arg2) ? arg1 : arg2;
            if (strcmp(node->func, "min") == 0) return (arg1 < arg2) ? arg1 : arg2;
            break;
        }

        case NODE_DERIVATIVE:
            return derivative(node->left, x);
    }
    return 0;
}

void freeAST(ASTNode *node) {
    if (!node) return;
    freeAST(node->left);
    freeAST(node->right);
    freeAST(node->arg2);
    free(node);
}

int validateAST(ASTNode *node) {
    if (!node) return 1;

    // Check for constant division by zero
    if (node->type == NODE_OP && node->op == '/') {
        if (node->right && node->right->type == NODE_NUMBER &&
            fabs(node->right->value) < 1e-10) {
            fprintf(stderr, "\033[1;31mError: Division by constant zero detected\033[0m\n");
            return 0;
        }
    }

    // Check for log of non-positive constant
    if (node->type == NODE_FUNC &&
        (strcmp(node->func, "log") == 0 || strcmp(node->func, "ln") == 0)) {
        if (node->left && node->left->type == NODE_NUMBER &&
            node->left->value <= 0) {
            fprintf(stderr, "\033[1;31mError: log/ln of non-positive constant (%.2f)\033[0m\n",
                    node->left->value);
            return 0;
        }
    }
    
    // Check for sqrt of negative constant
    if (node->type == NODE_FUNC && strcmp(node->func, "sqrt") == 0) {
        if (node->left && node->left->type == NODE_NUMBER &&
            node->left->value < 0) {
            fprintf(stderr, "\033[1;31mError: sqrt of negative constant (%.2f)\033[0m\n",
                    node->left->value);
            return 0;
        }
    }

    return validateAST(node->left) && validateAST(node->right) && validateAST(node->arg2);
}

void printAST(ASTNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER: %.4f\n", node->value);
            break;
        case NODE_VAR:
            printf("VAR: x\n");
            break;
        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->name);
            break;
        case NODE_OP:
            printf("OP: %c\n", node->op);
            printAST(node->left, indent + 1);
            printAST(node->right, indent + 1);
            break;
        case NODE_FUNC:
            printf("FUNC: %s\n", node->func);
            printAST(node->left, indent + 1);
            break;
        case NODE_FUNC2:
            printf("FUNC2: %s\n", node->func);
            printAST(node->left, indent + 1);
            printAST(node->right, indent + 1);
            break;
        case NODE_DERIVATIVE:
            printf("DERIV: %s\n", node->func);
            printAST(node->left, indent + 1);
    }
}

void printASTPretty(ASTNode *node, const char *prefix, int is_left) {
    if (!node) return;

    printf("%s", prefix);
    printf("%s", is_left ? "├── " : "└── ");

    switch (node->type) {
        case NODE_NUMBER:
            printf("\033[1;33mNUMBER\033[0m: %.4f\n", node->value);
            break;
        case NODE_VAR:
            printf("\033[1;32mVAR\033[0m: x\n");
            break;
        case NODE_IDENTIFIER:
            printf("\033[1;36mID\033[0m: %s\n", node->name);
            break;
        case NODE_OP: {
            printf("\033[1;31mOP\033[0m: %c\n", node->op);
            if (node->left || node->right) {
                char *new_prefix = malloc(strlen(prefix) + 10);
                if (node->left) {
                    sprintf(new_prefix, "%s%s", prefix, is_left ? "│   " : "    ");
                    printASTPretty(node->left, new_prefix, node->right != NULL);
                }
                if (node->right) {
                    sprintf(new_prefix, "%s%s", prefix, is_left ? "│   " : "    ");
                    printASTPretty(node->right, new_prefix, 0);
                }
                free(new_prefix);
            }
            break;
        }
        case NODE_FUNC: {
            printf("\033[1;35mFUNC\033[0m: %s\n", node->func);
            if (node->left) {
                char *new_prefix = malloc(strlen(prefix) + 10);
                sprintf(new_prefix, "%s%s", prefix, is_left ? "│   " : "    ");
                printASTPretty(node->left, new_prefix, 0);
                free(new_prefix);
            }
            break;
        }
        case NODE_FUNC2: {
            printf("\033[1;35mFUNC2\033[0m: %s\n", node->func);
            if (node->left || node->right) {
                char *new_prefix = malloc(strlen(prefix) + 10);
                if (node->left) {
                    sprintf(new_prefix, "%s%s", prefix, is_left ? "│   " : "    ");
                    printASTPretty(node->left, new_prefix, node->right != NULL);
                }
                if (node->right) {
                    sprintf(new_prefix, "%s%s", prefix, is_left ? "│   " : "    ");
                    printASTPretty(node->right, new_prefix, 0);
                }
                free(new_prefix);
            }
            break;
        }
        case NODE_DERIVATIVE: {
            printf("\033[1;35mDERIV\033[0m: %s\n", node->func);
            if (node->left) {
                char *new_prefix = malloc(strlen(prefix) + 10);
                sprintf(new_prefix, "%s%s", prefix, is_left ? "│   " : "    ");
                printASTPretty(node->left, new_prefix, 0);
                free(new_prefix);
            }
            break;
        }
    }
}

ASTNode* optimizeAST(ASTNode *node) {
    if (!node) return NULL;

    // Recursively optimize children first
    node->left = optimizeAST(node->left);
    node->right = optimizeAST(node->right);
    node->arg2 = optimizeAST(node->arg2);

    // Constant folding for operations
    if (node->type == NODE_OP) {
        if (node->left && node->left->type == NODE_NUMBER &&
            node->right && node->right->type == NODE_NUMBER) {
            double result = 0;
            int can_fold = 1;

            switch (node->op) {
                case '+': result = node->left->value + node->right->value; break;
                case '-': result = node->left->value - node->right->value; break;
                case '*': result = node->left->value * node->right->value; break;
                case '/':
                    if (fabs(node->right->value) > 1e-10) {
                        result = node->left->value / node->right->value;
                    } else can_fold = 0;
                    break;
                case '^': result = pow(node->left->value, node->right->value); break;
                default: can_fold = 0;
            }

            if (can_fold) {
                freeAST(node->left);
                freeAST(node->right);
                node->type = NODE_NUMBER;
                node->value = result;
                node->left = node->right = NULL;
            }
        }
        // Unary minus optimization
        else if (node->op == '~' && node->left && node->left->type == NODE_NUMBER) {
            double result = -node->left->value;
            freeAST(node->left);
            node->type = NODE_NUMBER;
            node->value = result;
            node->left = NULL;
        }
    }

    // Constant folding for single-arg functions
    if (node->type == NODE_FUNC && node->left && node->left->type == NODE_NUMBER) {
        double arg = node->left->value;
        double result = 0;
        int can_fold = 1;

        if (strcmp(node->func, "sin") == 0) result = sin(arg);
        else if (strcmp(node->func, "cos") == 0) result = cos(arg);
        else if (strcmp(node->func, "abs") == 0) result = fabs(arg);
        else if (strcmp(node->func, "sqrt") == 0 && arg >= 0) result = sqrt(arg);
        else if (strcmp(node->func, "exp") == 0) result = exp(arg);
        else if (strcmp(node->func, "tan") == 0) result = tan(arg);
        else can_fold = 0;

        if (can_fold) {
            freeAST(node->left);
            node->type = NODE_NUMBER;
            node->value = result;
            node->left = NULL;
        }
    }

    // Constant folding for two-arg functions
    if (node->type == NODE_FUNC2 && 
        node->left && node->left->type == NODE_NUMBER &&
        node->right && node->right->type == NODE_NUMBER) {
        double result = 0;
        if (strcmp(node->func, "max") == 0) {
            result = (node->left->value > node->right->value) ? node->left->value : node->right->value;
        } else if (strcmp(node->func, "min") == 0) {
            result = (node->left->value < node->right->value) ? node->left->value : node->right->value;
        }
        freeAST(node->left);
        freeAST(node->right);
        node->type = NODE_NUMBER;
        node->value = result;
        node->left = node->right = NULL;
    }

    return node;
}