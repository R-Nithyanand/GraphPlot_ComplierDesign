#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Flex/Bison functions
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse();
extern ASTNode *root;
extern void yyrestart(FILE *input_file);

#define MAX_FUNCTIONS 10
ASTNode *functions[MAX_FUNCTIONS];
char func_names[MAX_FUNCTIONS][100];
int func_count = 0;
int multi_mode = 0;  // Mode flag: 0 = single, 1 = multi

void print_banner() {
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║   Mathematical Expression Plotter v2.0     ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\nSupported operations:\n");
    printf("  Operators: +, -, *, /, ^\n");
    printf("  Functions: sin, cos, tan, exp, log, sqrt, abs, ln\n");
    printf("             asin, acos, atan, sinh, cosh, tanh\n");
    printf("  Constants: pi, e\n");
    printf("  Variable:  x\n");
    printf("\nModes:\n");
    printf("  Single mode: Enter expression → Plot immediately\n");
    printf("  Multi mode:  Collect expressions → Type 'plot' to show all\n");
    printf("\nCommands:\n");
    printf("  'mode'  - Toggle between single/multi mode\n");
    printf("  'plot'  - Plot all functions (multi mode only)\n");
    printf("  'clear' - Clear all functions (multi mode only)\n");
    printf("  'list'  - Show all functions (multi mode only)\n");
    printf("  'quit'  - Exit\n");
    printf("\n");
}

void print_mode_status() {
    if (multi_mode) {
        printf("[Multi-Function Mode] - Collect functions, type 'plot' to visualize\n");
    } else {
        printf("[Single-Function Mode] - Immediate plotting after each expression\n");
    }
}

void list_functions() {
    if (func_count == 0) {
        printf("No functions stored.\n");
        return;
    }
    printf("\nStored functions:\n");
    for (int i = 0; i < func_count; i++) {
        printf("  f%d(x) = %s\n", i, func_names[i]);
    }
    printf("\n");
}

void plot_single_function(ASTNode *node, double x_min, double x_max, double step) {
    FILE *f = fopen("data.txt", "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot create data file\n");
        return;
    }

    int points = 0;
    for (double x = x_min; x <= x_max; x += step) {
        double y = evaluate(node, x);
        fprintf(f, "%lf %lf\n", x, y);
        points++;
    }
    fclose(f);

    printf("\n[Generated %d data points from %.2f to %.2f]\n", 
           points, x_min, x_max);
    printf("Launching gnuplot...\n");

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
        fprintf(gp, "'data%d.txt' with lines linewidth 2 linecolor rgb '%s' title 'f%d(x): %s'",
                i, colors[i % 7], i, func_names[i]);
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

void toggle_mode() {
    multi_mode = !multi_mode;
    printf("\n");
    if (multi_mode) {
        printf("★ Switched to MULTI-FUNCTION mode\n");
        printf("  → Enter multiple expressions, then type 'plot' to visualize all\n");
    } else {
        printf("★ Switched to SINGLE-FUNCTION mode\n");
        printf("  → Each expression plots immediately\n");
        // Clear stored functions when switching to single mode
        if (func_count > 0) {
            printf("  → Clearing %d stored function%s\n", func_count, func_count > 1 ? "s" : "");
            clear_functions();
        }
    }
    printf("\n");
}

ASTNode* parse_expression(const char *input) {
    char expr_with_newline[260];
    snprintf(expr_with_newline, sizeof(expr_with_newline), "%s\n", input);
    
    YY_BUFFER_STATE buffer = yy_scan_string(expr_with_newline);
    root = NULL;
    int result = yyparse();
    yy_delete_buffer(buffer);

    if (result != 0 || !root) {
        printf("Parse error. Please try again.\n");
        return NULL;
    }

    if (!validateAST(root)) {
        freeAST(root);
        return NULL;
    }

    return root;
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
    print_mode_status();
    printf("\n");

    while (1) {
        // Print appropriate prompt
        if (multi_mode && func_count < MAX_FUNCTIONS) {
            printf("f%d(x) = ", func_count);
        } else if (multi_mode) {
            printf("> ");
        } else {
            printf("f(x) = ");
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
            printf("Goodbye!\n");
            break;
        }

        if (strcmp(input, "mode") == 0) {
            toggle_mode();
            continue;
        }

        if (strcmp(input, "list") == 0) {
            if (multi_mode) {
                list_functions();
            } else {
                printf("'list' command only available in multi-function mode. Type 'mode' to switch.\n");
            }
            continue;
        }

        if (strcmp(input, "clear") == 0) {
            if (multi_mode) {
                clear_functions();
            } else {
                printf("'clear' command only available in multi-function mode. Type 'mode' to switch.\n");
            }
            continue;
        }

        if (strcmp(input, "plot") == 0) {
            if (multi_mode) {
                plot_all_functions(x_min, x_max, step);
            } else {
                printf("Already in single-function mode (auto-plotting). Type 'mode' to switch to multi.\n");
            }
            continue;
        }

        // Parse the expression
        ASTNode *parsed_node = parse_expression(input);
        if (!parsed_node) {
            continue;  // Error already printed
        }

        // Handle based on mode
        if (multi_mode) {
            // Multi-function mode: store the function
            if (func_count >= MAX_FUNCTIONS) {
                printf("Maximum functions (%d) reached! Type 'plot', 'clear', or 'quit'.\n", MAX_FUNCTIONS);
                freeAST(parsed_node);
                continue;
            }

            strcpy(func_names[func_count], input);
            functions[func_count] = parsed_node;
            func_count++;

            printf("✓ Function f%d(x) added. ", func_count - 1);
            if (func_count < MAX_FUNCTIONS) {
                printf("Enter more or type 'plot' to visualize.\n");
            } else {
                printf("Max reached! Type 'plot' to visualize.\n");
            }
        } else {
            // Single-function mode: plot immediately
            plot_single_function(parsed_node, x_min, x_max, step);
            freeAST(parsed_node);
            printf("\n");
        }
    }

    return 0;
}