
.PHONY: test
obj := $(patsubst %.c, %.o, $(wildcard *.c))
CC=gcc
name=ifj19
cflags=-std=gnu99 -pedantic -Wall -Wextra -Werror -g
.PHONY: test

all: $(name)

$(name): $(obj)
	$(CC) $(cflags) $^ -o $@

run:
	./$(name)

test:
	./test.sh

clean:
	rm -f $(name) *.o

