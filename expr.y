%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

ASTNode *root;
int yylex(void);
void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }
%}

%union {
    double dval;
    ASTNode *node;
}

%token <dval> NUMBER
%token VAR SIN COS TAN EXP LOG
%type <node> expr
%token QUIT

%left '+' '-'
%left '*' '/'
%right '^'
%right UMINUS

%%

input:
    /* empty */
  | input expr '\n' { root = $2; return 0; }
  | QUIT '\n' { exit(0); } 
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
    | SIN '(' expr ')' { $$ = createFuncNode("sin", $3); }
    | COS '(' expr ')' { $$ = createFuncNode("cos", $3); }
    | TAN '(' expr ')' { $$ = createFuncNode("tan", $3); }
    | EXP '(' expr ')' { $$ = createFuncNode("exp", $3); }
    | LOG '(' expr ')' { $$ = createFuncNode("log", $3); }
;

%%
