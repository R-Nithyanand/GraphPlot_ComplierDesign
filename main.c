#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yyparse();
extern ASTNode *root;
extern FILE *yyin;
extern void yyrestart(FILE *input_file);

void print_banner() {
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║   Mathematical Expression Plotter v1.0     ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\nSupported operations:\n");
    printf("  Operators: +, -, *, /, ^\n");
    printf("  Functions: sin, cos, tan, exp, log, sqrt, abs, ln\n");
    printf("             asin, acos, atan, sinh, cosh, tanh\n");
    printf("  Constants: pi, e\n");
    printf("  Variable:  x\n");
    printf("  Commands:  quit, exit\n");
    printf("\nExamples:\n");
    printf("  > x^2 + 2*x + 1\n");
    printf("  > sin(x) * cos(x)\n");
    printf("  > exp(-x^2)\n");
    printf("  > sqrt(abs(x))\n");
    printf("\n");
}

void plot_function(ASTNode *root, double x_min, double x_max, double step) {
    FILE *f = fopen("data.txt", "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot create data file\n");
        return;
    }

    int points = 0;
    for (double x = x_min; x <= x_max; x += step) {
        double y = evaluate(root, x);
        fprintf(f, "%lf %lf\n", x, y);
        points++;
    }
    fclose(f);

    printf("\n[Generated %d data points from %.2f to %.2f]\n", 
           points, x_min, x_max);
    printf("Launching gnuplot...\n");

    // Enhanced gnuplot command with better styling
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "gnuplot -p -e \""
        "set title 'f(x) Plot' font ',14'; "
        "set xlabel 'x' font ',12'; "
        "set ylabel 'f(x)' font ',12'; "
        "set grid; "
        "set key top left; "
        "plot 'data.txt' with lines linewidth 2 linecolor rgb '#0072BD' title 'f(x)'"
        "\"");
    
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Warning: gnuplot command failed. Is gnuplot installed?\n");
    }
}

int main(int argc, char *argv[]) {
    double x_min = -10.0;
    double x_max = 10.0;
    double step = 0.1;

    // Parse command line arguments for range
    if (argc >= 3) {
        x_min = atof(argv[1]);
        x_max = atof(argv[2]);
    }
    if (argc >= 4) {
        step = atof(argv[3]);
    }

    print_banner();
    printf("Plot range: [%.2f, %.2f] with step %.3f\n", x_min, x_max, step);
    printf("(Use: ./graph_compiler <min> <max> <step> to change)\n\n");

    while (1) {
        printf("f(x) = ");
        fflush(stdout);
        
        root = NULL;
        int result = yyparse();
        
        if (result != 0) {
            printf("Parse error. Please try again.\n");
            // Restart the lexer to clear any bad state
            yyrestart(stdin);
            continue;
        }

        if (!root) {
            continue;
        }

        // Validate before plotting
        if (!validateAST(root)) {
            freeAST(root);
            continue;
        }

        plot_function(root, x_min, x_max, step);
        freeAST(root);
        printf("\n");
    }

    return 0;
}