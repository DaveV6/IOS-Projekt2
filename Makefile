.DEFAULT_GOAL=compile
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
EXEC=proj2
CC=gcc

run: compile
	@./$(EXEC) $(ARGS)

compile: $(EXEC).c $(EXEC).h
	$(CC) $(CFLAGS) $^ -o $(EXEC)

pack:
	@echo Packing zip...
	@mkdir pack
	@cp proj2.c pack && cp Makefile pack && cp proj2.h pack
	@cd pack && zip proj2.zip proj2.c Makefile proj2.h
	@mv pack/proj2.zip .
	@rm -rf pack

clean:
	@echo Cleaning...
	@rm -rf proj2 && rm -rf proj2.out && rm -rf *.zip

.PHONY: compile run pack clean