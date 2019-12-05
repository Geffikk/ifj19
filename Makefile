all:
	gcc -std=gnu99 -pedantic -Wall -Wextra -Werror -g main.c scanner.c parser.c code_generator.c lexem_string.c stack.c expression_stack.c symtable.c -o vysl 
