%{
#include "ast.h"
#include "expr.tab.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
%}

%%

[ \t]+              ;
[0-9]+(\.[0-9]+)?   { yylval.dval = atof(yytext); return NUMBER; }
"pi"|"PI"           { yylval.dval = M_PI; return NUMBER; }
"e"|"E"             { yylval.dval = M_E; return NUMBER; }
"quit"|"exit"       { return QUIT; }
"x"                 { return VAR; }
"sin"               { return SIN; }
"cos"               { return COS; }
"tan"               { return TAN; }
"exp"               { return EXP; }
"log"               { return LOG; }
"sqrt"              { return SQRT; }
"abs"               { return ABS; }
"ln"                { return LN; }
"asin"              { return ASIN; }
"acos"              { return ACOS; }
"atan"              { return ATAN; }
"sinh"              { return SINH; }
"cosh"              { return COSH; }
"tanh"              { return TANH; }
"^"                 { return '^'; }
[-+*/(){}]          { return yytext[0]; }
\n                  { return '\n'; }
.                   { /* ignore unknown */ }

%%

int yywrap() { return 1; }
