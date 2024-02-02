CC=gcc
CFLAGS=-Wall -pedantic -ggdb

main: lexer.o src/main.c
	$(CC) $(CFLAGS) -o main symbol_table.o string.o lexer.o src/main.c

lexer.o: string.o symbol_table.o src/lexer.c src/lexer.h
	$(CC) $(CFLAGS) -c src/lexer.c

string.o: src/string.c
	$(CC) $(CFLAGS) -c src/string.c

symbol_table.o: src/symbol_table.c
	$(CC) $(CFLAGS) -c src/symbol_table.c