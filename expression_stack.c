
/************************ EXPRESSION_STACK *************************
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Stack for parser's expressions
***********************************************************/

#include <stdlib.h>
#include "expression_stack.h"


void Expression_stack_init(Expression_stack* expr_stack)
{
	expr_stack->top = NULL;
}

Expression_stack_entry* Expression_stack_top(Expression_stack* expr_stack)
{
    return expr_stack->top;
}

bool Expression_stack_pop (Expression_stack* expr_stack)
{
    if (expr_stack->top != NULL)
    {
        Expression_stack_entry* pointer = expr_stack->top;
        expr_stack->top = pointer->next;

        free(pointer);

        return true;
    }
    return false;
}

void Expression_stack_count_of_pop (Expression_stack* expr_stack, int count)
{
    for (int counter = 0; counter < count; counter++)
    {
        Expression_stack_pop(expr_stack);
    }
}

bool Expression_stack_push (Expression_stack* expr_stack, Data_type type, Symbol_enumeration symbol)
{
	Expression_stack_entry* new_entry = (Expression_stack_entry*)malloc(sizeof(Expression_stack_entry));

	if (new_entry == NULL) {
        return false;
    }
	else
	{

        new_entry->symbol = symbol;
        new_entry->data_type = type;
        new_entry->next = expr_stack->top;

        expr_stack->top = new_entry;

        return true;
    }
}

Expression_stack_entry* Expression_stack_top_ter (Expression_stack* expr_stack)
{
	for (Expression_stack_entry* pointer = expr_stack->top; pointer != NULL; pointer = pointer->next)
	{
		if (pointer->symbol < SYMBOL_STOP)
			return pointer;
	}

	return NULL;
}


bool Expression_stack_after_top_ter (Expression_stack* expr_stack, Data_type type, Symbol_enumeration symbol)
{
    Expression_stack_entry* past = NULL;

	for (Expression_stack_entry* pointer = expr_stack->top; pointer != NULL; pointer = pointer->next)
	{
		if (pointer->symbol < SYMBOL_STOP) // pozeram, ci moj symbol zo zasobniku nie je Stop symbol
		{
            Expression_stack_entry* new_item = (Expression_stack_entry*)malloc(sizeof(Expression_stack_entry)); //alokovanie miesta pre struct

			if (new_item == NULL)
				return false;

			new_item->symbol = symbol;
			new_item->data_type = type;

			if (past == NULL)
			{
				new_item->next = expr_stack->top;
				expr_stack->top = new_item;
			}
			else
			{
				new_item->next = past->next;
				past->next = new_item;
			}

			return true;
		}

		past = pointer;
	}

	return false;
}

void Expression_stack_free (Expression_stack* expr_stack)
{
	while (Expression_stack_pop(expr_stack));
}

