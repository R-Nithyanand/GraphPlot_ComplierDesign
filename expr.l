%{
#include "ast.h"
#include "symtab.h"
#include "expr.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E  2.71828182845904523536
#endif

// Keyword lookup table
typedef struct {
    const char *name;
    int token;
} Keyword;

static Keyword keywords[] = {
    {"sin",SIN},{"cos",COS},{"tan",TAN},
    {"asin",ASIN},{"acos",ACOS},{"atan",ATAN},
    {"sinh",SINH},{"cosh",COSH},{"tanh",TANH},
    {"exp",EXP},{"log",LOG},{"ln",LN},
    {"sqrt",SQRT},{"abs",ABS},
    {"max",MAX},{"min",MIN},
    {"ceil",CEIL},{"floor",FLOOR},
    {"let",LET},{"def",DEF},
    {"plot",PLOT},{"ast",AST_CMD},
    {"vars",VARS},{"funcs",FUNCS},{"show",SHOW},
    {"quit",QUIT},{"exit",QUIT},
    {"clear",CLEAR},{"list",LIST},
    {"d",DERIV},
    {NULL,0}
};

static int lookup_keyword(const char *s)
{
    for (int i=0; keywords[i].name; ++i)
        if (!strcmp(s, keywords[i].name))
            return keywords[i].token;
    return 0;
}
%}

%option nounput noinput

%%

[ \t]+                  ;                              /* ignore whitespace */
[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)? {
                            yylval.dval = atof(yytext);
                            return NUMBER;
                        }
"pi"|"PI"               { yylval.dval = M_PI; return NUMBER; }                       
"e"|"E"                 { yylval.dval = M_E;  return NUMBER; }
"x"                     { return VAR; }
[a-zA-Z_][a-zA-Z0-9_]* {
                            int tok = lookup_keyword(yytext);
                            if (tok) return tok;
                            yylval.sval = strdup(yytext);
                            return IDENTIFIER;
                        }
"=="                    { return EQ; }
"!="                    { return NEQ; }
"<="                    { return LE; }
">="                    { return GE; }
"<"                     { return '<'; }
">"                     { return '>'; }
"^"                     { return '^'; }
"="                     { return '='; }
","                     { return ','; }
[-+*/(){}]              { return yytext[0]; }
\n                      { return '\n'; }
.                       ;                              /* ignore unknown */

%%

int yywrap(void) { return 1; }