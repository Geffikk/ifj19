
/************************ EXPRESSION_STACK *************************
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Expression stack
***********************************************************/

#include <stdlib.h>
#include "expression_stack.h"

void Expression_stack_init(Expression_stack* stack)
{
	stack->top = NULL; // inicializacia stacku
}


bool Expression_stack_push(Expression_stack* stack, Symbol_enumeration symbol, Data_type type)
{
	Expression_stack_entry* new_entry = (Expression_stack_entry*)malloc(sizeof(Expression_stack_entry)); // alokovanie struct new item

	if (new_entry == NULL)
		return false;

    new_entry->symbol = symbol;          // pri prvom prejdeni = SYMBOL_DOLLAR           // pri druhom = SYMBOL_INTEGER
	new_entry->data_type = type;         // pri prvom prejdeni = TYPE_NOT_DEFINED        // pri druhom = TYPE_INT
	new_entry->next = stack->top;        // pri prvom prejdeni = NULL                    // pri druhom = to, co bolo na predchadzajucom prejdeni(SYMBOL_DOLLAR, TYPE_NOT_DEFINED, NULL)

	stack->top = new_entry; // pripradi hodnoty z hora

	return true;
}


bool Expression_stack_pop(Expression_stack* stack)
{
	if (stack->top != NULL)
	{
		Expression_stack_entry* pointer = stack->top;
		stack->top = pointer->next;
		free(pointer);

		return true;
	}
	return false;
}


void Expression_stack_count_of_pop(Expression_stack* stack, int count)
{
	for (int counter = 0; counter < count; counter++)
	{
		Expression_stack_pop(stack);
	}
}


Expression_stack_entry* Expression_stack_top_ter(Expression_stack* stack)
{
	for (Expression_stack_entry* pointer = stack->top; pointer != NULL; pointer = pointer->next)
	{
		if (pointer->symbol < SYMBOL_STOP)
			return pointer;
	}

	return NULL;
}


bool Expression_stack_insert_after_top_ter(Expression_stack* stack, Symbol_enumeration symbol, Data_type type)
{
    Expression_stack_entry* past = NULL;

	for (Expression_stack_entry* pointer = stack->top; pointer != NULL; pointer = pointer->next)
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
				new_item->next = stack->top;
				stack->top = new_item;
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


Expression_stack_entry* Expression_stack_top(Expression_stack* stack)
{
	return stack->top;
}


void Expression_stack_free(Expression_stack* stack)
{
	while (Expression_stack_pop(stack));
}
