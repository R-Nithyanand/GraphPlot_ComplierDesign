#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include "ast.h"

void tac_reset(void);

char *generateTAC(ASTNode *node, FILE *out);

#endif
