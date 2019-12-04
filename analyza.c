
/************************* ANALYZA *************************
 * @author : Patrik Tomov <xtomov0200@stud.fit.vutbr.cz>
***********************************************************/


#include "analyza.h"
#include "lexem_string.h"
#include "symtable.h"
#include "scanner.h"
#include "expression.h"
#include "code_generator.h"

tStack *stack; // zasobnik na indenty/dedenty



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
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_indent);
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
    int counter = 0;
    char* array_of_parameters[100];

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
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_indent);


        MACRO_GET_TOKEN_AND_CHECK_RULE(statement);
        MACRO_CHECK_TYPE(token_type_dedent);
        MACRO_GET_TOKEN_AND_CHECK_KEYWORD(keyword_else);

        Gen_else_head();

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_indent);

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

        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);
        MACRO_GET_TOKEN_AND_CHECK_TYPE(token_type_indent);

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

        int number_to_term_function;

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
                    if(internal_error)
                        return error_internal;
                    else
                        return error_semantic;
            } else{
                //viacnasobne pouzitie premennev vo funkcii
                data->left_side_id = var;
            }
        }
        //Ak niesme tak pridame premennu do globalnej tabulky
        else{

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



        //GENEROVANIE PRI PRVOM PRIRADENI
        if(data->left_side_id->global == true)
        {
            Gen_var_def(Term_adjustment(data->left_side_id->identifier, number_to_term_function));
        }
        else{
            Gen_var_def(Term_adjustment(data->left_side_id->identifier, number_to_term_function));
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
            /*
            if(data->right_side_id->params->string[data->param_index] == 's')
                return error_semantic_compatibility;
            if(data->right_side_id->params->string[data->param_index] == 'i')
                return error_semantic_compatibility;
            printf("good");
            */
            data->right_side_id->params->string[data->param_index] = 'f';
            break;

        // <value> -> INT
        case token_type_int:
            /*
            if(data->right_side_id->params->string[data->param_index] == 's')
                return error_semantic_compatibility;
            if(data->right_side_id->params->string[data->param_index] == 'f')
                printf("Treba pretypovat");
            printf("good");
            */

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
                    /*
                    if(data->right_side_id->params->string[data->param_index] == 's')
                        return error_semantic_compatibility;
                    if(data->right_side_id->params->string[data->param_index] == 'f')
                        printf("Treba pretypovat");
                    printf("good");
                    */
                    data->right_side_id->params->string[data->param_index] = 'i';

                case DATA_TYPE_FLOAT:
                    /*
                    if(data->right_side_id->params->string[data->param_index] == 's')
                        return error_semantic_compatibility;
                    if(data->right_side_id->params->string[data->param_index] == 'i')
                        return error_semantic_compatibility;
                    printf("good");
                    */
                    data->right_side_id->params->string[data->param_index] = 'f';
                    break;

                case DATA_TYPE_STRING:
                    /*
                    if(data->right_side_id->params->string[data->param_index] != 's')
                        return error_semantic_compatibility;
                    */
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






















