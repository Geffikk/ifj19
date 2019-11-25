//
// Created by patres on 25. 10. 2019.
//


#include "analyza.h"
#include "lexem_string.h"
#include "symtable.h"
#include "scanner.h"



#define IS_VALUE(token)												\
	(token).type == token_type_float						\
	|| (token).type == token_type_int						\
	|| (token).type == token_type_str							\
	|| (token).type == token_type_identifier

#define GET_TOKEN()													\
	if ((result = get_token(&data->token)) != token_scan_accepted)\
		return result

#define CHECK_TYPE(_type)											\
	if (data->token.type != (_type)) return error_syntax

#define CHECK_RULE(rule)											\
	if ((result = rule(data))) return result

#define CHECK_KEYWORD(_keyword)										\
	if (															\
		data->token.type != token_type_keyword						\
		|| data->token.attribute.keyword != (_keyword)				\
	) return error_syntax

#define GET_TOKEN_AND_CHECK_TYPE(_type)								\
	do {															\
		GET_TOKEN();												\
		CHECK_TYPE(_type);											\
	} while(0)

#define GET_TOKEN_AND_CHECK_RULE(rule)								\
	do {															\
		GET_TOKEN();												\
		CHECK_RULE(rule);											\
	} while(0)

#define GET_TOKEN_AND_CHECK_KEYWORD(_keyword)						\
	do {															\
		GET_TOKEN();												\
		CHECK_KEYWORD(_keyword);									\
	} while(0)



static int prog(PData* data); //definicie premennych
static int main_body(PData* data); //hlavne telo programu
static int end_main(PData* data); // koniec hlavneho tela
static int params(PData* data);  //spravovanie parametru ->
static int param_n(PData* data); // ak ich je viac
static int statement(PData* data); //if, while, etc..
static int def_value(PData* data); // id = <def_value>
//static int def_var(PData* data);
static int arg(PData* data); //argument pri volani funkcii ->
static int arg_n(PData* data); //ak ich je viac
static int value(PData* data);
static int print(PData* data);


/**
 * Implementation of <prog> rule.
 *
 * @return Given exit code.
 */
static int prog(PData* data)
{
     int result;

    //<prog> ->   def id ( <params> ) : EOL <statement>  <prog>
    if (data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_def)
    {
        data->in_function = true;
        //data->in_declaration = true;
        data->label_index = 0;

        GET_TOKEN_AND_CHECK_TYPE(token_type_identifier);

        //Add id to the global symbol table
        bool internal_error;
        data->current_id = sym_table_add_symbol(&data->global_table, data->token.attribute.s->string, &internal_error);
        if(!data->current_id)
        {
            if (internal_error)
            {
                return ERROR_INTERNAL;
            } else{
                return SEM_ERR_UNDEFINED_VAR;
            }

        }

        GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
        GET_TOKEN_AND_CHECK_RULE(params);
        CHECK_TYPE(token_type_right_bracket);
        GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);
        GET_TOKEN_AND_CHECK_RULE(statement);


        // Current function is defined
        data->current_id->defined = true;

        // clear local symtable
        sym_table_free(&data->local_table);

        // get next token and execute <prog> rule
        GET_TOKEN();
        return prog(data);
    }
    // <prog> ->  EOL  <prog>
    else if(data->token.type == token_type_EOL)
    {
        GET_TOKEN();
        return prog(data);
    }
    // <prog> -> <main_body>
    else
    {
        return main_body(data);
    }

    return SYNTAX_ERR;
}


/**
 * Implementation of <main_body> rule.
 *
 * @return Given exit code.
 */
static int main_body(PData* data)
{
    int result;

    //check if all functios are define
    for(int i = 0; i < MAX_SYMTABLE_SIZE; i++)
        for(Sym_table_item* it = data->global_table[i]; it !=NULL; it = it->next)
            if(!it->data.defined) return SEM_ERR_UNDEFINED_VAR;

    //we are in main now
    data->in_function = false;
    data->current_id = NULL;
    data->label_index = 0;

    CHECK_RULE(statement);

    //clear local symbol table
    sym_table_free(&data->local_table);


    // get next token and execute <prog> rule
    GET_TOKEN();
    return end_main(data);

}

/**
 * Implementation of <end> rule.
 *
 * @return Given exit code.
 */
static int end_main(PData* data)
{
    int result;

    // <prog> -> EOL <end>
    if (data->token.type == token_type_EOL)
    {
        GET_TOKEN();
        return end_main(data);
    }

    // <end> -> EOF
    if (data->token.type == token_type_EOF)
    {
        return SYNTAX_OK;
    }

    return SYNTAX_ERR;
}


/**
 * Implementation of <params> rule.
 *
 * @return Given exit code.
 */
static int params(PData* data)
{
    int result;

    data->param_index = 0;

    // <params> ->   id  <param_n>
    if (data->token.type == token_type_identifier)
    {
        //if there is function named as parameter
        if(sym_table_search(&data->global_table, data->token.attribute.s->string))
        {
            return SEM_ERR_UNDEFINED_VAR;
        }

        //add params to the local symbol table
        bool internal_error;
        if(!(data->rhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error)))
        {
            if(internal_error)
            {
                return ERROR_INTERNAL;
            } else{
                return SEM_ERR_UNDEFINED_VAR;
            }
        }

        GET_TOKEN_AND_CHECK_RULE(param_n);

    }

    // <params> ->  ε

    return SYNTAX_OK;
}


/**
 * Implementation of <param_n> rule.
 *
 * @return Given exit code.
 */
static int param_n(PData* data)
{
    int result;

    // <param_n> -> , id <param_n>
    if(data->token.type == token_type_comma)
    {
        data->param_index++;

        GET_TOKEN_AND_CHECK_TYPE(token_type_identifier);

        bool internal_error;
        if(!(data->rhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error)))
        {
            if (internal_error){
                return ERROR_INTERNAL;
            } else{
                return SEM_ERR_UNDEFINED_VAR;
            }
        }

        GET_TOKEN();
        return param_n(data);
    }

    // <param_n> -> ε

    return SYNTAX_OK;

}

/**
 * Implementation of <statement> rule.
 *
 * @return Given exit code.
*/
static int statement(PData* data)
{
    int result;

    // <statement> ->    if <expression> : EOL <statement> else : EOL <statement> EOL    <statement>
    if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_if)
    {
        data->label_deep++;
        data->in_while_or_if = true;

        data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
        if(!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
        data->lhs_id->type = TYPE_BOOL;

        char *fuction_id = data->current_id ? data->current_id->identifier : "";
        int current_label_index = data->label_index;
        data->label_index += 2;

        //GET_TOKEN_AND_CHECK_RULE(expression);

        GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);


        GET_TOKEN_AND_CHECK_RULE(statement);
        CHECK_KEYWORD(keyword_else);
        GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);

        GET_TOKEN_AND_CHECK_RULE(statement);
        CHECK_TYPE(token_type_EOL);

        data->label_deep--;
        data->in_while_or_if = false;

        // get next token and execute <statement> rule
        GET_TOKEN();
        return statement(data);
    }


    // <statement> ->   while <expression> : EOL <statement> EOL  <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_while)
    {
        data->label_deep++;
        data->in_while_or_if = true;

        data->lhs_id = sym_table_search(&data->global_table, "%exp_result");
        if(!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
        data->lhs_id->type = TYPE_BOOL;

        char *function_id = data->current_id ? data->current_id->identifier : "";
        int current_label_index = data->label_index;
        data->label_index += 2;

        //GET_TOKEN_AND_CHECK_RULE(expression);

        GET_TOKEN_AND_CHECK_TYPE(token_type_colon);
        GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);

        GET_TOKEN_AND_CHECK_RULE(statement);

        CHECK_TYPE(token_type_EOL);

        data->label_deep--;
        data->in_while_or_if = false;

        // get next token and execute <statement> rule
        GET_TOKEN();
        return statement(data);
    }


    // <statement>  id = <def_value> EOL  <statement>
    else if(data->token.type == token_type_identifier)
    {
        //ak ma premenna rovnake meno ako skor definovana funkcia
        if(sym_table_search(&data->global_table, data->token.attribute.s->string))
        {
            return SEM_ERR_UNDEFINED_VAR;
        }

        //Ak sme vo funkcii tak pridame premennu do lokalnej tabulky
        bool internal_error;
        if(data->in_function)
        {
            //ak este nebola definovana premenna ta ju pridame do lokalnej tabulky inak uz bola pouzita cize preskocime
            TData* id = sym_table_search(&data->local_table, data->token.attribute.s->string);
            if(id == NULL)
            {
                data->lhs_id = sym_table_add_symbol(&data->local_table, data->token.attribute.s->string, &internal_error);
                if(!data->lhs_id || sym_table_search(&data->global_table, data->token.attribute.s->string))
                    if(internal_error)
                        return ERROR_INTERNAL;
                    else
                        return SEM_ERR_UNDEFINED_VAR;
            }

        }
        //Ak niesme tak pridame premennu do globalnej tabulky
        else{
            TData* id = sym_table_search(&data->global_table, data->token.attribute.s->string);
            if(id != NULL)
            {
                return SEM_ERR_UNDEFINED_VAR;
            }
            data->lhs_id = sym_table_add_symbol(&data->global_table, data->token.attribute.s->string, &internal_error);
            if(!data->lhs_id)
                if(internal_error)
                    return ERROR_INTERNAL;
                else
                    return SEM_ERR_UNDEFINED_VAR;
        }

        GET_TOKEN_AND_CHECK_TYPE(token_type_assign);
        GET_TOKEN_AND_CHECK_RULE(def_value);
        CHECK_TYPE(token_type_EOL);

        // get next token and execute <statement> rule
        GET_TOKEN();
        return statement(data);
    }

    // <statement> ->   print ( <expression> , <print> ) EOL   <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_print)
    {
        int result;

        GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
        //GET_TOKEN_AND_CHECK_RULE(expression);
        GET_TOKEN();
        if(data->token.type == token_type_right_bracket)
        {
            CHECK_TYPE(token_type_right_bracket);
            GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);
        }
        else if(data->token.type == token_type_comma)
        {
            CHECK_TYPE(token_type_comma);
            GET_TOKEN_AND_CHECK_RULE(print);

        }


        // get next token and execute <statement> rule
        GET_TOKEN();
        return statement(data);
    }


    // <statement> ->   return <expression> EOL   <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_return)
    {
        //return moze byt len v tele funkcii nie v hlavnom programe
        if(!data->in_function) return error_syntax;

        data->lhs_id = sym_table_search(&data->global_table, "$exp_result");
        if(!data->lhs_id) return SEM_ERR_UNDEFINED_VAR;
        data->lhs_id->type = data->current_id->type;

        //GET_TOKEN_AND_CHECK_RULE(expression);

        CHECK_TYPE(token_type_EOL);

        // get next token and execute <statement> rule
        GET_TOKEN();
        return statement(data);
    }


    // <statement>  pass EOL  <statement>
    else if(data->token.type == token_type_keyword && data->token.attribute.keyword == keyword_pass)
    {
        GET_TOKEN_AND_CHECK_TYPE(token_type_EOL);

        GET_TOKEN();
        return statement(data);
    }
        // <statement> -> ε
    else if (data->token.type == token_type_EOL)
    {
        GET_TOKEN();
        return statement(data);
    }

    return SYNTAX_OK;
}

/**
 * Implementation of <def_value> rule.
 *
 * @return Given exit code.
 */
static int def_value(PData* data)
{
    int result;
    if(data->token.type == token_type_identifier || data->token.type == token_type_keyword)
    {
        // <def_value> -> id ( <arg> )
        if(data->token.type == token_type_identifier)
        {
            data->rhs_id = sym_table_search(&data->global_table, data->token.attribute.s->string);
        }

        if(data->token.type == token_type_keyword)
        {
            switch(data->token.attribute.keyword)
            {
                    // <def_value> -> ord ( <arg> )
                case keyword_ord:
                    data->rhs_id = sym_table_search(&data->global_table, "ord");
                    break;

                    // <def_value> -> chr ( <arg> )
                case keyword_chr:
                    data->rhs_id = sym_table_search(&data->global_table, "chr");
                    break;

                    // <def_value> -> len ( <arg> )
                case keyword_len:
                    data->rhs_id = sym_table_search(&data->global_table, "len");
                    break;

                    // <def_value> -> substr ( <arg> )
                case keyword_substr:
                    data->rhs_id = sym_table_search(&data->global_table, "substr");
                    break;

                    // <def_value> -> inputs()
                case keyword_inputs:
                    data->rhs_id = sym_table_search(&data->global_table, "inputs");
                    break;

                    // <def_value> -> inputi()
                case keyword_inputi:
                    data->rhs_id = sym_table_search(&data->global_table, "inputi");
                    break;

                    // <def_value> -> inputf()
                case keyword_inputf:
                    data->rhs_id = sym_table_search(&data->global_table, "inputf");
                    break;

                default:
                    return SYNTAX_ERR;
            }
        }

        if(data->rhs_id)
        {
            // check type compatibilty
            // if either one expression is string, we cannot implicitly convert
            if (data->lhs_id->type != data->rhs_id->type)
            {
                if (data->lhs_id->type == TYPE_STRING || data->rhs_id->type == TYPE_STRING)
                {
                    return SEM_ERR_TYPE_COMPAT;
                }
            }

            //ak mame identifikator (funkciu) alebo vestavene funkcie len, ord, chr, substr tak kontrolujeme aj argumenty
            if(data->token.type == token_type_identifier || data->token.attribute.keyword == keyword_ord ||
            data->token.attribute.keyword == keyword_chr || data->token.attribute.keyword == keyword_len ||
            data->token.attribute.keyword == keyword_substr)
            {
                GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
                GET_TOKEN_AND_CHECK_RULE(arg);
                CHECK_TYPE(token_type_right_bracket);
            //ak mame vestavene funkcie inputs(), inputi(), inputf() tak kontrolujeme len zatvorky
            } else{
                GET_TOKEN_AND_CHECK_TYPE(token_type_left_bracket);
                GET_TOKEN_AND_CHECK_TYPE(token_type_right_bracket);
            }


            GET_TOKEN();


            if(data->rhs_id->params->length_of_lexem_string != data->param_index) // ma tu byt != ale zase PARAMETRE DPC!
                return SEM_ERR_WRONG_PARAM;


            return SYNTAX_OK;
        }

        data->rhs_id = sym_table_search(&data->local_table, data->token.attribute.s->string);
        if(!data->rhs_id)
        {
            return SEM_ERR_UNDEFINED_VAR;
        }
    }

    // <def_value> -> <expression>
    //CHECK_RULE(expression);
    GET_TOKEN();

    return SYNTAX_OK;
}

/**
 * Implementation of <def_var> rule.
 *
 * @return Given exit code.

static int def_var(PData* data)
{
    int result;

    // <def_var> -> = <expression>
    if (data->token.type == token_type_assign)
    {
        //GET_TOKEN_AND_CHECK_RULE(expression);
        GET_TOKEN();
        GET_TOKEN();
        GET_TOKEN();
    }

    // <def_var> -> ε

    return token_syntax_accepted;
}
 */
/**
 * Implementation of <arg> rule.
 *
 * @return Given exit code.
 */
static int arg(PData* data)
{
    int result;

    // currently processed argument
    data->param_index = 0;

    // <arg> -> <value> <arg_n>
    // if token is value
    if(IS_VALUE(data->token))
    {
        CHECK_RULE(value);
        GET_TOKEN_AND_CHECK_RULE(arg_n);
    }

    // <arg> -> ε

    return SYNTAX_OK;
}


/**
 * Implementation of <arg_n> rule.
 *
 * @return Given exit code.
 */
static int arg_n(PData* data)
{
    int result;

    // <arg_n> -> , <value> <arg-n>
    if(data->token.type == token_type_comma)
    {
        GET_TOKEN_AND_CHECK_RULE(value);
        GET_TOKEN_AND_CHECK_RULE(arg_n);
    }

    // <arg_n> ->  ε

    return SYNTAX_OK;
}

/**
 * Implementation of <value> rule.
 *
 * @return Given exit code.
 */
static int value(PData* data)
{
    //check number of arguments
    if(data->rhs_id->params->length_of_lexem_string != data->param_index) // ma tu byt == ale este nemam spravene pridavanie parametrov
    {
        return SEM_ERR_TYPE_COMPAT;
    }

    switch(data->token.type)
    {
        // <value> -> DOUBLE
        case token_type_float:
            if(data->rhs_id->params->string[data->param_index] == 's')
                return SEM_ERR_TYPE_COMPAT;
            if(data->rhs_id->params->string[data->param_index] == 'i')
                printf("good");
            break;

        // <value> -> INT
        case token_type_int:
            if(data->rhs_id->params->string[data->param_index] == 's')
                return SEM_ERR_TYPE_COMPAT;
            if(data->rhs_id->params->string[data->param_index] == 'd')
                printf("good");
            break;

        // <value> -> STRING
        case token_type_str:
            if(data->rhs_id->params->string[data->param_index] != 's')
                return SEM_ERR_TYPE_COMPAT;
            break;

        // <value> -> IDENTIFIER
        case token_type_identifier:; // ; C evil magic
            TData* id = sym_table_search(&data->local_table, data->token.attribute.s->string);
            if(!id) return SEM_ERR_UNDEFINED_VAR;

            switch(id->type)
            {
                case TYPE_INT:
                    if(data->rhs_id->params->string[data->param_index] == 's')
                        return SEM_ERR_TYPE_COMPAT;
                    if(data->rhs_id->params->string[data->param_index] == 'd')
                        printf("good");
                    break;

                case TYPE_DOUBLE:
                    if(data->rhs_id->params->string[data->param_index] == 's')
                        return SEM_ERR_TYPE_COMPAT;
                    if(data->rhs_id->params->string[data->param_index] == 'i')
                        printf("good");
                    break;

                case TYPE_STRING:
                    if(data->rhs_id->params->string[data->param_index] != 's')
                        return SEM_ERR_TYPE_COMPAT;
                    break;

                default:
                    break; // ma tu byt return ERROR_INTERNAL; pretoze by sa tu nikdy nemal dostat ale nemam este poriesene datove typy
            }
            break;
        default:
            return SCANNER_ERROR_LEX;
    }

    // increment argument position
    data->param_index++;

    return SYNTAX_OK;
}

/**
 * Implementation of <print> rule.
 *
 * @return Given exit code.
 */
static int print(PData* data)
{
    int result;

    //CHECK_RULE(expression);
    GET_TOKEN();
    if(data->token.type == token_type_comma)
    {
        CHECK_TYPE(token_type_comma);
        GET_TOKEN_AND_CHECK_RULE(print);
    }
    else if(data->token.type == token_type_right_bracket)
    {
        CHECK_TYPE(token_type_right_bracket);

        return SYNTAX_OK;
    }

}

/**
 * Initialize variable needed for analysis.
 *
 * @return True if inicialization was successful, false otherwise.
 */
static bool init_variables(PData* data)
{
    sym_table_init(&data->global_table);
    sym_table_init(&data->local_table);

    data->current_id = NULL;
    data->lhs_id = NULL;
    data->rhs_id = NULL;

    data->param_index = 0;
    data->label_index = 0;
    data->label_deep = -1;

    data->in_function = false;
    //data->in_declaration = false;
    data->in_while_or_if = false;
    //data->non_declared_function = false;


    // init default functions
    bool internal_error;
    TData* id;

    //len(s)
    id = sym_table_add_symbol(&data->global_table, "len", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_INT;
    if(!sym_table_add_param(id, TYPE_STRING)) return false;


    //substr(s, i, n)
    id = sym_table_add_symbol(&data->global_table, "substr", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_STRING;
    if(!sym_table_add_param(id, TYPE_STRING)) return false;
    if(!sym_table_add_param(id, TYPE_INT)) return false;
    if(!sym_table_add_param(id, TYPE_INT)) return false;


    //ord(s, i)
    id = sym_table_add_symbol(&data->global_table, "ord", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_INT;
    if (!sym_table_add_param(id, TYPE_STRING)) return false;
    if (!sym_table_add_param(id, TYPE_INT)) return false;


    //chr(i)
    id = sym_table_add_symbol(&data->global_table, "chr", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_STRING;
    if (!sym_table_add_param(id, TYPE_INT)) return false;

    //inputs()
    id = sym_table_add_symbol(&data->global_table, "inputs", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_STRING;


    //inputi()
    id = sym_table_add_symbol(&data->global_table, "inputi", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_INT;


    //inputf()
    id = sym_table_add_symbol(&data->global_table, "inputf", &internal_error);
    if(internal_error) return false;

    id->defined = true;
    id->type = TYPE_DOUBLE;


    // Global variable %exp_result for storing result of expression.
    id = sym_table_add_symbol(&data->global_table, "%exp_result", &internal_error);
    if (internal_error) return false;
    id->defined = true;
    id->type = TYPE_UNDEFINED;
    id->global = true;

    return true;
}



/**
 * Frees symbol tables
 */
static void free_variables(PData* data)
{
    sym_table_free(&data->global_table);
    sym_table_free(&data->local_table);
}



int analyza()
{
    int result;

    Lexem_string string;

    if (!lexem_string_init(&string))
    {
        return -1;
    }
    set_string(&string);

    PData parser_data;

    if (!init_variables(&parser_data))
    {
        lexem_string_clear(&string);
        return -1;
    }

    if ((result = get_token(&parser_data.token)) == token_scan_accepted)
    {
        result = prog(&parser_data);
    }

    lexem_string_clear(&string);
    free_variables(&parser_data);

    return result;
}






















