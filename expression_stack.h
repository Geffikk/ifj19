
/************************ EXPRESSION_STACK *************************
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Expression stack header
***********************************************************/
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
} Expression_stack_entry;

typedef struct
{
	Expression_stack_entry *top;
} Expression_stack;

void Expression_stack_init(Expression_stack* stack);

bool Expression_stack_push(Expression_stack* stack, Symbol_enumeration symbol, Data_type type);

bool Expression_stack_pop(Expression_stack* stack);

void Expression_stack_count_of_pop(Expression_stack* stack, int count);

Expression_stack_entry* Expression_stack_top_ter(Expression_stack* stack);

bool Expression_stack_insert_after_top_ter(Expression_stack* stack, Symbol_enumeration symbol, Data_type type);

Expression_stack_entry* Expression_stack_top(Expression_stack* stack);

void Expression_stack_free(Expression_stack* stack);

#endif
