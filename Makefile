CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: proj2.c
	gcc $(CFLAGS) proj2.c -o proj2