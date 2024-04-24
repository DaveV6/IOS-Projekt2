.DEFAULT_GOAL=compile
CFLAGS=-std=gnu99 -Wall -g -Wextra -Werror -pedantic
EXEC=proj2
CC=gcc

run: compile
	@./$(EXEC) $(ARGS)

compile: proj2.c
	$(CC) $(CFLAGS) $^ -o $(EXEC)

.PHONY: compile run