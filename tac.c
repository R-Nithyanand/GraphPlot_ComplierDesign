#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tac.h"

static int tempCount = 0;
static char *derivH = NULL;
static char *derivXph = NULL;
static char *derivXmh = NULL;
static char *derivH2 = NULL;

void tac_reset(void) {
    tempCount = 0;
    if (derivH) {
        free(derivH);
        derivH = NULL;
    }
    if (derivXph) {
        free(derivXph);
        derivXph = NULL;
    }
    if (derivXmh) {
        free(derivXmh);
        derivXmh = NULL;
    }
}

static char *newTemp(void) {
    char buf[32];
    sprintf(buf, "t%d", tempCount++);
    return strdup(buf); 
}

/* Forward declarations */
static char *generateTAC_with_varname(ASTNode *node, FILE *out, const char *varname);

char *generateTAC(ASTNode *node, FILE *out) {
    if (!node) return NULL;

    switch (node->type) {
        case NODE_NUMBER: {
            char *buf = malloc(64);
            snprintf(buf, 64, "%.10g", node->value);
            return buf;
        }

        case NODE_VAR: {
            return strdup("x");
        }

        case NODE_IDENTIFIER: {
            return strdup(node->name);
        }

        case NODE_OP: {
            if (node->op == '~') { /* unary minus */
                char *v = generateTAC(node->left, out);
                char *t = newTemp();
                fprintf(out, "%s = - %s\n", t, v);
                free(v);
                return t;
            } else {
                char *L = generateTAC(node->left, out);
                char *R = generateTAC(node->right, out);
                char *t = newTemp();
                fprintf(out, "%s = %s %c %s\n", t, L, node->op, R);
                free(L);
                free(R);
                return t;
            }
        }

        case NODE_FUNC: {
            char *arg = generateTAC(node->left, out);
            char *t = newTemp();
            fprintf(out, "%s = %s(%s)\n", t, node->func, arg);
            free(arg);
            return t;
        }

        case NODE_FUNC2: {
            char *a1 = generateTAC(node->left, out);
            char *a2 = generateTAC(node->right, out);
            char *t = newTemp();
            fprintf(out, "%s = %s(%s, %s)\n", t, node->func, a1, a2);
            free(a1);
            free(a2);
            return t;
        }

        case NODE_DERIVATIVE: {
            /* Initialize derivative helpers only once */
            if (!derivH) {
                derivH = newTemp();
                fprintf(out, "%s = 1e-5\n", derivH);

                derivXph = newTemp();
                fprintf(out, "%s = x + %s\n", derivXph, derivH);

                derivXmh = newTemp();
                fprintf(out, "%s = x - %s\n", derivXmh, derivH);

                derivH2 = newTemp();
                fprintf(out, "%s = 2 * %s\n", derivH2, derivH);
            }

            char *arg1 = generateTAC_with_varname(node->left, out, derivXph);
            char *arg2 = generateTAC_with_varname(node->left, out, derivXmh);

            char *tdiff = newTemp();
            fprintf(out, "%s = %s - %s\n", tdiff, arg1, arg2);

            char *tout = newTemp();
            fprintf(out, "%s = %s / %s\n", tout, tdiff, derivH2);

            free(arg1);
            free(arg2);
            free(tdiff);

            return tout;
        }

        default:
            return NULL;
    }
}

static char *generateTAC_with_varname(ASTNode *node, FILE *out, const char *varname) {
    if (!node) return NULL;

    switch (node->type) {
        case NODE_NUMBER: {
            char *buf = malloc(64);
            snprintf(buf, 64, "%.10g", node->value);
            return buf;
        }

        case NODE_VAR: {
            return strdup(varname);
        }

        case NODE_IDENTIFIER: {
            return strdup(node->name);
        }

        case NODE_OP: {
            if (node->op == '~') {
                char *v = generateTAC_with_varname(node->left, out, varname);
                char *t = newTemp();
                fprintf(out, "%s = - %s\n", t, v);
                free(v);
                return t;
            } else {
                char *L = generateTAC_with_varname(node->left, out, varname);
                char *R = generateTAC_with_varname(node->right, out, varname);
                char *t = newTemp();
                fprintf(out, "%s = %s %c %s\n", t, L, node->op, R);
                free(L);
                free(R);
                return t;
            }
        }

        case NODE_FUNC: {
            char *arg = generateTAC_with_varname(node->left, out, varname);
            char *t = newTemp();
            fprintf(out, "%s = %s(%s)\n", t, node->func, arg);
            free(arg);
            return t;
        }

        case NODE_FUNC2: {
            char *a1 = generateTAC_with_varname(node->left, out, varname);
            char *a2 = generateTAC_with_varname(node->right, out, varname);
            char *t = newTemp();
            fprintf(out, "%s = %s(%s, %s)\n", t, node->func, a1, a2);
            free(a1);
            free(a2);
            return t;
        }

        case NODE_DERIVATIVE: {
            if (!derivH) {
                derivH = newTemp();
                fprintf(out, "%s = 1e-5\n", derivH);

                char *vph = newTemp();
                fprintf(out, "%s = %s + %s\n", vph, varname, derivH);
                derivXph = vph;

                char *vmh = newTemp();
                fprintf(out, "%s = %s - %s\n", vmh, varname, derivH);
                derivXmh = vmh;
                
                derivH2 = newTemp();
                fprintf(out, "%s = 2 * %s\n", derivH2, derivH);
            }

            char *arg1 = generateTAC_with_varname(node->left, out, derivXph);
            char *arg2 = generateTAC_with_varname(node->left, out, derivXmh);

            char *tdiff = newTemp();
            fprintf(out, "%s = %s - %s\n", tdiff, arg1, arg2);

            char *tout = newTemp();
            fprintf(out, "%s = %s / %s\n", tout, tdiff, derivH2);

            free(arg1);
            free(arg2);
            free(tdiff);

            return tout;
        }

        default:
            return NULL;
    }
}