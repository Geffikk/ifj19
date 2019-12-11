
/****************** Syntactic and semantic analisis **************
 * @author : Patrik Tomov <xtomov02@stud.fit.vutbr.cz>
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ19
 * @brief  : Implementation of parser
****************************************************************/

#include <stdlib.h>
#include "parser.h"
#include "expression_stack.h"
#include "code_generator.h"
#include "stack.h"
#include <stdbool.h>

tStack *stack; /// stack for indents and dedents
Expression_stack stack_exp; /// stack for expressions
tStack_Param *stack_param;

///MACROS FOR BETTER CLARITY
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

/// function for empty lines
int processing_EOLs(Parser_data* data)
{
    int result;
    while(data->token.type == token_type_EOL)
    {
        if(data->token.type == token_type_EOL)
        {
            MACRO_CHECK_TYPE(token_type_EOL);
        }
        MACRO_GET_TOKEN();
    }
    return 0; ///?????????
}

int precedence_table[15][15] =
        {
//       |                                                                                      INPUT TOKEN
//-------|------------------------------------------------------------------------------------------------->
//       |          | == | != | <= | >= | <  | >  |  + |  - | *  |  / | // |  ) | (  |type| $ |
/*       |      */  {END, END, END, END, END, END, LSS, LSS, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // =
/*       |      */  {END, END, END, END, END, END, LSS, LSS, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // !=
/*       |      */  {END, END, END, END, END, END, LSS, LSS, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // <=
/*    T  |  S   */  {END, END, END, END, END, END, LSS, LSS, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // >=
/*    E  |  T   */  {END, END, END, END, END, END, LSS, LSS, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // <
/*    R  |  A   */  {END, END, END, END, END, END, LSS, LSS, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // >
/*    M  |  C   */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // +, -
/*    I  |  K   */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, LSS, LSS, LSS, GRT, LSS, LSS, GRT},  // +, -
/*    N  |      */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, LSS, LSS, GRT},  // *
/*    A  |  T   */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, LSS, LSS, GRT},  // /
/*    L  |  O   */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, LSS, LSS, GRT},  // //
/*       |  P   */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, END, END, GRT},  // )
/*    O  |      */  {LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, MCH, LSS, LSS, END},  // (
/*    N  |      */  {GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, GRT, END, END, GRT},  // id, int, double, string, None (type)
/*      \|/     */  {LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, LSS, END, LSS, LSS, END}   // $
        };


/** Function for transforming token from scanner to symbol
 *
 *  @param token - token received from analysis
 *  @return - symbol for other function
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
    else if (token->attribute.keyword == keyword_None && token->type != token_type_EOL && token->type != token_type_colon && token->type != token_type_EOF)
    {
        return SYMBOL_NONE;
    }
    else
    {
        return SYMBOL_DOLLAR;
    }
}

/** Function for transforming symbol to index for work with precedence table
 *
 *  @param - symbol from token
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
    else if (symbol == SYMBOL_PLUS)
    {
        return INDEX_PLUS;
    }
    else if (symbol == SYMBOL_MINUS)
    {
        return INDEX_MINUS;
    }
    else if (symbol == SYMBOL_MULTIPLY)
    {
        return INDEX_MULTIPLY;
    }
    else if (symbol == SYMBOL_DIVIDE)
    {
        return INDEX_DIVIDE;
    }
    else if (symbol == SYMBOL_DIVIDE_INTEGER)
    {
        return INDEX_DIVIDE_INTEGER;
    }
    else if (symbol == SYMBOL_RIGHT_BRACKET)
    {
        return INDEX_RIGHT_BRACKET;
    }
    else if (symbol == SYMBOL_LEFT_BRACKET)
    {
        return INDEX_LEFT_BRACKET;
    }
    else if (symbol == SYMBOL_IDENTIFIER || symbol == SYMBOL_INTEGER || symbol == SYMBOL_FLOAT || symbol == SYMBOL_STRING || symbol == SYMBOL_NONE)
    {
        return INDEX_DATA;
    }
    else
    {
        return INDEX_DOLLAR;
    }
}

/** Function for counting symbols after stop
 *
 *  @param - variable, which will change when stop is find
 *  @return - count of symbols after stop
 ***/
static int count_after_stop (bool* stop_on_stack)
{
    int counter = 0;

    Expression_stack_entry* pointer = Expression_stack_top(&stack_exp);

    while (pointer != NULL)
    {
        if (pointer->symbol != SYMBOL_STOP)
        {
            *stop_on_stack = false;             /// stop symbol was not find yet
            counter++;
        }
        else
        {
            *stop_on_stack = true;             /// stop symbol was find
            break;
        }

        pointer = pointer->next;
    }

    return counter;
}

/** Function for converting type of token to data type
 *
 *  @param token - type from scanner
 *  @param data - parser structure data
 *  @return - data type for semantic tests
 ***/
static Data_type get_data_Type (Token* token, Parser_data* data)
{
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
    else if (token->type == token_type_keyword && token->attribute.keyword == keyword_None)
    {
        return DATA_TYPE_NONE;
    }
    else if (token->type == token_type_identifier)
    {
        IData* symbol = sym_table_search(&data->local_table, token->attribute.s->string);

        if (symbol == NULL)
        {
            symbol = sym_table_search(&data->global_table, token->attribute.s->string);
            if (symbol == NULL)
            {
                return DATA_TYPE_NOT_DEFINED;
            }
            else
            {
                return symbol->type;
            }
        }
        else
        {
            return symbol->type;
        }
    }
    else
    {
        return DATA_TYPE_NOT_DEFINED;
    }

}

/** Function for assign rule by used operands
 *
 *  @param first_operand - first operand of expression
 *  @param second_operand - second operand of expression
 *  @param third_operand - third operand of expression
 *  @param count_of_operands - Number of symbols in expression
 *  @return - rule for code generation and semantic tests
 ***/
static Rule_enumeration check_Rule (Expression_stack_entry* first_operand, Expression_stack_entry* second_operand, Expression_stack_entry* third_operand, int count_of_operands)
{

    if (count_of_operands == 1)
    {
        switch (first_operand->symbol)
        {
            case SYMBOL_IDENTIFIER:
            case SYMBOL_INTEGER:
            case SYMBOL_FLOAT:
            case SYMBOL_STRING:
            case SYMBOL_NONE:
                return RULE_OPERAND;
            default:
                return RULE_NOT_DEFINED;
        }
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
    else
    {
        return RULE_NOT_DEFINED;
    }
    return RULE_NOT_DEFINED;


}

/** Function for semantics test and retype of operands by rules
 *
 *  @param first_operand - first operand of expression
 *  @param second_operand - second operand of expression
 *  @param third_operand - third operand of expression
 *  @param type_of_result - data type of operation
 *  @param data - parser structure data
 *  @param rule - type of operation
 *  @return - return good exit code or error
 ***/
static int semantic_test (Expression_stack_entry* first_operand, Expression_stack_entry* second_operand, Expression_stack_entry* third_operand, Data_type* type_of_result, Parser_data* data, Rule_enumeration rule)
{
    bool first_operand_to_float = false;
    bool third_operand_to_float = false;
    *type_of_result = DATA_TYPE_INTEGER;

    if (data->in_function == false)
    {
        if (rule == RULE_OPERAND)
        {
            if (first_operand == NULL)
            {
                return error_internal;
            }
            else if (first_operand->data_type == DATA_TYPE_NOT_DEFINED)
            {
                return error_semantic;
            }
        }
        else if (rule == RULE_BRACKETS)
        {
            if (second_operand == NULL)
            {
                return error_internal;
            }
            else if (second_operand->data_type == DATA_TYPE_NOT_DEFINED)
            {
                return error_semantic;
            }
        }
        else
        {
            if (first_operand == NULL || third_operand == NULL)
            {
                return error_internal;
            }
            else if (first_operand->data_type == DATA_TYPE_NOT_DEFINED || third_operand->data_type == DATA_TYPE_NOT_DEFINED)
            {
                return error_semantic;
            }
            else if ((first_operand->data_type == DATA_TYPE_NONE || third_operand->data_type == DATA_TYPE_NONE) && (rule != RULE_EQUAL && rule != RULE_NOT_EQUAL))
            {
                return error_semantic_compatibility;
            }
        }
    }
    else if(data->in_function == true)
    {
        if (rule == RULE_OPERAND)
        {
            if (first_operand == NULL)
            {
                return error_internal;
            }
        }
        else if (rule == RULE_BRACKETS)
        {
            if (second_operand == NULL)
            {
                return error_internal;
            }
            else if(data->in_function == true && data->left_side_id != NULL)
            {
                Gen_type_control(Term_adjustment(data->left_side_id->identifier,4),Term_adjustment(data->token.attribute.s->string,4));
            }
            else
            {
                //return (error_semantic_compatibility);   //error4
            }
        }
        else
        {
            if (first_operand == NULL || third_operand == NULL)
            {
                return error_internal;
            }
            else if ((first_operand->data_type == DATA_TYPE_NONE || third_operand->data_type == DATA_TYPE_NONE) && (rule != RULE_EQUAL && rule != RULE_NOT_EQUAL))
            {
                return error_semantic_compatibility;
            }
        }
    }

    if (rule == RULE_OPERAND)
    {
        *type_of_result = first_operand->data_type;
    }
    else if (rule == RULE_BRACKETS)
    {
        *type_of_result = second_operand->data_type;
    }
    else if (rule == RULE_EQUAL || rule == RULE_NOT_EQUAL)
    {
        if (first_operand->data_type == DATA_TYPE_INTEGER && third_operand->data_type == DATA_TYPE_FLOAT)
        {
            first_operand_to_float = true;
        }
        else if (first_operand->data_type == DATA_TYPE_FLOAT && third_operand->data_type == DATA_TYPE_INTEGER)
        {
            third_operand_to_float = true;
        }
    }

    if (data->in_function == false || (first_operand != DATA_TYPE_NOT_DEFINED && second_operand != DATA_TYPE_NOT_DEFINED))  /// semantic test if is it in main body or if are both operands defined
    {
        if (rule == RULE_LESS_EQUAL || rule == RULE_MORE_EQUAL || rule == RULE_LESS || rule == RULE_MORE)
        {
            if (first_operand->data_type != third_operand->data_type)
            {
                if (first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type == DATA_TYPE_STRING)
                {
                    return error_semantic_compatibility;   //error4
                }
                if (first_operand->data_type == DATA_TYPE_INTEGER && third_operand->data_type == DATA_TYPE_FLOAT)
                {
                    first_operand_to_float = true;
                }
                else if (first_operand->data_type == DATA_TYPE_FLOAT && third_operand->data_type == DATA_TYPE_INTEGER)
                {
                    third_operand_to_float = true;
                }
            }
        }
        else if (rule == RULE_PLUS)
        {
            if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
            {
                if (first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type == DATA_TYPE_STRING)
                {
                    *type_of_result = DATA_TYPE_STRING;
                }

                if (((first_operand->data_type == DATA_TYPE_STRING && third_operand->data_type != DATA_TYPE_STRING) || (first_operand->data_type != DATA_TYPE_STRING && third_operand->data_type == DATA_TYPE_STRING)))
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
            if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
            {
                if (first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type == DATA_TYPE_STRING)
                {
                    return error_semantic_compatibility;   //error4
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
            if (first_operand->data_type != DATA_TYPE_FLOAT || third_operand->data_type != DATA_TYPE_FLOAT)
            {
                if (first_operand->data_type == DATA_TYPE_STRING || third_operand->data_type == DATA_TYPE_STRING)
                {
                    return error_semantic_compatibility;   //error4
                }
                if (first_operand->data_type == DATA_TYPE_INTEGER)
                {
                    first_operand_to_float = true;
                }
                if (third_operand->data_type == DATA_TYPE_INTEGER)
                {
                    third_operand_to_float = true;
                }
            }
        }
        else if (rule == RULE_DIVIDE_INT)
        {
            if (first_operand->data_type != DATA_TYPE_INTEGER || third_operand->data_type != DATA_TYPE_INTEGER)
            {
                return error_semantic_compatibility;
            }
        }

        if (first_operand_to_float == true)
        {
            Gen_cast_stack_op1();
        }

        if (third_operand_to_float == true)
        {
            Gen_cast_stack_op2();
        }
    }
    return good_expression_syntax;

}

/** Function for reducing expression by precedence algorithm rules
 *
 *  @param data - parser structure data
 *  @return - return good exit code or error
 ***/
static int expression_reduction (Parser_data* data)
{
    Expression_stack_entry* first_operand = NULL;
    Expression_stack_entry* second_operand = NULL;
    Expression_stack_entry* third_operand = NULL;

    Rule_enumeration generation_rule;
    Data_type type_of_result;

    bool stop = false;
    int return_of_function = 0;
    int count = count_after_stop(&stop);


    if (count == 1 && stop) /// for rule E->i
    {
        first_operand = stack_exp.top;
        generation_rule = check_Rule(first_operand, NULL, NULL, count);
    }
    else if (count == 3 && stop) /// for other rules
    {
        first_operand = stack_exp.top->next->next;
        second_operand = stack_exp.top->next;
        third_operand = stack_exp.top;

        generation_rule = check_Rule (first_operand, second_operand, third_operand, count);
    }
    else
    {
        return error_syntax;
    }

    if (generation_rule != RULE_NOT_DEFINED)
    {
        if ((return_of_function = semantic_test (first_operand, second_operand, third_operand, &type_of_result, data, generation_rule)))
        {
            return return_of_function;
        }

        if (generation_rule == RULE_PLUS && type_of_result == DATA_TYPE_STRING)
        {
            Gen_string_concat (); /// generation of concatenation
        }
        else if (generation_rule != RULE_OPERAND && generation_rule != RULE_BRACKETS)
        {
            if (data->in_function == true)
            {
                Gen_type_control (); /// function for check of semantic in case of running errors
            }
            Gen_expr_calc (generation_rule, data->in_function);  ///  generation of other operation than concatanation
        }

        Expression_stack_count_of_pop(&stack_exp, count + 1);                       /// pop of useless stack entries
        Expression_stack_push(&stack_exp, type_of_result, SYMBOL_NOT_DEFINED);     /// push of E to stack instead of operation
    }
    else
    {
        return error_syntax;
    }

    return return_of_function;
}

/** Function for implementation shift of operands in expression by precedence algorithm
 *
 *  @param data- parser structure data
 *  @param actual_symbol - input symbol from token
 *  @return - return good exit code or error
 ***/
static int expression_shift(Parser_data* data, Symbol_enumeration actual_symbol)
{
    int result1 = Expression_stack_after_top_ter(&stack_exp, DATA_TYPE_NOT_DEFINED, SYMBOL_STOP); // dosadim si stop symbol, ktoy nahradzuje < v precedencnej analyze
    int result2 = Expression_stack_push(&stack_exp, get_data_Type(&data->token, data), actual_symbol); // najprv ziskame data type prveho tokenu,

    if (!(result1 || result2))
    {
        Expression_stack_free(&stack_exp);
        return error_internal;
    }

    /// if actual_symbol is id or some data type, it is generate push for it
    if (actual_symbol == SYMBOL_IDENTIFIER || actual_symbol == SYMBOL_INTEGER || actual_symbol == SYMBOL_FLOAT || actual_symbol == SYMBOL_STRING || actual_symbol == SYMBOL_NONE)
    {
        if (actual_symbol == SYMBOL_IDENTIFIER)
        {
            if (sym_table_search(&data->local_table, data->token.attribute.s->string) != NULL)
            {
                Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,4));
            }
            else if (sym_table_search(&data->global_table, data->token.attribute.s->string) != NULL)
            {
                Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,3));
            }
            else
            {
                if (data->in_function == true)
                {
                    Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,4));
                }
                else
                {
                    Expression_stack_free(&stack_exp);
                    return error_semantic;
                }
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
        else if (actual_symbol == SYMBOL_NONE)
        {
            Gen_push_stack_op (Term_adjustment("None",2));
        }
        else
        {
            Gen_push_stack_op (Term_adjustment(data->token.attribute.s->string,5));
        }
    }
    return good_expression_syntax;
}

/** Function for implementation expression rule from LL table
 *
 *  @param - parser structure data
 *  @return - return good exit code or error
 ***/
static int expression(Parser_data* data)
{
    int return_for_analysis = 0; /// return for code analysis in parser

    Expression_stack_init(&stack_exp);
    Expression_stack_push(&stack_exp, DATA_TYPE_NOT_DEFINED, SYMBOL_DOLLAR ); /// insert $ symbol on stack top for precedence algorithm

    Expression_stack_entry* ter_on_stack_top;
    Symbol_enumeration actual_symbol;

    for (bool condition = false; condition == false;) /// precedence algorithm loop
    {
        if ((ter_on_stack_top = Expression_stack_top_ter(&stack_exp)) == NULL)
        {
            Expression_stack_free(&stack_exp);
            return error_internal;
        }
        actual_symbol = get_Symbol(&data->token);

        int precedence_Index = precedence_table[get_Index(ter_on_stack_top->symbol)][get_Index(actual_symbol)]; //ziskam indexy actual symbol a stack top, dosadim ich do pola tabulky ako suradnice

        if (precedence_Index == LSS) /// rule < : adds symbol stop on stack and read another symbol from input
        {
            if ((return_for_analysis = expression_shift(data, actual_symbol)))
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
            }
            else if ((return_for_analysis = get_token(&data->token, NULL)))
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
            }
        }
        else if (precedence_Index == GRT) /// rule > : reduce by precedence algorithm
        {
            return_for_analysis = expression_reduction(data);
            if (return_for_analysis)
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
            }
        }
        else if (precedence_Index == MCH) /// rule = : push symbol in brackets, read next token from input
        {
            Expression_stack_push(&stack_exp, get_data_Type(&data->token, data), actual_symbol);

            return_for_analysis = get_token(&data->token, NULL);

            if (return_for_analysis)
            {
                Expression_stack_free(&stack_exp);
                return return_for_analysis;
            }
        }
        else if (precedence_Index == END) /// rule for end of algorithm or error
        {
            if (ter_on_stack_top->symbol == SYMBOL_DOLLAR && actual_symbol == SYMBOL_DOLLAR)
            {
                condition = true; /// success of algorithm
            }
            else
            {
                Expression_stack_free(&stack_exp);
                return error_syntax;
            }
        }
    }

    Expression_stack_entry* final_not_ter = Expression_stack_top(&stack_exp);

    if (final_not_ter == NULL)
    {
        Expression_stack_free(&stack_exp);
        return error_internal;
    }

    if (final_not_ter->symbol != SYMBOL_NOT_DEFINED)
    {
        Expression_stack_free(&stack_exp);
        return error_internal;
    }

    if (data->left_side_id != NULL)
    {
        if (data->left_side_id->type == DATA_TYPE_NOT_DEFINED)
        {
            data->left_side_id->type = final_not_ter->data_type; /// assign data type to variable
        }
    }

    Expression_stack_free(&stack_exp);
    return 0;
}











/** Processing <prog_body> rule
 *
 * @param data - Parser data
 * @return - return 0, otherwise error
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

        //funkcia sa nemoze volat ako skor definovana globalna premenna
        if(sym_table_search(&data->global_table, data->token.attribute.s->string))
        {
            return error_semantic;
        }

        //Generovanie hlavicky funkcie
        Gen_function_def_head(data->token.attribute.s->string);

        //Pridanie funkcie do globalnej tabulky
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
        processing_EOLs(data);
        MACRO_CHECK_TYPE(token_type_indent);

        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);

        //Koniec spracovania funkcie
        data->current_process_id->defined = true;
        data->current_process_id->is_function = true;

        //Generovanie paty fuknkcie
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
        processing_EOLs(data);
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

}

/** Processing <main_body> rule
 *
 * @param data - Parser data
 * @return - return <end_main>
 */
int main_body(Parser_data* data)
{
    int result;
    data->was_in_main = true;

    //check if all functios are define
    for(int i = 0; i < MAX_SYMTABLE_SIZE; i++)
        for(Sym_table_item* it = data->global_table[i]; it != NULL; it = it->next)
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

/** Processing <end_main> rule
 *
 * @param data - Parser data
 * @return - if it is OK return 0, otherwise error
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

/** Processing <par_list1> rule
 *
 * @param data - Parser data
 * @return - if processed parameter is OK return 0, otherwise error
 */
int par_list(Parser_data* data)
{
    int result;

    // <par_list> ->   id  <par_list2>
    if (data->token.type == token_type_identifier) {

        data->param_index = 0;

        //if there is function named as parameter
        if (sym_table_search(&data->global_table, data->token.attribute.s->string)) {
            return error_semantic;
        }

        //add param to the local symbol table
        bool internal_error;
        if (!(data->right_side_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error))) {
            if (internal_error) {
                return error_internal;
            } else {
                return error_semantic;
            }
        }


        if (!sym_table_add_parameter(data->current_process_id, DATA_TYPE_NOT_DEFINED)) {
            return error_internal;
        }

        data->right_side_id->is_variable = true;

        Lexem_string temp;
        Lexem_string *tmp = &temp;
        copy_lexem_string_to_attribute_string(data->token.attribute.s, tmp);


        stackPush_param(stack_param, tmp->string);

        MACRO_GET_TOKEN_AND_CHECK_RULE(par_list2);
    }

    // <par_list> ->  ε
    return token_scan_accepted;
}

/** Processing <par_list2> rule
 *
 * @param data - Parser data
 * @return - if processed parameters are OK return 0, otherwise error
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

        data->right_side_id->is_variable = true;
        Lexem_string temp;
        Lexem_string *tmp = &temp;
        copy_lexem_string_to_attribute_string(data->token.attribute.s, tmp);


        stackPush_param(stack_param, tmp->string);

        MACRO_GET_TOKEN_AND_CHECK_RULE(par_list2);
    }

    //Popovanie parametrov generatoru odzadu
    while(stackEmpty_param(stack_param) != 1)
    {
        char* param;
        param = stackTop_param(stack_param);
        Gen_pop_arg(Term_adjustment(param, 4));
        stackPop_param(stack_param);

    }
    // <par_list2> -> ε
    return token_scan_accepted;
}

/** Processing <statement> rule
 *
 * @param data - Parser data
 * @return - if processed statement is OK return 0, otherwise error
 */
int statement(Parser_data* data)
{
    int result;
    int number_to_term_function;

    //Chyba indent na prvom riadku
    if(data->token.type == token_type_indent)
    {
        return error_syntax;
    }

    /// <statement> ->    if <expression> : EOL <statement> else : EOL <statement> EOL    <statement>
    if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_if)
    {
        data->in_while_or_if = true;

        MACRO_GET_TOKEN_AND_CHECK_RULE(expression);

        Gen_if_head();

        MACRO_CHECK_TYPE(token_type_colon);

        //osetrenie medzier
        MACRO_GET_TOKEN();
        processing_EOLs(data);
        MACRO_CHECK_TYPE(token_type_indent);


        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);
        MACRO_GET_TOKEN_AND_CHECK_KEYWORD(keyword_else);

        Gen_else_head();

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_colon);

        //osetrenie medzier
        MACRO_GET_TOKEN();
        processing_EOLs(data);
        MACRO_CHECK_TYPE(token_type_indent);


        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);

        Gen_else_foot();

        data->in_while_or_if = false;

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }


    /// <statement> ->   while <expression> : EOL <statement> EOL  <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_while)
    {

        data->in_while_or_if = true;

        Gen_while_label();

        MACRO_GET_TOKEN_AND_CHECK_RULE(expression);

        Gen_while_head();

        MACRO_CHECK_TYPE(token_type_colon);

        //osetrenie medzier
        MACRO_GET_TOKEN();
        processing_EOLs(data);
        MACRO_CHECK_TYPE(token_type_indent);

        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);

        Gen_while_foot();

        data->in_while_or_if = false;

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }

    /// <statement>  id = <def_value> EOL  <statement>
    else if(data->token.type == token_type_identifier)
    {
        //osetrenie na riadku ->  nazov_funkcie()
        data->right_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
        if(data->right_side_id != NULL && data->right_side_id->is_function == true)
        {
            MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
            MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list);
            MACRO_CHECK_TYPE(token_type_right_bracket);

            Gen_function_call(data->right_side_id->identifier);

            MACRO_GET_TOKEN();
            return statement(data);
        }

        //test ci sa premenna nevola ako skor definovana funkcia a viacnasobne pouzitie GLOBALNEJ premennej
        if(data->in_function == false)
        {
            IData* function_or_variable = sym_table_search(&data->global_table, data->token.attribute.s->string);
            data->left_side_id = function_or_variable;

            if(function_or_variable != NULL && function_or_variable->is_function == true)
            {
                return error_semantic;
            }
            else if(function_or_variable != NULL && function_or_variable->is_function == false)
            {

                MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_assign);
                MACRO_GET_TOKEN_AND_CHECK_RULE(def_value);

                if(data->left_side_id->global == false)
                {
                    number_to_term_function = 4;
                } else{
                    number_to_term_function = 3;
                }
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

        }

        //Ak sme vo funkcii tak pridame premennu do lokalnej tabulky
        bool internal_error;
        if(data->in_function)
        {
            //ak este nebola definovana premenna ta ju pridame do lokalnej tabulky inak uz bola pouzita cize preskocime
            IData* var = sym_table_search(&data->local_table, data->token.attribute.s->string);
            if(var == NULL)
            {
                data->left_side_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error);
                if(!data->left_side_id)
                    if(internal_error)
                        return error_internal;
                    else
                        return error_semantic;
            } else{
                //viacnasobne pouzitie premennev vo funkcii
                data->left_side_id = var;
            }
            data->left_side_id->is_variable = true;
        }
        //Ak niesme tak pridame premennu do globalnej tabulky
        else{

            //ak este nebola definovana premenna ta ju pridame do globalnej tabulky inak uz bola pouzita cize preskocime
            IData* var = sym_table_search(&data->global_table, data->token.attribute.s->string);
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


        if(data->left_side_id->defined == false)
        {
            //Generovanie 1
            Gen_var_def(Term_adjustment(data->left_side_id->identifier, number_to_term_function));
            data->left_side_id->defined = true;
        }

        //Generovanie 2
        Gen_save_expr_or_retval(Term_adjustment(data->left_side_id->identifier, number_to_term_function));

        if(data->token.type == token_type_EOF)
        {
            return token_scan_accepted;
        }
        else if(data->token.type == token_type_dedent)
        {
            return token_scan_accepted;
        }
        MACRO_CHECK_TYPE(token_type_EOL);

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }

    /// <statement> ->   print ( <term> , <print_rule> ) EOL   <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_print)
    {
        int result;

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);

        MACRO_GET_TOKEN();
        if(data->token.type == token_type_int || data->token.type == token_type_float || data->token.type == token_type_str || data->token.type == token_type_identifier)
        {

            switch(data->token.type)
            {
                case token_type_str:
                    number_to_term_function = 5;
                    break;
                case token_type_int:
                    number_to_term_function = 0;
                    break;
                case token_type_float:
                    number_to_term_function = 1;
                    break;
            }

            //test na identifikator v printe ci je definovany ak nie tak chyba
            if(data->token.type == token_type_identifier)
            {
                if(data->in_function == true)
                {
                    IData* variable_in_global_table;
                    variable_in_global_table = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    data->left_side_id = sym_table_search(&data->local_table, data->token.attribute.s->string);
                    if(data->left_side_id == NULL && variable_in_global_table == NULL)
                    {
                        return error_semantic;
                    }
                    number_to_term_function = 4;
                }
                else{
                    data->left_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    if(data->left_side_id == NULL)
                    {
                        return error_semantic;
                    }
                    number_to_term_function = 3;
                }
            }

            Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));

            Gen_function_call("print");

            MACRO_GET_TOKEN_AND_CHECK_RULE(print_rule);
        }
        else if(data->token.type == token_type_right_bracket)
        {
            MACRO_CHECK_TYPE(token_type_right_bracket);

            //prazdny print() -> generovanie medzeri
            Gen_push_arg(Term_adjustment("\n", 5));

            Gen_function_call("print");

            MACRO_GET_TOKEN();
            if(data->token.type == token_type_EOF)
            {
                return token_scan_accepted;
            }
            MACRO_CHECK_TYPE(token_type_EOL);

            return token_scan_accepted;
        }

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }
    /// <statement> ->   return <expression> EOL   <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_return)
    {
        //return moze byt len v tele funkcii nie v hlavnom programe
        if(!data->in_function) return error_syntax;

        MACRO_GET_TOKEN_AND_CHECK_RULE(expression);

        //Generovanie
        Gen_return();

        MACRO_CHECK_TYPE(token_type_EOL);

        // get next token and execute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }
    /// <statement>  pass EOL  <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_pass)
    {

        // get next token and exexute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }
    /// <statement> -> def <prog_body>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_def)
    {
        return prog_body(data);
    }
    /// <statement> -> EOL <statement>
    else if(data->token.type == token_type_EOL)
    {
        processing_EOLs(data);
        return statement(data);
    }
    /// <statement> -> <value> <expression>
    else if(data->token.type == token_type_int || data->token.type == token_type_float || data->token.type == token_type_str)
    {
        return expression(data);
    }
    /// <statement> -> id (<arg_list>, <arg_list2>) <statement>
    else if(data->token.type == token_type_keyword)
    {
        data->right_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
        MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list);
        MACRO_CHECK_TYPE(token_type_right_bracket);

        // get next token and exexute <statement> rule
        MACRO_GET_TOKEN();
        return statement(data);
    }
    else if(data->token.type == token_type_dedent)
    {
        return token_scan_accepted;
    }
    /// <statement> -> <main_body>
    else if(data->token.type == token_type_EOF)
    {
        return token_scan_accepted;
    }
    else if(data->token.type == token_type_left_bracket)
    {

        MACRO_CHECK_RULE(expression);
        return token_scan_accepted;

    }
    return error_syntax;
}

/** Processing <def_value> rule
 *
 * @param data - Parser data
 * @return - if processed value is OK return 0, otherwise error
 */
int def_value(Parser_data* data)
{
    int result;

    // <def_value> None
    if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_None)
    {
        MACRO_CHECK_RULE(expression);
        return token_scan_accepted;

    }

    if(data->token.type == token_type_identifier || data->token.type == token_type_keyword)
    {

        //ak je na zaciatku vyrazu premenna napr b = a + 3
        if(data->in_function == true)
        {
            IData* var1 = sym_table_search(&data->local_table, data->token.attribute.s->string);
            data->right_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
            if(var1 == NULL && data->right_side_id != NULL)
            {

                MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
                MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list);
                MACRO_CHECK_TYPE(token_type_right_bracket);
                MACRO_GET_TOKEN();
                if(data->token.type == token_type_EOF)
                {
                    return token_scan_accepted;
                }
                MACRO_CHECK_TYPE(token_type_EOL);


                MACRO_GET_TOKEN();
                return statement(data);
            }
            else if (var1 != NULL && var1->is_variable == true)
            {
                MACRO_CHECK_RULE(expression);
                return token_scan_accepted;
            }
            else if(var1 == NULL && data->right_side_id == NULL)
            {
                MACRO_CHECK_RULE(expression);
                return token_scan_accepted;
            }


        } else
        {
            IData* var = sym_table_search(&data->global_table, data->token.attribute.s->string);
            if(var == NULL)
            {
                return error_semantic;
            }
            else if(var != NULL && var->is_variable == true)
            {
                MACRO_CHECK_RULE(expression);
                return token_scan_accepted;
            }
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

            MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
            MACRO_GET_TOKEN_AND_CHECK_RULE(arg_list);
            MACRO_CHECK_TYPE(token_type_right_bracket);

            Gen_function_call(data->right_side_id->identifier);

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

/** Processing <arg_list> rule
 *
 * @param data - Parser data
 * @return - if processed argument is OK return 0, otherwise error
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

    //osetrenie ak ma definovana funkcia parameter a nezadame ziadny
    if(data->param_index != data->right_side_id->params->length_of_lexem_string)
    {
        return error_semantic_bad_count_param;
    }


    // <arg_list> -> ε
    return token_scan_accepted;
}

/** Processing <arg_list2> rule
 *
 * @param data - Parser data
 * @return - if processed arguments are OK return 0, otherwise error
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

/** Processing <value> rule
 *
 * @param data - Parser data
 * @return - if processed value is OK return 0, otherwise error
 */
int value(Parser_data* data)
{
    int number_to_term_function;
    //check number of arguments
    if(data->right_side_id->params->length_of_lexem_string == data->param_index) // ma tu byt == ale este nemam spravene pridavanie parametrov
    {
        return error_semantic_bad_count_param;
    }

    switch(data->token.type)
    {
        // <value> -> FLOAT
        case token_type_float:

            data->right_side_id->params->string[data->param_index] = 'f';
            Gen_push_arg(Term_adjustment(data->token.attribute.s->string, 1));
            break;

        // <value> -> INT
        case token_type_int:

            data->right_side_id->params->string[data->param_index] = 'i';
            Gen_push_arg(Term_adjustment(data->token.attribute.s->string, 0));
            break;

        // <value> -> STRING
        case token_type_str:

            data->right_side_id->params->string[data->param_index] = 's';
            Gen_push_arg(Term_adjustment(data->token.attribute.s->string, 5));
            break;

        // <value> -> IDENTIFIER
        case token_type_identifier:;//  C evil magic

            if(data->in_function == true)
            {
                IData* id = sym_table_search(&data->local_table, data->token.attribute.s->string);
                if(!id) return error_semantic;
                number_to_term_function = 4;

                switch(id->type)
                {
                    case DATA_TYPE_INTEGER:

                        data->right_side_id->params->string[data->param_index] = 'i';
                        Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));
                        break;

                    case DATA_TYPE_FLOAT:

                        data->right_side_id->params->string[data->param_index] = 'f';
                        Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));
                        break;

                    case DATA_TYPE_STRING:
                        /*
                        if(data->right_side_id->params->string[data->param_index] != 's')
                            return error_semantic_compatibility;
                        */
                        data->right_side_id->params->string[data->param_index] = 's';
                        Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));
                        break;

                    default:
                        return error_internal;
                }


            } else{
                IData* id = sym_table_search(&data->global_table, data->token.attribute.s->string);
                if(!id) return error_semantic;
                number_to_term_function = 3;

                switch(id->type)
                {
                    case DATA_TYPE_INTEGER:
                        /*
                        if(data->right_side_id->params->string[data->param_index] == 's')
                            return error_semantic_compatibility;
                        if(data->right_side_id->params->string[data->param_index] == 'f')
                            printf("Treba pretypovat");
                        printf("good");
                        */
                        data->right_side_id->params->string[data->param_index] = 'i';
                        Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));
                        break;

                    case DATA_TYPE_FLOAT:
                        /*
                        if(data->right_side_id->params->string[data->param_index] == 's')
                            return error_semantic_compatibility;
                        if(data->right_side_id->params->string[data->param_index] == 'i')
                            return error_semantic_compatibility;
                        printf("good");
                        */
                        data->right_side_id->params->string[data->param_index] = 'f';
                        Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));
                        break;

                    case DATA_TYPE_STRING:
                        /*
                        if(data->right_side_id->params->string[data->param_index] != 's')
                            return error_semantic_compatibility;
                        */
                        data->right_side_id->params->string[data->param_index] = 's';
                        Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));
                        break;

                    default:
                        return error_internal;
                }
            }

            /*
            if(id->global == false)
            {
                number_to_term_function = 4;
            } else
            {
                number_to_term_function = 3;
            }
             */

            break;
        default:
            return error_syntax;
    }

    // increment argument position
    data->param_index++;

    return token_scan_accepted;
}

/** Processing <print_rule> rule
 *
 * @param data - Parser data
 * @return - if processed print is OK return 0, otherwise error
 */
int print_rule(Parser_data* data)
{
    int result;
    int number_to_term_function;

    //<print_rule> , <print_rule>
    if(data->token.type == token_type_comma)
    {
        MACRO_CHECK_TYPE(token_type_comma);
        MACRO_GET_TOKEN();
        if(data->token.type == token_type_int || data->token.type == token_type_float || data->token.type == token_type_str || data->token.type == token_type_identifier)
        {

            switch(data->token.type)
            {
                case token_type_str:
                    number_to_term_function = 5;
                    break;
                case token_type_int:
                    number_to_term_function = 0;
                    break;
                case token_type_float:
                    number_to_term_function = 1;
                    break;
            }

            //test na identifikator v printe ci je definovany ak nie tak chyba
            if(data->token.type == token_type_identifier)
            {
                if(data->in_function == true)
                {
                    IData* variable_in_global_table;
                    variable_in_global_table = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    data->left_side_id = sym_table_search(&data->local_table, data->token.attribute.s->string);
                    if( (data->left_side_id == NULL) && (variable_in_global_table == NULL) )
                    {
                        return error_semantic;
                    }
                    number_to_term_function = 4;
                }
                else{
                    data->left_side_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
                    if(data->left_side_id == NULL)
                    {
                        return error_semantic;
                    }
                    number_to_term_function = 3;
                }
            }

            Gen_push_arg(Term_adjustment(data->token.attribute.s->string, number_to_term_function));

            //osetrenie volania funkcie v definicii inej funkcie

            Gen_function_call("print");


            MACRO_GET_TOKEN_AND_CHECK_RULE(print_rule);
        }


    }
    else if(data->token.type == token_type_right_bracket)
    {
        MACRO_CHECK_TYPE(token_type_right_bracket);

        //prazdny print() "\n" -> generovanie medzeri
        Gen_push_arg(Term_adjustment("\n", 5));

        Gen_function_call("print");


        MACRO_GET_TOKEN();
        if(data->token.type == token_type_EOF)
        {
            return token_scan_accepted;
        }
        MACRO_CHECK_TYPE(token_type_EOL);

        return token_scan_accepted;
    }

    return token_scan_accepted;
}

/** Initialization of variables
 *
 * @param data - Parser data
 * @return - if initialization ran return 0, otherwise error
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
    IData* id;

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

/** Free all items in local and global table
 *
 */
void free_variables(Parser_data* data)
{
    sym_table_free(&data->global_table);
    sym_table_free(&data->local_table);
}

/** Main body of parser
 *
 * @return - if syntactic and semantic analysis is OK return 0, otherwise error
 */
int analyza()
{
    // declaration of variables
    int result;
    Parser_data parser_data;
    Lexem_string string;
    stack = (tStack*) malloc(sizeof(tStack));
    stack_param = (tStack_Param*) malloc(sizeof(tStack_Param));
    stackInit_param(stack_param);
    stackInit(stack);
    set_stack(stack);

    // initialization of boths strings
    if((!lexem_string_init(&string)))
    {
        return error_internal;
    }

    if((!lexem_string_init(&IFJcode19)))
    {
        return error_internal;
    }

    set_string(&string); // string in scanner

    // initialization of variables
    if (!init_variables(&parser_data))
    {
        lexem_string_clear(&string);
        free(stack);
        return error_internal;
    }

    // Get first token and start syntactic analysis
    if ((result = get_token(&parser_data.token, stack)) == token_scan_accepted)
    {
        //GENEROVANIE
        Gen_Start();
        result = prog_body(&parser_data);

    }

    if(result == 0)
    {
        // Assembly code is printed on stdout
        printf("%s", IFJcode19.string);
    }


    //free all variables
    lexem_string_clear(&string);
    lexem_string_clear(&IFJcode19);
    free_variables(&parser_data);
    free(stack);
    free(stack_param);
    return result;
}






















