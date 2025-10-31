#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yyparse();
extern ASTNode *root;

int main() {
    while (1) {
        printf("\nEnter an expression in x (or 'quit' to exit):\n> ");
        root = NULL;

        yyparse();  // parses until newline (thanks to YYACCEPT)

        if (!root)
            continue;

        FILE *f = fopen("data.txt", "w");
        for (double x = -10; x <= 10; x += 0.1) {
            double y = evaluate(root, x);
            fprintf(f, "%lf %lf\n", x, y);
        }
        fclose(f);

        printf("Plotting...\n");
        system("gnuplot -p -e \"plot 'data.txt' with lines title 'f(x)'\"");
        freeAST(root);
    }
    return 0;
}
