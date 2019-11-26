//
// Created by maros on 10/16/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

FILE *source_file;

int main()
{
    Lexem_string *lexem_string;
    lexem_string = (Lexem_string*) malloc(sizeof(Lexem_string)); // s = "AUTO";
    lexem_string_init(lexem_string);
    lexem_string_clear(lexem_string);

    tStack *stack;
    stack = (tStack*) malloc(sizeof(tStack));
    stackInit(stack);

    char filename[20];
    char dpath[50]="/home/geffik/CLionProjects/IFJ-ACTUAL/";
    char fnamepath[100]="";

    printf("Enter the filename : ");
    scanf("%s", filename);

    strcat(fnamepath, dpath);
    strcat(fnamepath, filename);

    if ((source_file = fopen(fnamepath, "r")) == NULL) {
        perror("fp fopen");
        return 1;
    }

    Token *token;
    token = (Token*) malloc(sizeof(Token));

    set_source_file(source_file);
    set_string(lexem_string);
    set_stack(stack);

    bool END_OF_FILE = false;
    bool first_line = true;
    int  count_lines = 1;
    int count_words = 1;

    while(!END_OF_FILE)
    {

        if(token->type == 5 || first_line || token->type == 4 || token->type == 3)
        {
            if(count_lines <= 9)
            {
                printf("Line [%d]  --  | ", count_lines);
            }
            else
            {
                printf("Line [%d] --  | ", count_lines);
            }
            first_line = false;
            count_lines ++;
            count_words = 0;
        }

        get_token(token, stack);

        switch(token->type)
        {
            case (0) :
                printf("W(%d)-[keyword]   ", count_words);
                break;
            case (1) :
                printf("W(%d)-[identif]   ", count_words);
                break;
            case (2) :
                printf("W(%d)-[   ,   ]   ", count_words);
                break;
            case (3) :
                printf("W(%d)-[indent ]   ", count_words);
                break;
            case (4) :
                printf("W(%d)-[dedent ]   ", count_words);
                break;
            case (5) :
                printf("W(%d)-[  EOL  ]   ", count_words);
                break;
            case (6) :
                printf("W(%d)-[  EOF  ]   ", count_words);
                END_OF_FILE = true;
                break;
            case (7) :
                printf("W(%d)-[ empty ]   ", count_words);
                break;
            case (8) :
                printf("W(%d)-[integer]   ", count_words);
                break;
            case (9) :
                printf("W(%d)-[ float ]   ", count_words);
                break;
            case (10) :
                printf("W(%d)-[string ]   ", count_words);
                break;
            case (11) :
                printf("W(%d)-[  ==   ]   ", count_words);
                break;
            case (12) :
                printf("W(%d)-[  !=   ]   ", count_words);
                break;
            case (13) :
                printf("W(%d)-[   >   ]   ", count_words);
                break;
            case (14) :
                printf("W(%d)-[   <   ]   ", count_words);
                break;
            case (15) :
                printf("W(%d)-[  >=   ]   ", count_words);
                break;
            case (16) :
                printf("W(%d)-[  <=   ]   ", count_words);
                break;
            case (17) :
                printf("W(%d)-[   +   ]   ", count_words);
                break;
            case (18) :
                printf("W(%d)-[   -   ]   ", count_words);
                break;
            case (19) :
                printf("W(%d)-[   *   ]   ", count_words);
                break;
            case (20) :
                printf("W(%d)-[   /   ]    ", count_words);
                break;
            case (21) :
                printf("W(%d)-[   (   ]   ", count_words);
                break;
            case (22) :
                printf("W(%d)-[   )   ]   ", count_words);
                break;
            case (23) :
                printf("W(%d)-[   =   ]   ", count_words);
                break;
            case (24) :
                printf("W(%d)-[   :   ]   ", count_words);
                break;
            case (26) :
                printf("W(%d)-[D-SRING]   ", count_words);
                break;
        }

        if(token->type == 5 || token->type == 4 || token->type == 3)
        {
            printf("\n");
        }

        count_words++;
    }



    return 0;
}