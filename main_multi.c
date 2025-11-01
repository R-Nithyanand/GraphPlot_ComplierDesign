#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Flex/Bison functions for parsing from string
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse();
extern ASTNode *root;

#define MAX_FUNCTIONS 10
ASTNode *functions[MAX_FUNCTIONS];
char func_names[MAX_FUNCTIONS][100];
int func_count = 0;

void print_banner() {
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║  Multi-Function Expression Plotter v1.0    ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\nSupported operations:\n");
    printf("  Operators: +, -, *, /, ^\n");
    printf("  Functions: sin, cos, tan, exp, log, sqrt, abs, ln\n");
    printf("             asin, acos, atan, sinh, cosh, tanh\n");
    printf("  Constants: pi, e\n");
    printf("  Variable:  x\n");
    printf("\nCommands:\n");
    printf("  'plot'  - Plot all entered functions\n");
    printf("  'clear' - Clear all functions\n");
    printf("  'list'  - Show all functions\n");
    printf("  'quit'  - Exit\n");
    printf("\n");
}

void list_functions() {
    if (func_count == 0) {
        printf("No functions entered yet.\n");
        return;
    }
    printf("\nCurrent functions:\n");
    for (int i = 0; i < func_count; i++) {
        printf("  f%d(x) = %s\n", i, func_names[i]);
    }
    printf("\n");
}

void plot_all_functions(double x_min, double x_max, double step) {
    if (func_count == 0) {
        printf("No functions to plot!\n");
        return;
    }

    // Generate data files for each function
    for (int i = 0; i < func_count; i++) {
        char filename[30];
        sprintf(filename, "data%d.txt", i);
        FILE *f = fopen(filename, "w");
        if (!f) {
            fprintf(stderr, "Error: Cannot create %s\n", filename);
            continue;
        }
        for (double x = x_min; x <= x_max; x += step) {
            double y = evaluate(functions[i], x);
            fprintf(f, "%lf %lf\n", x, y);
        }
        fclose(f);
    }

    printf("\nLaunching gnuplot with %d function%s...\n", 
           func_count, func_count > 1 ? "s" : "");

    // Build gnuplot command
    FILE *gp = popen("gnuplot -persist", "w");
    if (!gp) {
        fprintf(stderr, "Error: Cannot launch gnuplot\n");
        return;
    }

    fprintf(gp, "set title 'Multiple Functions Plot' font ',14'\n");
    fprintf(gp, "set xlabel 'x' font ',12'\n");
    fprintf(gp, "set ylabel 'f(x)' font ',12'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key top left\n");
    
    // Define colors for different functions
    const char *colors[] = {"#0072BD", "#D95319", "#EDB120", "#7E2F8E", 
                           "#77AC30", "#4DBEEE", "#A2142F"};
    
    fprintf(gp, "plot ");
    for (int i = 0; i < func_count; i++) {
        fprintf(gp, "'data%d.txt' with lines linewidth 2 linecolor rgb '%s' title 'f%d(x)'",
                i, colors[i % 7], i);
        if (i < func_count - 1) {
            fprintf(gp, ", ");
        }
    }
    fprintf(gp, "\n");
    
    fflush(gp);
    pclose(gp);
    printf("Plot complete!\n");
}

void clear_functions() {
    for (int i = 0; i < func_count; i++) {
        freeAST(functions[i]);
    }
    func_count = 0;
    printf("All functions cleared.\n");
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
    printf("(Use: ./graph_compiler_multi <min> <max> <step> to change)\n\n");

    while (1) {
        if (func_count < MAX_FUNCTIONS) {
            printf("f%d(x) = ", func_count);
        } else {
            printf("> ");
        }
        fflush(stdout);
        
        char input[256];
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Check for commands
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            clear_functions();
            break;
        }
        if (strcmp(input, "plot") == 0) {
            plot_all_functions(x_min, x_max, step);
            continue;
        }
        if (strcmp(input, "clear") == 0) {
            clear_functions();
            continue;
        }
        if (strcmp(input, "list") == 0) {
            list_functions();
            continue;
        }

        // Check if max functions reached
        if (func_count >= MAX_FUNCTIONS) {
            printf("Maximum functions (%d) reached! Type 'plot', 'clear', or 'quit'.\n", MAX_FUNCTIONS);
            continue;
        }

        // Parse the expression using yy_scan_string
        // Add newline for parser
        char expr_with_newline[260];
        snprintf(expr_with_newline, sizeof(expr_with_newline), "%s\n", input);
        
        YY_BUFFER_STATE buffer = yy_scan_string(expr_with_newline);
        root = NULL;
        int result = yyparse();
        yy_delete_buffer(buffer);

        if (result != 0 || !root) {
            printf("Parse error. Please try again.\n");
            continue;
        }

        // Validate the AST
        if (!validateAST(root)) {
            freeAST(root);
            continue;
        }

        // Store the expression
        strcpy(func_names[func_count], input);
        functions[func_count] = root;
        func_count++;

        printf("✓ Function f%d(x) added. ", func_count - 1);
        if (func_count < MAX_FUNCTIONS) {
            printf("Enter more or type 'plot' to visualize.\n");
        } else {
            printf("Max reached! Type 'plot' to visualize.\n");
        }
    }

    return 0;
}