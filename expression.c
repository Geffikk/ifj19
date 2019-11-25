/**
* @brief Implementacia vyrazov parseru	
* @author Martin Valach
*/

#include "scanner.h"
#include "vyrazy.h"
#include "error.h"
#include "code_generator.h"
#include "symstack.h"

#define FREE_RESOURCES_RETURN(_return)
	do {
		symbol_stack_free(&stack);
		return _return;
	} while (0)

Symbol_stack stack;

typedef enum
{
	SHIFT,	// < SHIFT 	0
	MATCH,	// = MATCH 	1
	REDUC,	// > REDUCE 	2
	ERROR	// # ERROR 	3
}
Sign_enumeration;

typedef enum
{
	INDEX_RELATION_OPERATOR,		// index for ==, !=, <=, >=, <, > 	0 
	INDEX_PLUS_OR_MINUS,			// index for +, - 			1
	INDEX_MULTIPLY_OR_DIVIDE,		// index for *, /, // 			2
	INDEX_RIGHT_BRACKET,			// index for ) 				3
	INDEX_LEFT_BRACKET,			// index for ( 				4 
	INDEX_DATA,				// index for identifier and data types 	5
	INDEX_DOLLAR				// index for $				6
}
Index_enumeration;


int precedence_table[7][7] =
{
//	| REL |  +,- |*,/,//|   )  |   (  | data | other|
	{ERROR, SHIFT, SHIFT, REDUC, SHIFT, SHIFT, REDUC} // RELATION OPERATORS
	{REDUC, REDUC, SHIFT, REDUC, SHIFT, SHIFT, REDUC} // +, -
	{REDUC, REDUC, REDUC, REDUC, SHIFT, SHIFT, REDUC} // *,/,//
	{REDUC, REDUC, REDUC, REDUC, ERROR, ERROR, REDUC} // )
	{SHIFT, SHIFT, SHIFT, MATCH, SHIFT, SHIFT, ERROR} // (
	{REDUC, REDUC, REDUC, REDUC, ERROR, ERROR, REDUC} // data
	{SHIFT, SHIFT, SHIFT, ERROR, SHIFT, SHIFT, ERROR} // other
	
}

static Index_enumeration get_Index (Symbol_enumeration symbol)
{
// SYMBOL_EQUAL, SYMBOL_NOT_EQUAL, SYMBOL_LESS_EQUAL, SYMBOL_MORE_EQUAL, SYMBOL_LESS, SYMBOL_MORE
	if (symbol == 0 || symbol == 1 || symbol == 2 || symbol == 3 || symbol == 4 || symbol == 5)
	{
		return INDEX_RELATION_OPERATOR;
	}
	else if (symbol == 6 || symbol == 7) // SYMBOL_PLUS, SYMBOL_MINUS
	{
		return INDEX_PLUS_OR_MINUS;
	}
	else if (symbol == 8 || symbol == 9 || symbol = 10) // SYMBOL_MULTIPLY, SYMBOL_DIVIDE, SYMBOL_DIVIDE_INTEGER

	{
		return INDEX_MULTIPLY_OR_DIVIDE;
	}
	else if (symbol == 11) // SYMBOL_RIGHT_BRACKET
	{
		return INDEX_RIGHT_BRACKET;
	}
	else if (symbol == 12) // SYMBOL_LEFT_BRACKET
	{
		return INDEX_LEFT_BRACKET;
	}
	else if (symbol == 13 || symbol == 14 || symbol == 15 || symbol == 16) // SYMBOL_IDENTIFIER, SYMBOL_INTEGER, SYMBOL_FLOAT, SYMBOL_STRING
	{
		return INDEX_DATA;
	}
	else	// SYMBOL_DOLLAR
	{
		return DOLLAR;
	}
}

static Symbol_enumeration get_Symbol (Token* token)
{
	if (token->type == token_type_EQ)
	{
		return SYMBOL_EQUAL;
	}
	else if (token->type == token_type_NEQ)
	{
		return SYMBOL_NOT_EQUAL;
	}
	else if (token->type == token_type_LEQ)
	{
		return SYMBOL_LESS_EQUAL;
	}
	else if (token->type == token_type_MEQ)
	{
		return SYMBOL_MORE_EQUAL;
	}
	else if (token->type == token_type_LT)
	{
		return SYMBOL_LESS;
	}
	else if (token->type == token_type_MT)
	{
		return SYMBOL_MORE;
	}
	else if (token->type == token_type_plus)
	{
		return SYMBOL_PLUS;
	}
	else if (token->type == token_type_minus)
	{
		return SYMBOL_MINUS;
	}
	else if (token->type == token_type_mul)
	{
		return SYMBOL_MULTIPLY;
	}
	else if (token->type == token_type_div)
	{
		return SYMBOL_DIVIDE;
	}
	else if (token->type == token_type_ )
	{
		return SYMBOL_DIVIDE_INTEGER;
	}
	else if (token->type == token_type_right_bracket)
	{
		return SYMBOL_RIGHT_BRACKET;
	}
	else if (token->type == token_type_left_bracket )
	{
		return SYMBOL_LEFT_BRACKET;
	}
	/*
	else if (token->type == token)
	{
		return SYMBOL_INTEGER;
	}
	else if (token->type == token )
	{
		return SYMBOL_float;
	}
	else if (token->type == token )
	{
		return SYMBOL_CHAR;
	}
	else if (token->type == token )
	{
		return SYMBOL_IDENTIFIER;
	}
	else if (token->type == state )
	{
		return SYMBOL_;
	}
	*/
	else
	{
		return SYMBOL_DOLLAR;
	}
}

static int count_after_stop (bool* stop_on_stack)
{
	int counter = 0;

	Symbol_stack_item* pointer = symbol_stack_top(&stack);

	while (pointer != NULL)
	{
		if (pointer->symbol != SYMBOL_STOP)
		{
			*stop_on_stack = false;
			counter++;
		}
		else
		{
			*stop_on_stack = true;
			break;
		}

		pointer = pointer->next;
	}

	return counter;
}



static Data_type get_Type (Token* token, PData* data)
{		
	if (token_type = token_type_integer)
	{
		return DATA_TYPE_INTEGER;
	}
	else if (token_type = token_type_float)
	{
		return DATA_TYPE_FLOAT;
	}
	else if (token_type = token_type_string)
	{
		return DATA_TYPE_STRING;
	}
	else if (token_type = token_type_identifier)
	{
		symbol = sym_table_search(&data->local_table, token->attribute.string->str);
		
		if (symbol != NULL)
		{
			return symbol->type;
		}
		else
		{
			return DATA_TYPE_NOT_DEFINED;
		}
	}
	else
	{
		return DATA_TYPE_NOT DEFINED;
	}
}


static Rule_enumeration check_Rule (int count_of_operands, symbol_stack_item* first_operand, symbol_stack_item* second_operand, symbol_stack_item* third_operand)
{

	if (count_of_operands < 1 || count_of_operands > 3)
	{
		return RULE_NOT_DEFINED;
	}
	else if (count_of_operands == 1)
	{
		switch (first_operand->symbol)
		{
			case SYMBOL_IDENTIFIER: 
			case SYMBOL_INTEGER:
			case SYMBOL_FLOAT:
			case SYMBOL_CHAR:			
				return RULE_OPERAND;
		}
		return RULE_NOT_DEFINED;
	}
	else if (count_of_operands == 2)
	{
		return RULE_NOT_DEFINED;	
	}
	else if (count_of_operands == 3)
	{
		if (first_operand->symbol == SYMBOL_NOT_DEFINED && third_operand->symbol == SYMBOL_NOT_DEFINED)
		{
			if (second_operand->symbol == SYMBOL_EQUAL)
			{
				return RULE_EQUAL;
			}
			else if (second_operand->symbol == SYMBOL_NOT_EQUAL)
			{
				return RULE_NOT_EQUAL;
			}
			else if (second_operand->symbol == SYMBOL_LESS_EQUAL)
			{
				return RULE_LESS_EQUAL;
			}
			else if (second_operand->symbol == SYMBOL__MORE_EQUAL)
			{
				return RULE_MORE_EQUAL;
			}	
			else if (second_operand->symbol == SYMBOL_LESS)
			{
				return RULE_LESS;
			}
			else if (second_operand->symbol == SYMBOL_MORE)
			{
				return RULE_MORE;
			}
			else if (second_operand->symbol == SYMBOL_PLUS)
			{
				return RULE_PLUS;
			}
			else if (second_operand->symbol == SYMBOL_MINUS)
			{
				return RULE_MINUS;
			}
			else if (second_operand->symbol == SYMBOL_MULTIPLY)
			{
				return RULE_MULTIPLY;
			}
			else if (second_operand->symbol == SYMBOL_DIVIDE)
			{
				return RULE_DIVIDE;
			}
			else if (second_operand->symbol == SYMBOL_DIVIDE_INTEGER)
			{
				return RULE_DIVIDE_INTEGER;
			}
			else
			{
				return RULE_NOT_DEFINED;
			}
		}
		else if (first_operand->symbol == SYMBOL_LEFT_BRACKET && second_operand->symbol == SYMBOL_NOT_DEFINED  && third_operand->symbol == SYMBOL_RIGHT_BRACKET)
		{
			return RULE_BRACKETS;
		}
	}
	return RULE_NOT_DEFINED;

}

int sematics_test (Rules_enumeration rule, Symbol_stack_item* first_operand, Symbol_stack_item* second_operand, Symbol_stack_item* third_operand, Data_type* type)
{

	int first_operand_to_float = 0;
	int third_operand_to_float = 0;


	if (rule == RULE_OPERAND) 
	{
		if (first_operand == DATA_TYPE_NOT_DEFINED)
		{
			return error_semantic; //sematicka chyba, nedefinovana premmenna
		}
	}
	else if (rule == RULE_BRACKETS)
	{
		if (second_operand->data_type == DATA_TYPE_NOT_DEFINED)
		{
			return error_semantic; //sematicka chyba, nedefinovana premenna
		}
	}
	else
	{
		if (second_operand->data_type == DATA_TYPE_NOT_DEFINED || third_operand->data_type = DATA_TYPE_NOT_DEFINED)
		{
			return error_semantic; //sematicka chyba, nedefinovana premmenna
		}
		if ((rule == RULE_DIVIDE || rule == RULE_DIVIDE_INTEGER) && second_operand == 0)
		{
			return error_div_zero; //sematicka chyba, delenie nulou
		}
	}

	if (rule == RULE_OPERAND)
	{
		*type = first_operand->data_type;
	}
	else if (rule == RULE_BRACKETS)
	{
		*type = second_operand->data_type;
	}
	else if (rule == RULE_EQUALL || rule == RULE_NOT_EQUAL || rule == RULE_LESS_EQUALL || rule == RULE_MORE_EQUAL || rule == RULE_LESS || rule == RULE_MORE)
	{
		//*type = TYPE_BOOL;
		*type = DATA_TYPE_INTEGER;

		if (first_operand->data_type == DATA_TYPE_INTEGER && third_operand->data_type == DATA_TYPE_FLOAT)
		{
			first_operand_to_float = 1;
			*type = DATA_TYPE_FLOAT;
		}
		else if (first_operand->data_type == DATA_TYPE_FLOAT && third_operand->data_type == DATA_TYPE_INTEGER)
		{
			third_operand_to_float = 1;
			*type = DATA_TYPE_FLOAT;
		}
/*		else if ()
		{
		
		}*/
		else if (first_operand->data_type != third_operand->data_type)
		{
			return error_semantic_compatibility; // sematicka chyba, nekompatibilne datove typy
		}
	}
	else if (rule == RULE_PLUS)
	{
		*type = DATA_TYPE_INTEGER;
	
		if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
		{
			if (first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type == DATA_TYPE_STRING) //konkatenacia
			{
				*type = DATA_TYPE_STRING;
			}

			if ((first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type != DATA_TYPE_STRING) || (first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type != DATA_TYPE_STRING))
			{
				return error_semantic_compatibility; // nekompatibilne datove typy
			}

			if (first_operand->data_type == TYPE_INT)
			{
				first_operand_to_float = 1;
			}
			else if (third_operand->data_type == TYPE_INT)
			{
				third_operand_to_float = 1;
			}
			
			*type = DATA_TYPE_float;
		}

	}
	else if (rule == RULE_MINUS || rule == RULE_MULTIPLY)
	{
		*type = DATA_TYPE_INTEGER;
		
		if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
		{
			if (first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type == DATA_TYPE_STRING)
			{
				return error_semantic_compatibility; // nekompatibilne datove typy
			}

			if (first_operand->data_type == TYPE_INT)
			{
				first_operand_to_float = 1;
			}
			else if (third_operand->data_type == TYPE_INT)
			{
				third_operand_to_float = 1;
			}
			
			*type = DATA_TYPE_float;
		}
	}
	else if (rule = RULE_DIVIDE)
	{
		*type = DATA_TYPE_FLOAT;

		if (first_operand->data_type != DATA_TYPE_FLOAT || third_operand->data_type != DATA_TYPE_FLOAT)
		{

			if (first_operand->data_type == DATA_TYPE_INTEGER)
			{
				first_operand_to_float = 1;
			}
			
			if (third_operand->data_type == DATA_TYPE_INTEGER)
			{
				third_operand_to_float = 1;
			}
			
			if (first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type == DATA_TYPE_STRING)
			{
				return error_semantic_compatibility; // sematicka_chyba, nekompatibilne  datove typy
			}
		}	
	}
	else if (rule = RULE_DIVIDE_INTEGER)
	{	
		*type = DATA_TYPE_INTEGER;

		if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
		{
			return error_semantic_compatibility; // sematicka chyba, nekompatibilne datove typy
		}
	
	}
	else
	{
	}
	
	if (first_operand_to_float = 1)
	{
		GENERATE_CODE(generate_stack_op1_to_float); // docasna funckia
	}

	if (third_operand_to_float = 1)
	{
		GENERATE_CODE(generate_stack_op1_to_integer); // docasna funckia
	}

	return SYNTAX_OK;

}

static int reduce_by_rule (PData* data)
{

	Symbol_stack_item* first_operand = NULL;
	Symbol_stack_item* second_operand = NULL;
	Symbol_stack_item* third_operand = NULL;

	Rules_enumeration generation_rule;

	Data_type type;
	
	(void) data;	

	bool stop = false;

	int count = count_after_stop(&stop);


	if (count == 1 && stop)
	{
		first_operand = stack.top;
		rule_for_code_gen = check_rule(count, first_operand, NULL, NULL);
	}
	else if (count == 3 && stop)
	{
		first_operand = stack.top->next->next;
		second_operand = stack.top->next;
		third_operand = stack.top;

		generation_rule = check_rule (count, first_operand, second_operand, third_operand);
	}
	else
	{
		return error_syntax;
	}
	
	if (generation_rule == RULE_NOT_DEFINED)
	{
		return error_syntax;
	}
	else
	{
		if ((result = test_semantics (rule_for_code_gen, first_operand, second_operand, third_operand, &type)))
		{
			return result;
		}

		if (rule_for_code_gen == RULE_PLUS && final_type == DATA_TYPE_STRING)
		{
//			GENERATE_CODE(generate_concat_stack_strings);
		}
		else
		{ 
//			GENERATE_CODE(generate_stack_operation, rule_for_code_gen);
		}

		symbol_stack_pop_count(&stack, count + 1);
		symbol_stack_push(&stack, NON_TERM, final_type);
	}

	return SYNTAX_OK;
}


int expression(PData* data)
{
	int return_for_analysis = error_syntax;

	symbol_stack_init(&Stack);

	if (!symbol_stack_push(&stack, DOLLAR, DATA_TYPE_NOT_DEFINED))
	{
		FREE_RESOURCES_RETURN(error_internal);
	}

	Symbol_stack_item* stack_top;
	Symbol_enumeration actual_symbol;


	for (int condition = 0; condition == 0;)
	{
		actual_symbol = get_symbol_from_token(&data->token);
		stack_top = symbol_stack_top_terminal(&stack);

		if (stack_top == NULL)
		{
			FREE_RESOURCES_RETURN(error_internal); 
		}
	
		if (precedence_table[get_index(stack_top->symbol)][get_index(actual_symbol)] == ERROR)
		{
			if (stack_top->symbol != DOLLAR || actual_symbol != DOLLAR)
			{
					FREE_RESOURCES_RETURN(error_syntax);	
			}
			else
			{
				condition = 1;
			}
		}
		else if (precedence_table[get_index(stack_top->symbol)][get_index(actual_symbol)] == MATCH)
		{
			symbol_stack_push(&stack, actual_symbol, get_data_type(&data->token, data));

			if ((return_for_analysis = get_next_token(&data->token)))
			{
				FREE_RESOURCES_RETURN(return_for_analysis);
			}
		}
		else if (precedence_table[get_index(top_stack_terminal->symbol)][get_index(actual_symbol)] == REDUC)
		{
			if ((return_for_analysis = reduce_by_rule(data)))
			{
				FREE_RESOURCES_RETURN(return_for_analysis);
			}
		}
		else if (precedence_table[get_index(stack_top->symbol)][get_index(actual_symbol)] == SHIFT)
		{
			if (!symbol_stack_insert_after_top_terminal(&stack, SYMBOL_STOP, DATA_TYPE_NOT_DEFINED))
			{
				FREE_RESOURCES_RETURN(error_internal);
			}

			if(!symbol_stack_push(&stack, actual_symbol, get_type(&data->token, data)))
			{
				FREE_RESOURCES_RETURN(error_internal);
			}

			if (actual_symbol == SYMBOL_IDENTIFIER || actual_symbol == SYMBOL_INTEGER || actual_symbol == SYMBOL_FLOAT || actual_symbol == SYMBOL_STRING)
			{
//				GENERATE_CODE(generate_push, data->token);
			}

			if ((return_for_analysis = get_token(&data->token)))
			{
				FREE_RESOURCES_RETURN(return_for_analysis);
			}
		}
	}

	Symbol_stack_item* final_non_terminal = symbol_stack_top(&stack);

	if (final_non_terminal == NULL)
	{
		FREE_RESOURCES_RETURN(error_internal);
	}

	if (final_non_terminal->symbol != SYMBOL_NOT_DEFINED)
	{
		FREE_RESOURCES_RETURN(error_internal);
	}

	if (data->lhs_id != NULL)
	{
		char *frame = "LF";
		
		if (data->lhs_id->global) 
		{
			frame = "GF";
		}
		
		if (data->lhs_id->type == TYPE_INT)
		{
			if (final_non_terminal->data_type == DATA_TYPE_STRING)
			{
				FREE_RESOURCES_RETURN(error_semantic_compatibility);
			}
			
//			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, DATA_TYPE_INT, frame);
		}
		else if (data->lhs_id->global ==  DATA_TYPE_FLOAT)
		{
			if (final_non_terminal->data_type == DATA_TYPE_STRING)
			{
				FREE_RESOURCES_RETURN(error_semantic_compability);
			}
		
//			GENERATE_CODE(generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, DATA_TYPE_FLOAT, frame);
		}
		else if (data->lhs_id->type == DATA_TYPE_STRING)
		{
			if (final_non_terminal->data_type != DATA_TYPE_STRING)
			{
				FREE_RESOURCES_RETURN(semantic_compability);
			}
		
//			GENERATE_CODE (generate_save_expression_result, data->lhs_id->identifier, DATA_TYPE_STRING, DATA_TYPE_STRING, frame);
		}
		else if (data->lhs_id->type == DATA_TYPE_NOT_DEFINED)
		{
//			GENERATE_CODE (generate_save_expression_result, data->lhs_id->identifier, final_non_terminal->data_type, DATA_TYPE_NOT_DEFINED, frame);
		}
	}
	FREE_RESOURCES_RETURN(semantic_syntax_accepted);	
}
