#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "commands.h"
#include "tac.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse();
extern ASTNode *root;
extern void yyrestart(FILE *input_file);

int cmd_let=0, cmd_def=0, cmd_ast=0, cmd_vars=0, cmd_funcs=0, cmd_show=0;
char show_func_name[50];
int  error_occurred = 0;

// Multi-function storage
#define MAX_MULTI_FUNCTIONS 10
ASTNode *multi_functions[MAX_MULTI_FUNCTIONS];
char multi_func_names[MAX_MULTI_FUNCTIONS][100];
int multi_func_count = 0;
int multi_mode = 0;  // 0 = single (advanced features), 1 = multi (simple plotting)

void print_banner() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║          Mathematical Expression Plotter v1.0             ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n\033[1;36mFeatures:\033[0m\n");
    printf("  • User-defined variables: \033[1;33mlet a = 5\033[0m\n");
    printf("  • User-defined functions: \033[1;33mdef f = x^2 + a\033[0m\n");
    printf("  • \033[1;35mThree-Address Code (TAC):\033[0m \033[1;33mtac\033[0m\n");
    printf("  • Constant folding optimization\n");
    printf("  • Two-argument functions: \033[1;33mmax(a,b)\033[0m, \033[1;33mmin(a,b)\033[0m\n");
    printf("  • AST visualization: \033[1;33mast <expr>\033[0m\n");
    printf("  • Error recovery & validation\n");
    printf("\n\033[1;36mModes:\033[0m\n");
    printf("  \033[1;32mSingle mode\033[0m: Advanced features (vars, funcs, ast) + immediate plotting\n");
    printf("  \033[1;32mMulti mode\033[0m:  Collect multiple expressions → type 'plot' to overlay\n");
    printf("\n\033[1;36mCommands:\033[0m\n");
    printf("  \033[1;32mmode\033[0m        - Toggle between single/multi mode\n");
    printf("  \033[1;32mvars\033[0m        - List all variables (single mode)\n");
    printf("  \033[1;32mfuncs\033[0m       - List all functions (single mode)\n");
    printf("  \033[1;32mtac\033[0m         - Show Three-Address Code (single mode & mulit mode) \n");
    printf("  \033[1;32mshow <name>\033[0m - Display function AST (single mode)\n");
    printf("  \033[1;32mlist\033[0m        - Show stored expressions (multi mode)\n");
    printf("  \033[1;32mplot\033[0m        - Plot all stored expressions (multi mode)\n");
    printf("  \033[1;32mclear\033[0m       - Clear stored expressions (multi mode)\n");
    printf("  \033[1;32mquit\033[0m        - Exit\n");
    printf("\n");
}

void print_mode_status() {
    if (multi_mode) {
        printf("\033[1;33m[Multi-Function Mode]\033[0m - Collect expressions, type 'plot' to visualize all\n");
    } else {
        printf("\033[1;33m[Single-Function Mode]\033[0m - Advanced features + immediate plotting\n");
    }
}

void list_multi_functions() {
    if (multi_func_count == 0) {
        printf("No functions stored.\n");
        return;
    }
    printf("\n\033[1;36mStored functions:\033[0m\n");
    for (int i = 0; i < multi_func_count; i++) {
        printf("  \033[1;33mf%d(x)\033[0m = %s\n", i, multi_func_names[i]);
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
    int has_error = 0;
    for (double x = x_min; x <= x_max; x += step) {
        double y = evaluate(node, x);
        if (!isnan(y) && !isinf(y)) {
            fprintf(f, "%lf %lf\n", x, y);
            points++;
        } else {
            has_error = 1;
        }
    }
    fclose(f);

    if (points == 0) {
        fprintf(stderr, "\033[1;31mError: No valid points to plot\033[0m\n");
        return;
    }

    if (has_error) {
        printf("\n\033[1;33mWarning: Some points skipped due to undefined values (NaN/Inf)\033[0m\n");
    }

    printf("\n[Generated %d data points from %.2f to %.2f]\n", points, x_min, x_max);
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
    printf("\n");
}

void plot_all_multi_functions(double x_min, double x_max, double step) {
    if (multi_func_count == 0) {
        printf("No functions to plot!\n");
        return;
    }

    // Generate data files for each function
    for (int i = 0; i < multi_func_count; i++) {
        char filename[30];
        sprintf(filename, "data%d.txt", i);
        FILE *f = fopen(filename, "w");
        if (!f) {
            fprintf(stderr, "Error: Cannot create %s\n", filename);
            continue;
        }
        for (double x = x_min; x <= x_max; x += step) {
            double y = evaluate(multi_functions[i], x);
            if (!isnan(y) && !isinf(y)) {
                fprintf(f, "%lf %lf\n", x, y);
            }
        }
        fclose(f);
    }

    printf("\nLaunching gnuplot with %d function%s...\n", 
           multi_func_count, multi_func_count > 1 ? "s" : "");

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
    for (int i = 0; i < multi_func_count; i++) {
        fprintf(gp, "'data%d.txt' with lines linewidth 2 linecolor rgb '%s' title 'f%d: %s'",
                i, colors[i % 7], i, multi_func_names[i]);
        if (i < multi_func_count - 1) {
            fprintf(gp, ", ");
        }
    }
    fprintf(gp, "\n");
    
    fflush(gp);
    pclose(gp);
    printf("Plot complete!\n\n");
}

void clear_multi_functions() {
    for (int i = 0; i < multi_func_count; i++) {
        freeAST(multi_functions[i]);
    }
    multi_func_count = 0;
    printf("All stored functions cleared.\n");
}

void toggle_mode() {
    multi_mode = !multi_mode;
    tac_reset();
    FILE *f = fopen("tac.txt", "w");
    fclose(f);
    printf("\n");
    if (multi_mode) {
        printf("★ Switched to \033[1;33mMULTI-FUNCTION\033[0m mode\n");
        printf("  → Enter multiple expressions, then type 'plot' to overlay them\n");
        printf("  → Commands: list, plot, tac, clear\n");
    } else {
        printf("★ Switched to \033[1;33mSINGLE-FUNCTION\033[0m mode\n");
        printf("  → Use advanced features: let, def, vars, funcs, show, ast, tac\n");
        printf("  → Each expression plots immediately\n");
        // Clear stored multi functions when switching to single mode
        if (multi_func_count > 0) {
            printf("  → Clearing %d stored function%s\n", multi_func_count, 
                   multi_func_count > 1 ? "s" : "");
            clear_multi_functions();
        }
    }
    printf("\n");
}

ASTNode* parse_expression_from_string(const char *input) {
    char expr_with_newline[260];
    snprintf(expr_with_newline, sizeof(expr_with_newline), "%s\n", input);
    
    YY_BUFFER_STATE buffer = yy_scan_string(expr_with_newline);
    root = NULL;
    error_occurred = 0;
    
    int result = yyparse();
    yy_delete_buffer(buffer);

    if (error_occurred) {
        printf("\033[1;31mSyntax error. Please try again.\033[0m\n");
        return NULL;
    }

    if (result != 0 || !root) {
        printf("\033[1;31mParse error. Please try again.\033[0m\n");
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
        // Print appropriate prompt based on mode
        if (multi_mode) {
            if (multi_func_count < MAX_MULTI_FUNCTIONS) {
                printf("f%d(x) = ", multi_func_count);
            } else {
                printf("\033[1;32m>\033[0m ");
            }
        } else {
            printf("\033[1;32m>\033[0m ");
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

        // ===== COMMON COMMANDS =====
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            clear_multi_functions();
            printf("Goodbye!\n");
            break;
        }

        if (strcmp(input, "mode") == 0) {
            toggle_mode();
            continue;
        }

        // ===== MULTI-MODE COMMANDS =====
        if (multi_mode) {
            if (strcmp(input, "list") == 0) {
                list_multi_functions();
                continue;
            }

            if (strcmp(input, "clear") == 0) {
                clear_multi_functions();
                continue;
            }

            if (strcmp(input, "plot") == 0) {
                plot_all_multi_functions(x_min, x_max, step);
                continue;
            }

            if (strcmp(input, "tac") == 0) {
                FILE *f = fopen("tac.txt", "r");
                if (!f){
                    perror("Error opening file");
                }
                else{
                    int c;
                    while ((c = fgetc(f)) != EOF){
                    putchar(c);
                    }
                    fclose(f);
                }
                continue;
            }

            // Multi-mode: Check for invalid commands
            if (strcmp(input, "vars") == 0 || strcmp(input, "funcs") == 0 || 
                strncmp(input, "show ", 5) == 0 || strncmp(input, "let ", 4) == 0 || 
                strncmp(input, "def ", 4) == 0 || strncmp(input, "ast ", 4) == 0) {
                printf("\033[1;33mCommand '%s' only available in single-function mode.\033[0m\n", input);
                printf("Type 'mode' to switch.\n");
                continue;
            }

            // Parse and store expression in multi-mode
            ASTNode *parsed_node = parse_expression_from_string(input);
            if (!parsed_node) {
                continue;  // Error already printed
            }

            if (multi_func_count >= MAX_MULTI_FUNCTIONS) {
                printf("Maximum functions (%d) reached! Type 'plot', 'clear', or 'quit'.\n", 
                       MAX_MULTI_FUNCTIONS);
                freeAST(parsed_node);
                continue;
            }
            
            FILE *tacOut = fopen("tac.txt", "a");
            if (!tacOut) {
                perror("tac.txt");
                freeAST(root);
                continue;
            }
            generateTAC(root, tacOut);
            fclose(tacOut);

            // Optimize and store
            parsed_node = optimizeAST(parsed_node);
            strcpy(multi_func_names[multi_func_count], input);
            multi_functions[multi_func_count] = parsed_node;
            multi_func_count++;

            printf("\033[1;32m✓\033[0m Function f%d(x) added. ", multi_func_count - 1);
            if (multi_func_count < MAX_MULTI_FUNCTIONS) {
                printf("Enter more or type 'plot' to visualize.\n");
            } else {
                printf("Max reached! Type 'plot' to visualize.\n");
            }
            continue;
        }

        // ===== SINGLE-MODE (Advanced features) =====
        // Check for multi-mode-only commands
        if (strcmp(input, "list") == 0 || strcmp(input, "clear") == 0) {
            printf("\033[1;33mCommand '%s' only available in multi-function mode.\033[0m\n", input);
            printf("Type 'mode' to switch.\n");
            continue;
        }

        if (strcmp(input, "plot") == 0) {
            printf("Already in single-function mode (auto-plotting).\n");
            printf("Type 'mode' to switch to multi-function mode.\n");
            continue;
        }

        // Use standard parser for single mode (supports let, def, vars, funcs, show, ast)
        root = NULL;
        error_occurred = 0;

        // Need to feed input through parser
        YY_BUFFER_STATE buffer = yy_scan_string(strcat(input, "\n"));
        int result = yyparse();
        yy_delete_buffer(buffer);

        if (error_occurred) {
            printf("\033[1;31mSyntax error. Please try again.\033[0m\n");
            continue;
        }

        if (result != 0) {
            continue;
        }

        // If we got an expression (not a command), plot it
        if (root) {
            root = optimizeAST(root);
            
            if (!validateAST(root)) {
                freeAST(root);
                continue;
            }

            
            tac_reset();
            FILE *tacOut = fopen("tac.txt", "w");
            if (!tacOut) {
                perror("tac.txt");
                freeAST(root);
                continue;
            }
            generateTAC(root, tacOut);
            fclose(tacOut);

            plot_single_function(root, x_min, x_max, step);
            freeAST(root);
        }
    }

    return 0;
}