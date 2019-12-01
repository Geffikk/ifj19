/************************ EXPRESSION ************************
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief  : Expression
***********************************************************/

#include "scanner.h"
#include "expression.h"
#include "error.h"
#include "expression_stack.h"
#include "analyza.h"
#include "symtable.h"
#include "code_generator.h"

Expression_stack stack_exp;

int precedence_table[12][12] =
{
//                                                       INPUT TOKEN
//<--------------------------------------------------------------->
//            | == | != | <= | >= | <  | >  |+,- |*/,//| ) | (  |data|  $ |
/* =  */  {ERR, ERR, ERR, ERR, ERR, ERR, SFT, SFT, RED, SFT, SFT, RED},  // =
/* != */  {ERR, ERR, ERR, ERR, ERR, ERR, SFT, SFT, RED, SFT, SFT, RED},  // !=
/* <= */  {ERR, ERR, ERR, ERR, ERR, ERR, SFT, SFT, RED, SFT, SFT, RED},  // <=
/* >= */  {ERR, ERR, ERR, ERR, ERR, ERR, SFT, SFT, RED, SFT, SFT, RED},  // >=
/* <  */  {ERR, ERR, ERR, ERR, ERR, ERR, SFT, SFT, RED, SFT, SFT, RED},  // <
/* > */   {ERR, ERR, ERR, ERR, ERR, ERR, SFT, SFT, RED, SFT, SFT, RED},  // >
/* +- */  {RED, RED, RED, RED, RED, RED, RED, SFT, RED, SFT, SFT, RED},  // +, -
/* */     {RED, RED, RED, RED, RED, RED, RED, RED, RED, SFT, SFT, RED},  // *,/,//
/* |*/    {RED, RED, RED, RED, RED, RED, RED, RED, RED, ERR, ERR, RED},  // )
/* |*/    {SFT, SFT, SFT, SFT, SFT, SFT, SFT, SFT, MCH, SFT, SFT, ERR},  // (
/* |*/    {RED, RED, RED, RED, RED, RED, RED, RED, RED, ERR, ERR, RED},  // data
/* E|*/   {SFT, SFT, SFT, SFT, SFT, SFT, SFT, SFT, ERR, SFT, SFT, ERR}   // $


};

/** Function for transforming symbol to index
 *
 *  @param - sytmbol from token
 *  @return - index for precedence algorithm
 ***/
static Index_enumeration get_Index(Symbol_enumeration symbol) // priradi index z precedencnej tabulky
{
    if (symbol == SYMBOL_EQUAL)
    {
        return INDEX_EQUAL;
    }
    else if (symbol == SYMBOL_NOT_EQUAL)
    {
        return INDEX_NOT_EQUAL;
    }
    else if (symbol == SYMBOL_LESS_EQUAL)
    {
        return INDEX_LESS_EQUAL;
    }
    else if (symbol == SYMBOL_MORE_EQUAL)
    {
        return INDEX_MORE_EQUAL;
    }
    else if (symbol == SYMBOL_LESS)
    {
        return INDEX_LESS;
    }
    else if (symbol == SYMBOL_MORE)
    {
        return INDEX_MORE;
    }
    else if (symbol == SYMBOL_PLUS || symbol == SYMBOL_MINUS)
    {
        return INDEX_PLUS_OR_MINUS;
    }
    else if (symbol == SYMBOL_MULTIPLY || symbol == SYMBOL_DIVIDE || symbol == SYMBOL_DIVIDE_INTEGER) // SYMBOL_MULTIPLY, SYMBOL_DIVIDE, SYMBOL_DIVIDE_INTEGER
    {
        return INDEX_MULTIPLY_OR_DIVIDE;
    }
    else if (symbol == SYMBOL_RIGHT_BRACKET) // SYMBOL_RIGHT_BRACKET
    {
        return INDEX_RIGHT_BRACKET;
    }
    else if (symbol == SYMBOL_LEFT_BRACKET) // SYMBOL_LEFT_BRACKET
    {
        return INDEX_LEFT_BRACKET;
    }
    else if (symbol == SYMBOL_IDENTIFIER || symbol == SYMBOL_INTEGER || symbol == SYMBOL_FLOAT || symbol == SYMBOL_STRING) // SYMBOL_IDENTIFIER, SYMBOL_INTEGER, SYMBOL_FLOAT, SYMBOL_STRING
    {
        return INDEX_DATA;
    }
    else	// SYMBOL_DOLLAR
    {
        return INDEX_DOLLAR;
    }
}

/** Function for transforming  token to symbol
 *
 *  @param token - token received from analysis
 *  @return - symbol for other functions
 ***/
static Symbol_enumeration get_Symbol (Token* token) // v Pdata v analyze mame zadefinovany token, ktory patres posiela
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
	/*else if (token->type == token_type_ )
	{
		return SYMBOL_DIVIDE_INTEGER;
	}*/
	else if (token->type == token_type_right_bracket)
	{
		return SYMBOL_RIGHT_BRACKET;
	}
	else if (token->type == token_type_left_bracket )
	{
		return SYMBOL_LEFT_BRACKET;
	}
	else if (token->type == token_type_int)
	{
		return SYMBOL_INTEGER;
	}
	else if (token->type == token_type_float )
	{
		return SYMBOL_FLOAT;
	}
	else if (token->type == token_type_str )
	{
		return SYMBOL_STRING;
	}
	else if (token->type == token_type_identifier )
	{
		return SYMBOL_IDENTIFIER;
	}
	else
	{
		return SYMBOL_DOLLAR;
	}
}

/** Function for
 *
 *  @param -
 *  @return -
 ***/
static int count_after_stop (bool* stop_on_stack)
{
	int counter = 0;

    Expression_stack_entry* pointer = Expression_stack_top(&stack_exp);

	while (pointer != NULL)
	{
		if (pointer->symbol != SYMBOL_STOP)
		{
			*stop_on_stack = false; // nenasli sme stop na stacku
			counter++;
		}
		else
		{
			*stop_on_stack = true; //nasli sme stop na stacku
			break;
		}

		pointer = pointer->next;
	}

	return counter; // vraciamne pocet znakov po znaku stop na stacku
}

/** Function for
 *
 *  @param -
 *  @return -
 ***/
static Data_type get_data_Type (Token* token, Parser_data* data)
{		
	TData* symbol;

    if (token->type == token_type_int)
	{
		return DATA_TYPE_INTEGER;
	}
	else if (token->type == token_type_float)
	{
		return DATA_TYPE_FLOAT;
	}
	else if (token->type == token_type_str)
	{
		return DATA_TYPE_STRING;
	}
	else if (token->type == token_type_identifier)
	{
		symbol = sym_table_search(&data->global_table, token->attribute.s->string);

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
		return DATA_TYPE_NOT_DEFINED;
	}
}

/** Function for
 *
 *  @param -
 *  @return -
 ***/
static Rule_enumeration check_Rule (int count_of_operands, Expression_stack_entry* first_operand, Expression_stack_entry* second_operand, Expression_stack_entry* third_operand, Parser_data* data)
{

	if (count_of_operands == 1)
	{
		switch (first_operand->symbol)
		{
			case SYMBOL_IDENTIFIER:
                return RULE_OPERAND;
			case SYMBOL_INTEGER:
                data->left_side_id->type = DATA_TYPE_INTEGER;
                data->left_side_id->is_variable = true;
                return RULE_OPERAND;
			case SYMBOL_FLOAT:
                data->left_side_id->type = DATA_TYPE_FLOAT;
                data->left_side_id->is_variable = true;
                return RULE_OPERAND;
			case SYMBOL_STRING:
                data->left_side_id->type = DATA_TYPE_STRING;
				return RULE_OPERAND;
		    default:
                return RULE_NOT_DEFINED;
		}
	}
	else if (count_of_operands == 3)
	{
		if (first_operand->symbol == SYMBOL_NOT_DEFINED && third_operand->symbol == SYMBOL_NOT_DEFINED)
		{
			if (second_operand->symbol == SYMBOL_EQUAL)
			{
				return RULE_EQUAL; // rule E -> E = E
			}
			else if (second_operand->symbol == SYMBOL_NOT_EQUAL)
			{
				return RULE_NOT_EQUAL;
			}
			else if (second_operand->symbol == SYMBOL_LESS_EQUAL)
			{
				return RULE_LESS_EQUAL;
			}
			else if (second_operand->symbol == SYMBOL_MORE_EQUAL)
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
				return RULE_DIVIDE;
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

/** Function for
 *
 *  @param -
 *  @return -
 ***/
int semantic_test (Rule_enumeration rule, Expression_stack_entry* first_operand, Expression_stack_entry* second_operand, Expression_stack_entry* third_operand, Data_type* type_of_result)
{
    bool first_operand_to_float = false;
    bool third_operand_to_float = false;

/*******************************************************************************************/
/******************************ERROR_STATES*************************************************/
/*******************************************************************************************/

    if (rule == RULE_OPERAND)
    {
        if (first_operand == DATA_TYPE_NOT_DEFINED)
        {
            return (error_semantic_compatibility);   //error4
        }
    }
    else if (rule == RULE_BRACKETS)
    {
        if (second_operand->data_type == DATA_TYPE_NOT_DEFINED)
        {
            return (error_semantic_compatibility);   //error4
        }
    }
    else
    {
        if (first_operand->data_type == DATA_TYPE_NOT_DEFINED || third_operand->data_type == DATA_TYPE_NOT_DEFINED)
        {
            return (error_semantic_compatibility);   //error4
        }
        /*if ((rule == RULE_DIVIDE || rule == RULE_DIVIDE_INTEGER) && second_operand == 0)
        {
            return error_div_zero; //error6
        }
         */
    }

/*******************************************************************************************/
/******************************RULE_STATES**************************************************/
/*******************************************************************************************/
    if (rule == RULE_OPERAND)
	{
		*type_of_result = first_operand->data_type;
	}
	else if (rule == RULE_BRACKETS)
	{
		*type_of_result = second_operand->data_type;
	}
	else if (rule == RULE_EQUAL || rule == RULE_NOT_EQUAL || rule == RULE_LESS_EQUAL || rule == RULE_MORE_EQUAL || rule == RULE_LESS || rule == RULE_MORE)
	{
        *type_of_result = DATA_TYPE_INTEGER;

        if (first_operand->data_type !=  third_operand->data_type)
        {
            if ((first_operand->data_type == DATA_TYPE_STRING && second_operand->data_type != DATA_TYPE_STRING)||(first_operand->data_type != DATA_TYPE_STRING && second_operand->data_type == DATA_TYPE_STRING))
            {
                return (error_semantic_compatibility);   //error4
            }
            if (first_operand->data_type == DATA_TYPE_INTEGER || third_operand->data_type == DATA_TYPE_FLOAT)
            {
                first_operand_to_float = true;
            }

            if (first_operand->data_type == DATA_TYPE_FLOAT || third_operand->data_type == DATA_TYPE_FLOAT)
            {
                third_operand_to_float = true;
            }



        }
	}
	else if (rule == RULE_PLUS)
	{
		*type_of_result = DATA_TYPE_INTEGER;
        if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
        {
            if(first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type ==DATA_TYPE_STRING)
            {
                *type_of_result = DATA_TYPE_STRING;
                //konkatenacia
            }

            if ((first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type != DATA_TYPE_STRING) || (first_operand->data_type != DATA_TYPE_STRING && third_operand->data_type == DATA_TYPE_STRING))
            {
                return error_semantic_compatibility; //error4
            }

            if (first_operand->data_type == DATA_TYPE_INTEGER)
            {
                first_operand_to_float = true;
                *type_of_result = DATA_TYPE_FLOAT;
            }
            else if (third_operand->data_type == DATA_TYPE_INTEGER)
            {
                third_operand_to_float = true;
                *type_of_result = DATA_TYPE_FLOAT;
            }
        }

	}
	else if (rule == RULE_MINUS || rule == RULE_MULTIPLY)
	{
		*type_of_result = DATA_TYPE_INTEGER;
        if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type !=DATA_TYPE_INTEGER)
        {
            if(first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type ==DATA_TYPE_STRING)
            {
                return (error_semantic_compatibility);   //error4
            }

            if (first_operand->data_type == DATA_TYPE_INTEGER)
            {
                first_operand_to_float = true;
            }
            else if (third_operand->data_type == DATA_TYPE_INTEGER)
            {
                third_operand_to_float = true;
            }

            *type_of_result = DATA_TYPE_FLOAT;

        }
	}
	else if (rule == RULE_DIVIDE)
	{
		*type_of_result = DATA_TYPE_FLOAT;
        if(first_operand->data_type != DATA_TYPE_FLOAT || third_operand->data_type !=DATA_TYPE_FLOAT)
        {
            if(first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type == DATA_TYPE_STRING)
            {
                return (error_semantic_compatibility);   //error4
            }
            if(first_operand->data_type == DATA_TYPE_INTEGER)
            {
                first_operand_to_float = true;
            }
            if(third_operand->data_type == DATA_TYPE_INTEGER)
            {
                third_operand_to_float = true;
            }
        }
	}
	else if (rule == RULE_DIVIDE_INT)
	{
        *type_of_result = DATA_TYPE_INTEGER;

        if(first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type !=DATA_TYPE_INTEGER)
        {
            return (error_semantic_compatibility);   //error4
        }
    }

    if (first_operand_to_float == true)
    {
        printf("GENERATION: First operand to float!\n");
    }

    if (third_operand_to_float == true)
    {
        printf("GENERATION: Third operand to float!\n");
    }


    return good_expression_syntax;

}

/** Function for
 *
 *  @param -
 *  @return -
 ***/
static int reduce_by_rule (Parser_data* data)
{
// tri operandy, za ktore si mozeme dosadzovat znaky
    Expression_stack_entry* first_operand = NULL;
    Expression_stack_entry* second_operand = NULL;
    Expression_stack_entry* third_operand = NULL;

    Rule_enumeration generation_rule;

	Data_type type_of_result;
	
	(void) data;
    bool stop = false;
    int return_of_function = 0;
	int count = count_after_stop(&stop); //pocet znakov okrem stopu


	if (count == 1 && stop) // ak je pocet znakov po stop 1 a zaroven sme ho nasli
	{
		first_operand = stack_exp.top;
		generation_rule = check_Rule(count, first_operand, NULL, NULL, data);
	}
	else if (count == 3 && stop)
	{
		first_operand = stack_exp.top->next->next;
		second_operand = stack_exp.top->next;
		third_operand = stack_exp.top;

		generation_rule = check_Rule (count, first_operand, second_operand, third_operand, data);
	}
	else
	{
		return error_syntax;
	}

	if (generation_rule != RULE_NOT_DEFINED)
    {
		if ((return_of_function = semantic_test (generation_rule, first_operand, second_operand, third_operand, &type_of_result)))
		{
			return return_of_function;
		}
        if (generation_rule != RULE_PLUS && type_of_result != DATA_TYPE_STRING)
        {
            Gen_expr_calc (generation_rule);
            printf("GENERATION : Performing generation by the rule!\n");

        }

        if (generation_rule == RULE_PLUS && type_of_result == DATA_TYPE_STRING)
        {
            printf("GENERATION : Performing concatenation!\n");
        }

        Expression_stack_count_of_pop(&stack_exp, count + 1);
        Expression_stack_push(&stack_exp, SYMBOL_NOT_DEFINED, type_of_result);
	}
	else
    {
       return error_syntax;
    }

	return return_of_function;
}

/** Function for
 *
 *  @param -
 *  @return -
 ***/
int expression(Parser_data* data)
{
    Expression_stack_init(&stack_exp);  // inicializacia stacku
    Expression_stack_push(&stack_exp, SYMBOL_DOLLAR, DATA_TYPE_NOT_DEFINED); // vlozenie $ na stack top

    Expression_stack_entry* ter_on_stack_top;  // deklaracia ukazovatela na stack top
	Symbol_enumeration actual_symbol;  // deklaracia aktualneho symbolu

	int return_for_analysis = 0;

	for (int condition = 0; condition == 0;) // loop
	{
		actual_symbol = get_Symbol(&data->token);  // zistime, aky je aktualny symbol od patresa
		ter_on_stack_top = Expression_stack_top_ter(&stack_exp); //pozrie item na vrchu stacku, ci nie je STOP symbol

        if (ter_on_stack_top == NULL)
        {
            Expression_stack_free(&stack_exp);
        }
		else if (ter_on_stack_top->symbol == SYMBOL_IDENTIFIER)
        {
            TData* variable = sym_table_search(&data->global_table, data->token.attribute.s->string);
            ter_on_stack_top->data_type = variable->type;
        }


		int INDEX = precedence_table[get_Index(ter_on_stack_top->symbol)][get_Index(actual_symbol)]; //ziskam indexy actual symbol a stack top, dosadim ich do pola tabulky ako suradnice

		if (INDEX == SFT)
		{
			int result1 = Expression_stack_insert_after_top_ter(&stack_exp, SYMBOL_STOP, DATA_TYPE_NOT_DEFINED); // dosadim si stop symbol
			int result2 = Expression_stack_push(&stack_exp, actual_symbol, get_data_Type(&data->token, data)); // najprv ziskame data type prveho tokenu,
            int result3;
			if(!(result1 || result2))
            {
                Expression_stack_free(&stack_exp);
                return error_internal;
            }

            if (actual_symbol == SYMBOL_IDENTIFIER || actual_symbol == SYMBOL_INTEGER || actual_symbol == SYMBOL_FLOAT || actual_symbol == SYMBOL_STRING)
            {
                    printf("GENERATION: Pushning token!\n");
            }

            result3 = get_token(&data->token, NULL); // ziskanie dalsieho tokenu, pri prvom prejdeni +

            if(result3)
            {
//              Expression_stack_free(&stack_exp);
//              return return_for_analysis;
            }
		}
        else if (INDEX == RED) //zredukovanie vyrazu
        {
            return_for_analysis = reduce_by_rule(data);
            if (return_for_analysis)
            {
//              Expression_stack_free(&stack_exp);
//              return return_for_analysis;
            }
        }
        else if (INDEX == MCH)  // prava a lava zatvorka
        {
            Expression_stack_push(&stack_exp, actual_symbol, get_data_Type(&data->token, data));

            return_for_analysis = get_token(&data->token, NULL);

            if(return_for_analysis)
            {
//              Expression_stack_free(&stack_exp);
//              return return_for_analysis;
            }
        }
        else if (INDEX == ERR) // ostali nam iba znaky $
        {
            if (ter_on_stack_top->symbol == SYMBOL_DOLLAR && actual_symbol == SYMBOL_DOLLAR)
            {
                condition = 1; // spravne ukoncenie funkcie
            }
            else
            {
                Expression_stack_free(&stack_exp);
                return error_syntax;
            }
        }
	}

    Expression_stack_entry* final_non_ter = Expression_stack_top(&stack_exp);

	char *frame = "GF";

    if (final_non_ter == NULL)
    {
        Expression_stack_free(&stack_exp);
        return error_internal;
    }

    if (final_non_ter->symbol != SYMBOL_NOT_DEFINED)
    {
        Expression_stack_free(&stack_exp);
        return error_internal;
    }

    if (data->left_side_id->type == DATA_TYPE_INTEGER)
    {
        if (final_non_ter->data_type == DATA_TYPE_STRING)
        {
            Expression_stack_free(&stack_exp);
            return error_semantic_compatibility;
        }

        printf("GENERATION: Expression result, for integer!\n");

//		GENERATE_CODE(generate_save_expression_result, data->left_side_id->identifier, final_non_ter->data_type, DATA_TYPE_INT, frame);
    }
    else if (data->left_side_id->type == DATA_TYPE_FLOAT)
    {
        if (final_non_ter->data_type == DATA_TYPE_STRING)
        {
            Expression_stack_free(&stack_exp);
            return error_semantic_compatibility;
        }

        printf("GENERATION: Expression result, for float!\n");

//		GENERATE_CODE(generate_save_expression_result, data->left_side_id->identifier, final_non_ter->data_type, DATA_TYPE_FLOAT, frame);
    }
    else if (data->left_side_id->type == DATA_TYPE_STRING)
    {
        if (final_non_ter->data_type != DATA_TYPE_STRING)
        {
            Expression_stack_free(&stack_exp);
            return error_semantic_compatibility;
        }

        printf("GENERATION: Expression result, for string!\n");

//		GENERATE_CODE (generate_save_expression_result, data->left_side_id->identifier, DATA_TYPE_STRING, DATA_TYPE_STRING, frame);
    }
    else if (data->left_side_id->type == DATA_TYPE_NOT_DEFINED)
    {
        printf("GENERATION: Expression result, for not defined type!\n");
//			GENERATE_CODE (generate_save_expression_result, data->left_side_id->identifier, final_non_ter->data_type, DATA_TYPE_NOT_DEFINED, frame);
    }

    Expression_stack_free(&stack_exp);
    return 0;
}
