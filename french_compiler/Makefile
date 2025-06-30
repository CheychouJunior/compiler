# Simple Makefile for French Language Compiler
# Chapter 4 - Code Generation

CC = gcc
CFLAGS = -Wall -g
NASM = nasm
LD = ld

# Main targets
all: compilateur

# Build the compiler
compilateur: analyseur.tab.c lex.yy.c
	$(CC) $(CFLAGS) -o compilateur analyseur.tab.c lex.yy.c -lfl

# Generate parser with Bison
analyseur.tab.c analyseur.tab.h: analyseur.y
	bison -d analyseur.y

# Generate lexer with Flex
lex.yy.c: analyseur_lexical.l analyseur.tab.h
	flex analyseur_lexical.l

# Test the compiler
test: compilateur
	./compilateur < ternary_test.fr
	$(NASM) -f elf32 -o output.o output.asm
	$(LD) -m elf_i386 -o program output.o
	./program

# Clean generated files
clean:
	rm -f lex.yy.c analyseur.tab.c analyseur.tab.h
	rm -f compilateur output.asm output.o program
	rm -f test.fr

.PHONY: all test clean
