//
// Created by geffik on 19.11.19.
//

#ifndef IFJ_STACK_H
#define IFJ_STACK_H

#include <stdio.h>
#include "error.h"

// maximal size of stack
#define  MAX_STACK 20
extern int STACK_SIZE;

// stack structure
typedef struct
{
    char arr[MAX_STACK];
    int top;
} tStack;

/** Print error message, when stack fail
 *
 * @param error_code - message
 */
void stackError (int error_code);

/** Initialization STACK
 *
 * @param s - stack, which i wanna initialize
 */
void stackInit (tStack* s);

/** Empty STACK
 *
 * @param s - stack, which i wanna control, if is empty
 * @return - 1 if is empty, 0 if is something in
 */
int stackEmpty (const tStack* s);

/** Full STACK
 *
 * @param s - stack, which i wanna control, if is full
 * @return - 1 if is full, 0 if is free space there
 */
int stackFull (const tStack* s);

/** stack TOP
 *
 * @param s - stack
 * @param c - assign value top value of stack
 */
void stackTop (const tStack* s, char* c);

/** stack POP
 *
 * @param s - stack, pop top value from stack
 */
void stackPop (tStack* s);

/** stack PUSH
 *
 * @param s - stack, where i wanna push character
 * @param c - character, which i wanna push
 */
void stackPush (tStack* s, char c);

#endif //IFJ_STACK_H
