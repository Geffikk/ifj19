
/******************* INDENTATION STACK *********************
 * @author : Maros Geffert <xgeffe00@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Indentation stack
***********************************************************/

#include "stack.h"

int STACK_SIZE = MAX_STACK;

void stackError (int error_code)
{
    static const char* SERR_STRINGS[MAX_SERR + 1] = {"Unknown error","Stack error: INIT","Stack error: PUSH","Stack error: TOP"};
    if ( error_code <= 0 || error_code > MAX_SERR )
        error_code = 0;
    printf ( "%s\n", SERR_STRINGS[error_code] );
}

void stackInit ( tStack* s ) {

    if (s == NULL) {
        stackError(SERR_INIT);
    } else {
        s->top = 0;
    }
}

int stackEmpty ( const tStack* s ) {

    return(s->top == 0) ? 1 : 0;
}

int stackFull ( const tStack* s ) {

    return ((STACK_SIZE - 1) == s->top) ? 1 : 0;
}

void stackTop ( const tStack* s, char* c ) {

    if(stackEmpty(s) == 0){
        *c = s->arr[s->top];
    }
    else{
        stackError(SERR_TOP);
    }
}


void stackPop ( tStack* s ) {

    if(stackEmpty(s) == 0){
        s->arr[s->top] = '\0';
        s->top = s->top -1;
    }
}


void stackPush ( tStack* s, char c ) {

    if(stackFull(s) == 0){
        s->top = s->top + 1;
        s->arr[s->top] = c;
    }
    else{
        stackError(SERR_PUSH);
    }
}