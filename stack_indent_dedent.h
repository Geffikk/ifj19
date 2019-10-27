//
// Created by maros on 10/26/2019.
//

#ifndef IFJ_STACK_INDENT_DEDENT_H
#define IFJ_STACK_INDENT_DEDENT_H

#define MAX_STACK 20

#define MAX_SERR    3                                   /* počet možných chyb */
#define SERR_INIT   1                                  /* chyba při stackInit */
#define SERR_PUSH   2                                  /* chyba při stackPush */
#define SERR_TOP    3                                  /* chyba při stackTop */

typedef  struct
{
    char arr[MAX_STACK];
    int top;
} Stack_indent_dedent;

void stackError (int error_code);
void stackInit (Stack_indent_dedent *stack );
int stackEmpty (const Stack_indent_dedent *stack);
int stackFull (const Stack_indent_dedent *stack);
void stackTop (const Stack_indent_dedent *stack, char *c);
void stackPop (Stack_indent_dedent *stack);
void stackPush (Stack_indent_dedent *stack, char c);

#endif //IFJ_STACK_INDENT_DEDENT_H