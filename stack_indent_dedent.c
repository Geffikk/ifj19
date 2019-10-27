//
// Created by maros on 10/26/2019.
//

#include "stack_indent_dedent.h"
int stack_size = MAX_STACK;
int err_flag;

void stackError (int error_code)
{
    static const char* SERR_STRINGS[MAX_SERR+1] = {"Unknown error","Stack error: INIT","Stack error: PUSH","Stack error: TOP"};
    if ( error_code <= 0 || error_code > MAX_SERR )
        error_code = 0;
    printf ( "%s\n", SERR_STRINGS[error_code] );
    err_flag = 1;
}

void stackInit (Stack_indent_dedent *stack)
{

    // Pokial je zásobník prázdny tak mi vrať error
    if (stack == NULL)
    {
        stackError(SERR_INIT);
        return;
    }

    // Inak mi ho inicializuj na -1 (značenie prázdneho zásobniku)
    stack->top = 0;
}

int stackEmpty (const Stack_indent_dedent *stack)
{
    // Ak je zásobník prázdny vráť mi -1
    return stack->top == 0;
}

int stackFull(const Stack_indent_dedent *stack)
{

    // Maximálny index na zásobniku je STACK_SIZE-1
    return stack->top == stack_size - 1;
}

void stackTop(const Stack_indent_dedent *stack, char *c)
{
    // Kontrolujem či je s prázdne ak áno tak error
    if (stackEmpty(stack))
    {
        stackError(SERR_TOP);
        return;
    }
    // Ukladám na adresu vrchol zásobníku
    *c = stack->arr[stack->top];
}

void stackPop(Stack_indent_dedent *stack)
{
    // Kontrola či je s prázdny ak nie tak vymaž vrchol zásobniku
    if(!stackEmpty(stack))
    {
        stack->top--;
    }
}

void stackPush(Stack_indent_dedent *stack, char c)
{
    // Ak nie je s (zásobník) plný tak zvýšim index na zásobniku a priradim hodnotu c
    if(!stackFull(stack))
    {
        stack->top++;
        stack->arr[stack->top] = c;
    }
    else
    {
        stackError(SERR_PUSH);
    }
}
