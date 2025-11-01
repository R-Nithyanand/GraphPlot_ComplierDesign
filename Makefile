all: graph_compiler

graph_compiler: expr.l expr.y ast.c main.c
	bison -d expr.y
	flex expr.l
	gcc -o graph_compiler expr.tab.c lex.yy.c ast.c main.c -lm

clean:
	rm -f graph_compiler lex.yy.c expr.tab.c expr.tab.h data*.txt