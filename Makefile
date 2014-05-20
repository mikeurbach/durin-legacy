CC = gcc
CFLAGS = -g -Wall
CLIBS = -lcgraph -lgvc
LEXER = lex.yy.c
LEXER_SRC = lexer.l
PARSER = parser.tab.c
PARSER_SRC = parser.y
SRCS = durin.c repl.c script.c ast.c symtab.c codegen.c $(PARSER) $(LEXER)
HDRS = durin.h repl.h script.h ast.h symtab.h codegen.h
OBJS = durin.o repl.o script.o ast.o symtab.o codegen.o parser.tab.o lex.yy.o
EXEC = durin


$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(CLIBS)

$(OBJS): $(SRCS) $(HDRS) $(PARSER) $(LEXER)
	$(CC) $(CFLAGS) -c $(SRCS) $(PARSER) $(LEXER)

$(PARSER): $(PARSER_SRC)
	bison -d -v $(PARSER_SRC)

$(LEXER): $(LEXER_SRC)
	flex $(LEXER_SRC)

clean:
	rm -f $(EXEC)
	rm -f $(OBJS)
	rm -f $(PARSER)
	rm -f *~
	rm -f lex.yy.c
	rm -f parser.tab.*
	rm -f parser.output
	rm -f datapath.*
	rm -f examples/*~