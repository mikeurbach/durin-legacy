CC = gcc
CFLAGS = -g
EXEC = durin
OBJS = durin.o repl.o script.o
SRCS = durin.c repl.c script.c
HDRS = durin.h repl.h script.h
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -c $(SRCS)

parser:
	bison -d -v parser.y
	flex lexer.l
	$(CC) $(CFLAGS) -o $(EXEC) lex.yy.c parser.tab.c ast.c

clean:
	rm -f durin
	rm -f *.o
	rm -f *~
	rm -f lex.yy.c
	rm -f parser.tab.*
	rm -f parser.output
