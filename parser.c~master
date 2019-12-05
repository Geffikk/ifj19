
/************************* PARSER *************************
 * @author : Patrik Tomov <xtomov02@stud.fit.vutbr.cz>
 * @author : Martin Valach <xvalac12@stuf.fit.vutbr.cz>
***********************************************************/

#include <stdlib.h>

#include "parser.h"
#include "expression_stack.h"
#include "code_generator.h"


tStack *stack; // zasobnik na indenty/dedenty
Expression_stack stack_exp; // zasobnik pre vyrazy


//MACROS FOR BETTER CLARITY
#define MACRO_GET_TOKEN()  if((result = get_token(&data->token, stack)) != token_scan_accepted) {   \
                            return result;                                                           \
                            }

#define MACRO_CHECK_KEYWORD(_keyword) if(data->token.type != token_type_keyword             \
                                        || data->token.attribute.keyword != (_keyword)){   \
                                        return error_syntax;                               \
                                        }

#define MACRO_CHECK_TYPE(_type) if(data->token.type != (_type)) {    \
                                return error_syntax;                 \
                                }

#define MACRO_CHECK_RULE(rule)  if((result = rule(data))) {      \
                                return result;                    \
                                }

#define MACRO_GET_TOKEN_AND_CHECK_KEYWORD(_keyword)  do {                                       \
                                                            MACRO_GET_TOKEN();                  \
                                                            MACRO_CHECK_KEYWORD(_keyword);        \
                                                     } while(0)


#define MACRO_GET_TOKEN_AND_CHECK_TYPE(_type)  do {                                 \
                                                    MACRO_GET_TOKEN();              \
                                                    MACRO_CHECK_TYPE(_type);        \
                                               } while(0)

#define MACRO_GET_TOKEN_AND_CHECK_RULE(rule)  do {                                  \
                                                    MACRO_GET_TOKEN();              \
                                                    MACRO_CHECK_RULE(rule);         \
                                               } while(0)

#define MACRO_VALUE(token)  (token).type == token_type_float     \
                            || (token).type == token_type_int    \
                            || (token).type == token_type_str    \
                            || (token).type == token_type_identifier



int precedence_table[12][12] =
{
//                                                       INPUT TOKEN
//<--------------------------------------------------------------->
//        | == | != | <= | >= | <  | >  |+,- |*/  | )  | (  |data|  $ |
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
    else if (symbol == SYMBOL_RIGHT_BRACKET)
    {
        return INDEX_RIGHT_BRACKET;
    }
    else if (symbol == SYMBOL_LEFT_BRACKET)
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
    else if (token->type == token_type_div_int)
    {
            return SYMBOL_DIVIDE_INTEGER;
    }
    else if (token->type == token_type_right_bracket)
    {
        return SYMBOL_RIGHT_BRACKET;
    }
    else if (token->type == token_type_left_bracket)
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
static Rule_enumeration check_Rule (int count_of_operands, Expression_stack_entry* first_operand, Expression_stack_entry* second_operand, Expression_stack_entry* third_operand)
{

    if (count_of_operands == 1)
    {
        switch (first_operand->symbol)
        {
            case SYMBOL_IDENTIFIER:
                return RULE_OPERAND;
            case SYMBOL_INTEGER:
                return RULE_OPERAND;
            case SYMBOL_FLOAT:
                return RULE_OPERAND;
            case SYMBOL_STRING:
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
                return RULE_DIVIDE_INT;
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
int semantic_test (Rule_enumeration rule, Expression_stack_entry* first_operand, Expression_stack_entry* second_operand, Expression_stack_entry* third_operand, Data_type* type_of_result, Parser_data* data)
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
        if (rule == RULE_DIVIDE /*|| rule == RULE_DIVIDE_INTEGER*/)
        {
            if (data->token.attribute.int_number == 0 || data->token.attribute.float_number == 0)
            {
                return error_div_zero; //error6
            }
        }
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
        Gen_cast_stack_op1 ();
        //printf("GENERATION: First operand to float!\n");
    }

    if (third_operand_to_float == true)
    {
        Gen_cast_stack_op2 ();
        //printf("GENERATION: Third operand to float!\n");
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
        generation_rule = check_Rule(count, first_operand, NULL, NULL);
    }
    else if (count == 3 && stop)
    {
        first_operand = stack_exp.top->next->next;
        second_operand = stack_exp.top->next;
        third_operand = stack_exp.top;

        generation_rule = check_Rule (count, first_operand, second_operand, third_operand);
    }
    else
    {
        return error_syntax;
    }

    if (generation_rule != RULE_NOT_DEFINED)
    {
        if ((return_of_function = semantic_test (generation_rule, first_operand, second_operand, third_operand, &type_of_result,data)))
        {
            return return_of_function;
        }

        if (generation_rule == RULE_PLUS && type_of_result == DATA_TYPE_STRING)
        {
            Gen_string_concat ();
            //printf("GENERATION : Performing concatenation!\n");
        } else{

            Gen_expr_calc (generation_rule);
            //printf("GENERATION : Performing generation by the rule!\n");


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
    Expression_stack_init(&stack_exp);  // inicializacia stacku pre vyrazy
    Expression_stack_push(&stack_exp, SYMBOL_DOLLAR, DATA_TYPE_NOT_DEFINED); // vlozenie $ na stack top kvoli precedencnemu algoritmu

    Expression_stack_entry* ter_on_stack_top;  // deklaracia ukazovatela na ter na stack tope
    Symbol_enumeration actual_symbol;  // deklaracia aktualneho symbolu z vyrazu

    int return_for_analysis = 0;    // konecne navratova hodnota vyrazov

    for (int condition = 0; condition == 0;) // loop pre precedencni algoritmus
    {
        actual_symbol = get_Symbol(&data->token);  // zistime, aky je symbol poslany z analyzy

        ter_on_stack_top = Expression_stack_top_ter(&stack_exp); //pozrie ter na top stacku, ci to nie je STOP symbol

        if (ter_on_stack_top == NULL) // nie je ziadnu ter na vrchu stacku
        {
            Expression_stack_free(&stack_exp);
        }
        else if (ter_on_stack_top->symbol == SYMBOL_IDENTIFIER) // ak ter je identifikator, tak pozrie ci v tabulke uz nie je v globalnej tabulke
        {
            if(data->in_function == true)
            {
                TData* variable = sym_table_search(&data->local_table, data->token.attribute.s->string);
                ter_on_stack_top->data_type = variable->type;
            }
            else{
                TData* variable = sym_table_search(&data->global_table, data->token.attribute.s->string);
                ter_on_stack_top->data_type = variable->type;
            }
        }

        int INDEX = precedence_table[get_Index(ter_on_stack_top->symbol)][get_Index(actual_symbol)]; //ziskam indexy actual symbol a stack top, dosadim ich do pola tabulky ako suradnice

        if (INDEX == SFT)
        {
            int result1 = Expression_stack_insert_after_top_ter(&stack_exp, SYMBOL_STOP, DATA_TYPE_NOT_DEFINED); // dosadim si stop symbol, ktoy nahradzuje < v precedencnej analyze
            int result2 = Expression_stack_push(&stack_exp, actual_symbol, get_data_Type(&data->token, data)); // najprv ziskame data type prveho tokenu,

            if(!(result1 || result2)) // ak je aspon jedno z nich pravda, vyhodi error
            {
                Expression_stack_free(&stack_exp);
                return error_internal;
            }

            if (actual_symbol == SYMBOL_IDENTIFIER || actual_symbol == SYMBOL_INTEGER || actual_symbol == SYMBOL_FLOAT || actual_symbol == SYMBOL_STRING)
            {
                if (actual_symbol == SYMBOL_IDENTIFIER)
                {
                    if (sym_table_search(&data->local_table, data->token.attribute.s->string) != NULL) {
                        Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,4));
                    } else if (sym_table_search(&data->global_table, data->token.attribute.s->string) != NULL) {
                        Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,3));
                    } else {
                        //error;
                    }
                }
                else if (actual_symbol == SYMBOL_INTEGER)
                {
                    Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,0));
                }
                else if (actual_symbol == SYMBOL_FLOAT)
                {
                    Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,1));
                }
                else
                {
                    Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,5));
                }

                //printf("GENERATION: Pushning token!\n");
            }

            return_for_analysis = get_token(&data->token, NULL); // ziskanie dalsieho tokenu

            if(return_for_analysis)
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
            }
        }
        else if (INDEX == RED) // zredukovanie vyrazu pomocou daneho pravidla
        {
            return_for_analysis = reduce_by_rule(data);
            if (return_for_analysis)
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
            }
        }
        else if (INDEX == MCH)  // pravidlo, ked ostala len prava a lava zatvorka
        {
            Expression_stack_push(&stack_exp, actual_symbol, get_data_Type(&data->token, data));

            return_for_analysis = get_token(&data->token, NULL);

            if(return_for_analysis)
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
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

    //char *frame = "GF";

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
    if( data->left_side_id != NULL)
    {
        if (data->left_side_id->type == DATA_TYPE_NOT_DEFINED)
        {
            data->left_side_id->type = final_non_ter->data_type;
        }
    }
    //  Gen_save_expr_or_retval(data->left_side_id->identifier);

    Expression_stack_free(&stack_exp);
    return 0;
}















/**
 **********************************************<PROG_BODY>************************************************
 * @param data
 * @return exit code
 */
int prog_body(Parser_data* data)
{
    int result;


    //Chyba indent na prvom riadku
    if(data->token.type == token_type_indent)
    {
        return error_syntax;
    }

    //<prog_body> ->   def id ( <par_list> ) : EOL <statement>  <prog_body>
    if (data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_def)
    {
        data->in_function = true;

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_identifier);

        //GENEROVANIE HLAVICKY
        Gen_function_def_head(data->token.attribute.s->string);


        //funkcia sa nemoze volat ako skor definovana globalna premenna
        if(sym_table_search(&data->global_table, data->token.attribute.s->string))
        {
            return error_semantic;
        }

        //Add id to the global symbol table
        bool internal_error;
        data->current_process_id = sym_table_add_symbol(&data->global_table, data->token.attribute.s->string, &internal_error);
        if(!data->current_process_id)
        {
            if (internal_error)
            {
                return error_internal;
            } else{
                return error_semantic;
            }
        }

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
        MACRO_GET_TOKEN_AND_CHECK_RULE(par_list);
        MACRO_CHECK_TYPE(token_type_right_bracket);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_colon);

        //osetrenie medzier
        MACRO_GET_TOKEN();

        while(data->token.type == token_type_EOL)
        {
            if(data->token.type == token_type_EOL)
            {
                MACRO_CHECK_TYPE(token_type_EOL);
            }
            MACRO_GET_TOKEN();
        }


        MACRO_CHECK_TYPE(token_type_indent);
        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);


        // Current function is defined
        data->current_process_id->defined = true;
        data->current_process_id->is_function = true;


        Gen_function_def_foot(data->current_process_id->identifier);

        // clear local symtable
        sym_table_free(&data->local_table);

        // get next token and execute <prog_body> rule
        MACRO_GET_TOKEN();
        data->in_function = false; // musi tu byt aby nastavilo false ak je definicia v maine
        return prog_body(data);
    }
    // <prog_body> ->  EOL  <prog_body>
    else if(data->token.type == token_type_EOL)
    {
        MACRO_GET_TOKEN();
        return prog_body(data);
    }
    // <prog_body> -> <main_body>
    else
    {
        //podmienka na to aby sa mohli definovat funckie v maine
        if(data->was_in_main == true)
        {
            return token_scan_accepted;
        }
        return main_body(data);
    }

    return token_scan_accepted;
}

/**
 **********************************************<MAIN_BODY>************************************************
 * @param data
 * @return exit code
 */
int main_body(Parser_data* data)
{
    int result;
    data->was_in_main = true;

    //check if all functios are define
    for(int i = 0; i < MAX_SYMTABLE_SIZE; i++)
        for(Sym_table_entry* it = data->global_table[i]; it !=NULL; it = it->next)
            if(!it->data.defined) return error_semantic;

    //we are in main now
    data->in_function = false;
    data->current_process_id = NULL;


    MACRO_CHECK_RULE(statement);

    if(data->token.type != token_type_EOL || data->token.type != token_type_EOF)
    {
        if(data->token.type == token_type_EOF)
        {
            //clear global symbol table
            sym_table_free(&data->global_table);
            return end_main(data);
        }
        MACRO_CHECK_RULE(statement);
    }

    //clear global symbol table
    sym_table_free(&data->global_table);

    //execute <end_main> rule
    return end_main(data);

}

/**
 **********************************************<END_MAIN>************************************************
 * @param data
 * @return exit code
 */
int end_main(Parser_data* data)
{
    int result;

    // <main_body> -> EOL <end_main>
    if (data->token.type == token_type_EOL)
    {
        MACRO_GET_TOKEN();
        return end_main(data);
    }

    // <end> -> EOF
    if (data->token.type == token_type_EOF)
    {
        return token_scan_accepted;
    }

    return error_syntax;
}


int par_list(Parser_data* data)
{
    int result;
    //int counter = 0;
    //char* array_of_parameters[100];

    data->param_index = 0;

    // <par_list> ->   id  <par_list2>
    if (data->token.type == token_type_identifier)
    {
        //if there is function named as parameter
        if(sym_table_search(&data->global_table, data->token.attribute.s->string))
        {
            return error_semantic;
        }

        //add param to the local symbol table
        bool internal_error;
        if(!(data->right_side_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error)))
        {
            if(internal_error)
            {
                return error_internal;
            } else{
                return error_semantic;
            }
        }

        if(!sym_table_add_parameter(data->current_process_id, DATA_TYPE_NOT_DEFINED))
        {
            return error_internal;
        }

        /*
        while(data->token.type != token_type_right_bracket) {
            array_of_parameters[counter] = data->token.attribute.s->string;
            printf("%s\n", array_of_parameters[counter]);
            counter++;
            MACRO_GET_TOKEN_AND_CHECK_RULE(par_list2);
        }

        counter--;

        while(counter >= 0)
        {
            Gen_pop_arg(Term_adjustment( array_of_parameters[counter], 4));
            counter--;

        }
        */
        MACRO_GET_TOKEN_AND_CHECK_RULE(par_list2);
    }

    // <par_list> ->  ε

    return token_scan_accepted;
}



/**
 **********************************************<PARAM_N>************************************************
 * @param data
 * @return exit code
 */
int par_list2(Parser_data* data)
{
    int result;

    // <param_n> -> , id <par_list2>
    if(data->token.type == token_type_comma)
    {
        data->param_index++;

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_identifier);

        bool internal_error;
        if(!(data->right_side_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error)))
        {
            if (internal_error){
                return error_internal;
            } else{
                return error_semantic;
            }
        }

        if(!sym_table_add_parameter(data->current_process_id, DATA_TYPE_NOT_DEFINED))
        {
            return error_internal;
        }

        MACRO_GET_TOKEN()
        return par_list2(data);
        //return token_scan_accepted;
    }

    return token_scan_accepted;
    // <par_list2> -> ε

    //return token_scan_accepted;

}



/**
 **********************************************<STATEMENT>************************************************
 * @param data
 * @return exit code
 */
int statement(Parser_data* data)
{
    int result;

    //Chyba indent na prvom riadku
    if(data->token.type == token_type_indent)
    {
        return error_syntax;
    }

    // <statement> ->    if <expression> : EOL <statement> else : EOL <statement> EOL    <statement>
    if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_if)
    {

        data->in_while_or_if = true;



        MACRO_GET_TOKEN_AND_CHECK_RULE(expression);

        Gen_if_head();

        MACRO_CHECK_TYPE(token_type_colon);

        //osetrenie medzier
        MACRO_GET_TOKEN();
        while(data->token.type == token_type_EOL)
        {
            if(data->token.type == token_type_EOL)
            {
                MACRO_CHECK_TYPE(token_type_EOL);
            }
            MACRO_GET_TOKEN();
        }
        MACRO_CHECK_TYPE(token_type_indent);


        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);
        MACRO_GET_TOKEN_AND_CHECK_KEYWORD(keyword_else);

        Gen_else_head();

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        //MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);

        //osetrenie medzier
        MACRO_GET_TOKEN();

        while(data->token.type == token_type_EOL)
        {
            if(data->token.type == token_type_EOL)
            {
                MACRO_CHECK_TYPE(token_type_EOL);
            }
            MACRO_GET_TOKEN();
        }
        MACRO_CHECK_TYPE(token_type_indent);


        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);

        Gen_else_foot();

        data->in_while_or_if = false;

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }


    // <statement> ->   while <expression> : EOL <statement> EOL  <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_while)
    {

        data->in_while_or_if = true;

        //
        Gen_while_label();

        MACRO_GET_TOKEN_AND_CHECK_RULE(expression);

        Gen_while_head();

        MACRO_CHECK_TYPE(token_type_colon);

        //osetrenie medzier
        MACRO_GET_TOKEN();

        while(data->token.type == token_type_EOL)
        {
            if(data->token.type == token_type_EOL)
            {
                MACRO_CHECK_TYPE(token_type_EOL);
            }
            MACRO_GET_TOKEN();
        }

        MACRO_CHECK_TYPE(token_type_indent);


        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);

        Gen_while_foot();

        data->in_while_or_if = false;

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }


    // <statement>  id = <def_value> EOL  <statement>
    else if(data->token.type == token_type_identifier)
    {

        int number_to_term_function = 0;

        //test ci sa premenna nevola ako skor definovana funkcia a viacnasobne pouzitie GLOBALNEJ premennej
        if(data->in_function == false)
        {
            TData* function_or_variable = sym_table_search(&data->global_table, data->token.attribute.s->string);

            if(function_or_variable != NULL && function_or_variable->is_function == true)
            {
                return error_semantic;
            }
            else if(function_or_variable != NULL && function_or_variable->is_function == false)
            {
                MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_assign);
                MACRO_GET_TOKEN_AND_CHECK_RULE(def_value);

                //GENEROVANIE 2
                Gen_save_expr_or_retval(Term_adjustment(data->left_side_id->identifier, number_to_term_function));

                MACRO_CHECK_TYPE(token_type_EOL);

                // get next token and execute <statement> rule
                MACRO_GET_TOKEN();
                return statement(data);
            }

        }

        //Ak sme vo funkcii tak pridame premennu do lokalnej tabulky
        bool internal_error;
        if(data->in_function)
        {

            //ak este nebola definovana premenna ta ju pridame do lokalnej tabulky inak uz bola pouzita cize preskocime
            TData* var = sym_table_search(&data->local_table, data->token.attribute.s->string);
            if(var == NULL)
            {
                data->left_side_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error);
                if(!data->left_side_id)
		{
                   	if(internal_error)
			{
                   	     return error_internal;
                   	}
			else
			{	
                        	return error_semantic;
			}
		}
            } 
	    else
	    {
                //viacnasobne pouzitie premennev vo funkcii
                data->left_side_id = var;
            }
        }
        //Ak niesme tak pridame premennu do globalnej tabulky
        else
	{

            //ak este nebola definovana premenna ta ju pridame do globalnej tabulky inak uz bola pouzita cize preskocime
            TData* var = sym_table_search(&data->global_table, data->token.attribute.s->string);
            if(var == NULL)
            {
                data->left_side_id = sym_table_add_symbol(&data->global_table, data->token.attribute.s->string, &internal_error);
                if(!data->left_side_id){
                    if(internal_error)
                    {
                        return error_internal;
                    } else{
                        return error_semantic;
                    }
                }
                data->left_side_id->is_variable = true;
            }
        }

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_assign);
        MACRO_GET_TOKEN_AND_CHECK_RULE(def_value);

        //priradenie premennej ramec
        if(data->in_function == true)
        {
            data->left_side_id->global = false;
            number_to_term_function = 4;
        }
        else{
            data->left_side_id->global = true;
            number_to_term_function = 3;
        }
        data->left_side_id->defined = true;


        /*
        //GENEROVANIE PRI PRVOM PRIRADENI
        if(data->left_side_id->global == true)
        {
            Gen_var_def(Term_adjustment(data->left_side_id->identifier, number_to_term_function));
        }
        else{
            Gen_var_def(Term_adjustment(data->left_side_id->identifier, number_to_term_function));
        }
        */

        Gen_var_def(Term_adjustment(data->left_side_id->identifier, number_to_term_function));

        //GENEROVANIE 2
        Gen_save_expr_or_retval(Term_adjustment(data->left_side_id->identifier, number_to_term_function));


        if(data->token.type == token_type_EOF)
        {
            return token_scan_accepted;
        }
        MACRO_CHECK_TYPE(token_type_EOL);



        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }

    // <statement> ->   print ( <term> , <print_rule> ) EOL   <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_print)
    {
        int result;

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);

        MACRO_GET_TOKEN();
        if(data->token.type == token_type_int || data->token.type == token_type_float || data->token.type == token_type_str || data->token.type == token_type_identifier)
        {
            //test na identifikator v printe ci je definovany ak nie tak chyba
            if(data->token.type == token_type_identifier)
            {
                if(data->in_function == true)
                {
                    TData* variable_in_global_table;
                    variable_in_global_table = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    data->left_side_id = sym_table_search(&data->local_table, data->token.attribute.s->string);
                    if(data->left_side_id == NULL && variable_in_global_table == NULL)
                    {
                        return error_semantic;
                    }
                }
                else{
                    data->left_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    if(data->left_side_id == NULL)
                    {
                        return error_semantic;
                    }
                }
            }


            MACRO_GET_TOKEN_AND_CHECK_RULE(print_rule);
        }
        else if(data->token.type == token_type_right_bracket)
        {

            MACRO_CHECK_TYPE(token_type_right_bracket);
            MACRO_GET_TOKEN();
            if(data->token.type == token_type_EOF)
            {
                return token_scan_accepted;
            }
            MACRO_CHECK_TYPE(token_type_EOL);
        }

        MACRO_GET_TOKEN();
        return statement(data);
    }


    // <statement> ->   return <expression> EOL   <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_return)
    {


        //return moze byt len v tele funkcii nie v hlavnom programe
        if(!data->in_function) return error_syntax;

        MACRO_GET_TOKEN_AND_CHECK_RULE(expression);

        //GENEROVANIE
        Gen_return();

        MACRO_CHECK_TYPE(token_type_EOL);

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }


    // <statement>  pass EOL  <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_pass)
    {
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);

        MACRO_GET_TOKEN();
        return statement(data);
    }
        // <statement> -> ε
    else if(data->token.type == token_type_EOL)
    {
        MACRO_GET_TOKEN();
        return statement(data);
    }
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_def)
    {
        return prog_body(data);
    }
    else if(data->token.type == token_type_indent)
    {
        return error_syntax;
    }

    return token_scan_accepted;
}

/**
 **********************************************<DEF_VALUE>************************************************
 * @param data
 * @return exit code
 */
int def_value(Parser_data* data)
{
    int result;

    if(data->token.type == token_type_identifier || data->token.type == token_type_keyword)
    {

        //ak je na zaciatku vyrazu premenna napr b = a + 3
        TData* var = sym_table_search(&data->global_table, data->token.attribute.s->string);
        if(var == NULL)
        {
            return error_semantic;
        }
        else if(var->is_variable == true && var != NULL)
        {
            MACRO_CHECK_RULE(expression);
            return token_scan_accepted;
        }

        // <def_value> -> id ( <arg_list> )
        if(data->token.type == token_type_identifier)
        {
            data->right_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
        }

        if(data->token.type == token_type_keyword)
        {
            switch(data->token.attribute.keyword)
            {
                    // <def_value> -> ord ( <arg_list> )
                case keyword_ord:
                    data->right_side_id = sym_table_search(&data->global_table, "ord");
                    data->left_side_id->type = DATA_TYPE_INTEGER;
                    break;

                    // <def_value> -> chr ( <arg_list> )
                case keyword_chr:
                    data->right_side_id = sym_table_search(&data->global_table, "chr");
                    data->left_side_id->type = DATA_TYPE_STRING;
                    break;

                    // <def_value> -> len ( <arg_list> )
                case keyword_len:
                    data->right_side_id = sym_table_search(&data->global_table, "len");
                    data->left_side_id->type = DATA_TYPE_INTEGER;
                    break;

                    // <def_value> -> substr ( <arg_list> )
                case keyword_substr:
                    data->right_side_id = sym_table_search(&data->global_table, "substr");
                    data->left_side_id->type = DATA_TYPE_STRING;
                    break;

                    // <def_value> -> inputs()
                case keyword_inputs:
                    data->right_side_id = sym_table_search(&data->global_table, "inputs");
                    data->left_side_id->type = DATA_TYPE_STRING;
                    break;

                    // <def_value> -> inputi()
                case keyword_inputi:
                    data->right_side_id = sym_table_search(&data->global_table, "inputi");
                    data->left_side_id->type = DATA_TYPE_INTEGER;
                    break;

                    // <def_value> -> inputf()
                case keyword_inputf:
                    data->right_side_id = sym_table_search(&data->global_table, "inputf");
                    data->left_side_id->type = DATA_TYPE_FLOAT;
                    break;

                case keyword_None:
                    data->left_side_id->type = DATA_TYPE_NOT_DEFINED;

                    MACRO_GET_TOKEN();
                    return token_scan_accepted;

                default:
                    return error_syntax;
            }
        }

        if(data->right_side_id)
        {

            //ak mame identifikator (funkciu) alebo vestavene funkcie len, ord, chr, substr tak kontrolujeme aj argumenty
            if(data->token.type == token_type_identifier || data->token.attribute.keyword == keyword_ord ||
            data->token.attribute.keyword == keyword_chr || data->token.attribute.keyword == keyword_len ||
            data->token.attribute.keyword == keyword_substr)
            {
                MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
                MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list);
                MACRO_CHECK_TYPE(token_type_right_bracket);

            //ak mame vestavene funkcie inputs(), inputi(), inputf() tak kontrolujeme len zatvorky
            } else{
                MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
                MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_right_bracket);
            }


            MACRO_GET_TOKEN();

            //pocet celkovych parameterov sa musi rovnat poctu spracovanych parametrov
            if(data->right_side_id->params->length_of_lexem_string != data->param_index)
                return error_semantic_bad_count_param;


            return token_scan_accepted;
        }

        data->right_side_id = sym_table_search(&data->local_table, data->token.attribute.s->string);
        if(!data->right_side_id)
        {
            return error_semantic;
        }
    }

    // <def_value> -> <expression>
    MACRO_CHECK_RULE(expression);

    return token_scan_accepted;
}

/**
 **********************************************<ARG>************************************************
 * @param data
 * @return exit code
 */
int arg_list(Parser_data* data)
{
    int result;

    // currently processed argument
    data->param_index = 0;

    // <arg_list> -> <value> <arg_list2>
    // if token is value
    if(MACRO_VALUE(data->token))
    {
        MACRO_CHECK_RULE(value);
        MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list2);
    }

    // <arg_list> -> ε

    return token_scan_accepted;
}

/**
 **********************************************<ARG_N>************************************************
 * @param data
 * @return exit code
 */
int arg_list2(Parser_data* data)
{
    int result;

    // <arg_list2> -> , <value> <arg_list-n>
    if(data->token.type == token_type_comma)
    {
        MACRO_GET_TOKEN_AND_CHECK_RULE(value);
        MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list2);
    }

    // <arg_list2> ->  ε

    return token_scan_accepted;
}

/**
 **********************************************<VALUE>************************************************
 * Description: Check types of arg_list
 * @param data
 * @return exit code
 */
int value(Parser_data* data)
{
    //check number of arguments
    if(data->right_side_id->params->length_of_lexem_string == data->param_index) // ma tu byt == ale este nemam spravene pridavanie parametrov
    {
        return error_semantic_compatibility;
    }

    switch(data->token.type)
    {
        // <value> -> DOUBLE
        case token_type_float:
            data->right_side_id->params->string[data->param_index] = 'f';
            break;
        // <value> -> INT
        case token_type_int:
            data->right_side_id->params->string[data->param_index] = 'i';
            break;
        // <value> -> STRING
        case token_type_str:
            /*
            if(data->right_side_id->params->string[data->param_index] != 's')
                return error_semantic_compatibility;
            */
            data->right_side_id->params->string[data->param_index] = 's';
            break;

        // <value> -> IDENTIFIER
        case token_type_identifier:; // ; C evil magic
            TData* id = sym_table_search(&data->local_table, data->token.attribute.s->string);
            if(!id) return error_semantic;

            switch(id->type)
            {
                case DATA_TYPE_INTEGER:
                    data->right_side_id->params->string[data->param_index] = 'i';
		    break;

                case DATA_TYPE_FLOAT:
                    data->right_side_id->params->string[data->param_index] = 'f';
                    break;

                case DATA_TYPE_STRING:
                    data->right_side_id->params->string[data->param_index] = 's';
                    break;

                default:
                    return error_internal;
            }
            break;
        default:
            return error_syntax;
    }

    // increment argument position
    data->param_index++;

    return token_scan_accepted;
}

/**
 **********************************************<PRINT_RULE>************************************************
 * @param data
 * @return exit code
 */
int print_rule(Parser_data* data)
{
    int result;


    if(data->token.type == token_type_comma)
    {
        MACRO_CHECK_TYPE(token_type_comma);
        MACRO_GET_TOKEN();
        if(data->token.type == token_type_int || data->token.type == token_type_float || data->token.type == token_type_str || data->token.type == token_type_identifier)
        {
            //test na identifikator v printe ci je definovany ak nie tak chyba
            if(data->token.type == token_type_identifier)
            {
                if(data->in_function == true)
                {
                    TData* variable_in_global_table;
                    variable_in_global_table = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    data->left_side_id = sym_table_search(&data->local_table, data->token.attribute.s->string);
                    if( (data->left_side_id == NULL) && (variable_in_global_table == NULL) )
                    {
                        return error_semantic;
                    }
                }
                else
		{
                    data->left_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    if(data->left_side_id == NULL)
                    {
                        return error_semantic;
                    }
                }
            }

            MACRO_GET_TOKEN_AND_CHECK_RULE(print_rule);
        }


    }
    else if(data->token.type == token_type_right_bracket)
    {
        MACRO_CHECK_TYPE(token_type_right_bracket);
        MACRO_GET_TOKEN();
        if(data->token.type == token_type_EOF)
        {
            return token_scan_accepted;
        }
        MACRO_CHECK_TYPE(token_type_EOL);

        return token_scan_accepted;
    }


	return 0;
}

/**
 **********************************************INIT_VARIABLES************************************************
 * @param data
 * @return true if everything ok, otherwise false
 */
bool init_variables(Parser_data* data)
{
    sym_table_init(&data->global_table);
    sym_table_init(&data->local_table);

    data->current_process_id = NULL;
    data->left_side_id = NULL;
    data->right_side_id = NULL;

    data->param_index = 0;

    data->in_function = false;
    data->in_while_or_if = false;

    // init IFJ19 functions
    bool internal_error;
    TData* id;

    //len(s)
    id = sym_table_add_symbol(&data->global_table, "len", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = DATA_TYPE_INTEGER;
    if(!sym_table_add_parameter(id, DATA_TYPE_STRING))
        return false;


    //substr(s, i, n)
    id = sym_table_add_symbol(&data->global_table, "substr", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = DATA_TYPE_STRING;
    if(!sym_table_add_parameter(id, DATA_TYPE_STRING))
        return false;
    if(!sym_table_add_parameter(id, DATA_TYPE_INTEGER))
        return false;
    if(!sym_table_add_parameter(id, DATA_TYPE_INTEGER))
        return false;


    //ord(s, i)
    id = sym_table_add_symbol(&data->global_table, "ord", &internal_error);
    if(internal_error)
        return false;

    id->defined = true;
    id->type = DATA_TYPE_INTEGER;
    if (!sym_table_add_parameter(id, DATA_TYPE_STRING))
        return false;
    if (!sym_table_add_parameter(id, DATA_TYPE_INTEGER))
        return false;


    //chr(i)
    id = sym_table_add_symbol(&data->global_table, "chr", &internal_error);
    if(internal_error)
        return false;

    id->defined = true;
    id->type = DATA_TYPE_STRING;
    if (!sym_table_add_parameter(id, DATA_TYPE_INTEGER))
        return false;

    //inputs()
    id = sym_table_add_symbol(&data->global_table, "inputs", &internal_error);
    if(internal_error)
        return false;

    id->defined = true;
    id->type = DATA_TYPE_STRING;


    //inputi()
    id = sym_table_add_symbol(&data->global_table, "inputi", &internal_error);
    if(internal_error)
        return false;

    id->defined = true;
    id->type = DATA_TYPE_INTEGER;


    //inputf()
    id = sym_table_add_symbol(&data->global_table, "inputf", &internal_error);
    if(internal_error)
        return false;

    id->defined = true;
    id->type = DATA_TYPE_FLOAT;

    return true;
}

/**
 **********************************************FREE_VARIABLES************************************************
 * @param data
 */
void free_variables(Parser_data* data)
{
    sym_table_free(&data->global_table);
    sym_table_free(&data->local_table);
}

/**
 **********************************************ANALYZA************************************************
 * @return exit code
 */
int analyza()
{
    int result;

    Parser_data parser_data;
    stack = (tStack*) malloc(sizeof(tStack));
    stackInit(stack);
    set_stack(stack);
    Lexem_string string;
    lexem_string_init(&IFJcode19);

    if (!lexem_string_init(&string))
    {
        return error_internal;
    }
    set_string(&string);

    if (!init_variables(&parser_data))
    {
        lexem_string_clear(&string);
        return error_internal;
    }



    if ((result = get_token(&parser_data.token, stack)) == token_scan_accepted)
    {
        //GENEROVANIE
        //Gen_Start();
        result = prog_body(&parser_data);

    }

    printf("%s", IFJcode19.string);

    lexem_string_clear(&string);
    free_variables(&parser_data);

    return result;
}






















