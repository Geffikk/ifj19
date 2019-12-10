
/************************ EXPRESSION_STACK HEADER *************************
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
**************************************************************************/
#ifndef _EXPRESSION_STACK_H
#define _EXPRESSION_STACK_H


#include <stdbool.h>
#include "parser.h"
#include "symtable.h"


typedef struct stack_item
{
	Symbol_enumeration symbol;
	Data_type data_type;
	struct stack_item *next;
}
Expression_stack_entry;

typedef struct
{
    Expression_stack_entry *top; /// entry on stack top
}
Expression_stack;

/** Function for initialization of stack
 *
 *  @param expr_stack - stack pointer
 ***/
void Expression_stack_init(Expression_stack* expr_stack);

/** Function for return symbol on top of stack
 *
 *  @param expr_stack - stack pointer
 *  @return pointer to stack top
 ***/
Expression_stack_entry* Expression_stack_top(Expression_stack* expr_stack);

/** Function for return symbol on top of stack
 *
 *  @param expr_stack - stack pointer
 *  @return count of symbols poped
 ***/
bool Expression_stack_pop(Expression_stack* expr_stack);

/** Function for return symbol on top of stack
 *
 *  @param expr_stack - stack pointer
 *  @param count - count of symbols poped
 ***/
void Expression_stack_count_of_pop(Expression_stack* expr_stack, int count);

/** Function for pushing symbol on stack
 *
 *  @param expr_stack - stack pointer
 *  @param type - data type for pushed symbol
 *  @param symbol - pushed symbol
 *  @return pointer to stack top
 ***/
bool Expression_stack_push (Expression_stack* expr_stack, Data_type type, Symbol_enumeration symbol);

/** Function for implementation shift of operands in expression by precedence algorithm
 *
 *  @param expr_stack - stack pointer
 *  @return pointer to top ter
 ***/
Expression_stack_entry* Expression_stack_top_ter(Expression_stack* expr_stack);

/** Function for pushing symbol on stack
*
*  @param expr_stack - stack pointer
*  @param type - data type for pushed symbol
*  @param symbol - pushed symbol
*  @return bool value
***/
bool Expression_stack_after_top_ter(Expression_stack* expr_stack, Data_type type, Symbol_enumeration symbol);

/** Function for free of all used resources
 *
 *  @param expr_stack - stack pointer
 ***/
void Expression_stack_free(Expression_stack* expr_stack);

#endif
