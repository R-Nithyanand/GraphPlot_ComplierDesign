CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

all: graph_compiler

graph_compiler: expr.tab.o lex.yy.o ast.o symtab.o main.o
	$(CC) -o graph_compiler expr.tab.o lex.yy.o ast.o symtab.o main.o $(LDFLAGS)

expr.tab.c expr.tab.h: expr.y
	bison -d expr.y

lex.yy.c: expr.l expr.tab.h
	flex expr.l

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

main.o: main.c ast.h expr.tab.h
	$(CC) $(CFLAGS) -c main.c

expr.tab.o: expr.tab.c ast.h
	$(CC) $(CFLAGS) -c expr.tab.c

lex.yy.o: lex.yy.c expr.tab.h ast.h
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	rm -f graph_compiler *.o lex.yy.c expr.tab.c expr.tab.h data*.txt

.PHONY: all clean
