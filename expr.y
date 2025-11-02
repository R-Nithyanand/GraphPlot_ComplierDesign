%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "commands.h"

ASTNode *root;
int yylex(void);
int yyerror(const char *s);

%}

%union {
    double   dval;
    ASTNode *node;
    char    *sval;
}

%token <dval> NUMBER
%token <sval> IDENTIFIER
%token VAR SIN COS TAN EXP LOG SQRT ABS LN ASIN ACOS ATAN SINH COSH TANH MAX MIN
%token CEIL FLOOR      
%token DERIV
%token LET DEF PLOT AST_CMD VARS FUNCS SHOW QUIT CLEAR LIST
%token EQ NEQ LE GE

%type <node> expr
%type <sval> ident

%left  EQ NEQ '<' '>' LE GE
%left  '+' '-'
%left  '*' '/'
%right '^'
%right UMINUS

%%

input:
    /* empty */
  | input line
;

line:
    '\n'                    { /* empty line */ }
  | statement '\n'          { return 0; }
  | error '\n'              { 
        yyerrok; 
        error_occurred = 1;
    }
;

statement:
      VARS {
          listVariables();
      }
    | FUNCS {
          listFunctions();
      }
    | SHOW ident {
          showFunction($2);
          free($2);
      }
    | LET ident '=' expr { 
          double val = evaluate($4, 0);
          storeVariable($2, val); 
          printf("Variable '\033[1;33m%s\033[0m' = %.4f\n", $2, val);
          freeAST($4);
          free($2);
      }
    | DEF ident '=' expr {
          storeFunction($2, $4);
          printf("Function '\033[1;33m%s\033[0m' defined\n", $2);
          free($2);
      }
    | AST_CMD expr {
          ASTNode *optimized = optimizeAST($2);
          printf("\n\033[1;36m╔════════════════════════════════════════╗\033[0m\n");
          printf("\033[1;36m║  Abstract Syntax Tree (optimized)      ║\033[0m\n");
          printf("\033[1;36m╚════════════════════════════════════════╝\033[0m\n\n");
          printASTPretty(optimized, "", 0);
          printf("\n");
          freeAST(optimized);
      }
    | expr { 
          root = $1; 
      }
    | QUIT { exit(0); }
;

ident: IDENTIFIER { $$ = $1; }
;

expr:
      expr '+' expr { $$ = createOpNode('+', $1, $3); }
    | expr '-' expr { $$ = createOpNode('-', $1, $3); }
    | expr '*' expr { $$ = createOpNode('*', $1, $3); }
    | expr '/' expr { $$ = createOpNode('/', $1, $3); }
    | expr '^' expr { $$ = createOpNode('^', $1, $3); }
    | '-' expr %prec UMINUS { $$ = createOpNode('~', $2, NULL); }
    | '(' expr ')' { $$ = $2; }
    | NUMBER { $$ = createNumberNode($1); }
    | VAR { $$ = createVarNode(); }
    | IDENTIFIER { $$ = createIdentifierNode($1); free($1); }
    | DERIV '(' expr ')'  { $$ = createDerivative("derivative", $3); }
    | SIN '(' expr ')' { $$ = createFuncNode("sin", $3); }
    | COS '(' expr ')' { $$ = createFuncNode("cos", $3); }
    | TAN '(' expr ')' { $$ = createFuncNode("tan", $3); }
    | EXP '(' expr ')' { $$ = createFuncNode("exp", $3); }
    | LOG '(' expr ')' { $$ = createFuncNode("log", $3); }
    | SQRT '(' expr ')' { $$ = createFuncNode("sqrt", $3); }
    | ABS '(' expr ')' { $$ = createFuncNode("abs", $3); }
    | LN '(' expr ')' { $$ = createFuncNode("ln", $3); }
    | ASIN '(' expr ')' { $$ = createFuncNode("asin", $3); }
    | ACOS '(' expr ')' { $$ = createFuncNode("acos", $3); }
    | ATAN '(' expr ')' { $$ = createFuncNode("atan", $3); }
    | SINH '(' expr ')' { $$ = createFuncNode("sinh", $3); }
    | COSH '(' expr ')' { $$ = createFuncNode("cosh", $3); }
    | TANH '(' expr ')' { $$ = createFuncNode("tanh", $3); }
    | CEIL '(' expr ')'  { $$ = createFuncNode("ceil", $3); }  
    | FLOOR '(' expr ')' { $$ = createFuncNode("floor", $3); } 
    | MAX '(' expr ',' expr ')' { $$ = createFunc2Node("max", $3, $5); }
    | MIN '(' expr ',' expr ')' { $$ = createFunc2Node("min", $3, $5); }
;

%%

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    return 0;
}