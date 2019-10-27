//
// Created by maros on 10/16/2019.
//

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "scanner.c"

int main()
{
    Lexem_string *lexem_string;
    lexem_string = (Lexem_string*) malloc(sizeof(Lexem_string)); // s = "AUTO";
    lexem_string_init(lexem_string);
    lexem_string_clear(lexem_string);
/*
    char add_char = 'a';
    add_char_to_lexem_string(lexem_string, add_char);
    printf("%s (I added a character) \n", *lexem_string);

    const char* add_string = "Intieger";
    add_string_to_lexem_string(lexem_string, add_string);
    printf("%s (I added a string) \n", *lexem_string);

    const char* string_to_cmp = "aIntieger";
    if(cmp_lexem_string_with_another_string(lexem_string, string_to_cmp) == true){
        printf("True");
    }
    else{
        printf("False");
    }
*/
    FILE *source_file;
    Token *token;
    if ((source_file = fopen("C:\\Users\\maros\\CLionProjects\\IFJ_19\\test.txt", "rb")) == NULL) {
        perror("fp fopen");
        return 1;
    }

    Stack_indent_dedent *stack_main = (Stack_indent_dedent*)malloc(sizeof(Stack_indent_dedent));
    set_stack(stack_main);
    set_string(lexem_string);
    set_source_file(source_file);
    for(int i = 0; i <= 1000; i++)
    {
        get_token(&token);
    }



    return 0;
}