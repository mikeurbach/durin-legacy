CC = gcc
CFLAGS = -Wall -pedantic -ansi -g
EXEC = durin
OBJS = durin.o repl.o script.o
SRCS = durin.c repl.c script.c
HDRS = durin.h repl.h script.h
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -c $(SRCS)
clean:
	rm -f durin
	rm -f *.o
	rm -f *~
