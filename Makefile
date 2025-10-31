all: graph_compiler graph_compiler_multi

graph_compiler: expr.l expr.y ast.c main.c
	bison -d expr.y
	flex expr.l
	gcc -o graph_compiler expr.tab.c lex.yy.c ast.c main.c -lm

graph_compiler_multi: expr.l expr.y ast.c main_multi.c
	bison -d expr.y
	flex expr.l
	gcc -o graph_compiler_multi expr.tab.c lex.yy.c ast.c main_multi.c -lm

clean:
	rm -f graph_compiler graph_compiler_multi lex.yy.c expr.tab.c expr.tab.h data*.txt