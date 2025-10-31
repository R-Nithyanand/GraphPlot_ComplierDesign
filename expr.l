%{
#include "ast.h"
#include "expr.tab.h"
#include <math.h>
%}

%%

[ \t]+              ;
[0-9]+(\.[0-9]+)?   { yylval.dval = atof(yytext); return NUMBER; }
"quit"|"exit"       { return QUIT; }
"x"                 { return VAR; }
"sin"               { return SIN; }
"cos"               { return COS; }
"tan"               { return TAN; }
"exp"               { return EXP; }
"log"               { return LOG; }
"^"                 { return '^'; }
[-+*/(){}]          { return yytext[0]; }
\n                  { return '\n'; }
.                   { /* ignore unknown */ }

%%

int yywrap() { return 1; }
