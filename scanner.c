//
// Created by maros on 10/19/2019.
//

#include "scanner.h"
#include "stack_indent_dedent.c"
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define YES 999 // Something is good
#define NO 998 // Something is bad
#define ACCEPTED 997 // Something is accepted
#define DECLINE 996 // Something is decline
#define IDENTIFIER 995 // Property IDENTIFIER
#define KEYWORD 994 // Peroperty KEYWORD

#define state_start 100 // Starting state where starting every new token and initializes other states
#define state_identifier_keyword 101 // Start with letter
#define state_comment 103 // When symbol is ' , then this is start of string
#define state_comment_end 104 // When is second one this symbol ', that's mean is end of string
#define state_backslash 105
#define state_ident 106 // Indent lines based on priority
#define state_dedent 107 // Dedent lines based on priority
#define state_comment_EOL 108 // State where end of comment is EOL

// Operations with numbers
#define state_int 109
#define state_float 110
#define state_point 111
#define state_exponent 112
#define state_exponent_assigned 113
#define state_exponent_finish 114
#define state_less_then 115
#define state_more_then 116
#define state_exclamation_mark 117
#define state_equal 123
// Operations with string
#define state_string 118
#define state_string_activation 119
// Escape secvention
#define state_escape 120
#define state_escape_00FF 121
#define state_escape_00FF_00FF 122
#define state_escape_hexadecimal_first 124
#define state_escape_hexadecimal_second 125
#define state_indent_dedent 126

int EOL_actived = 0;
int init = 0;
FILE *source_file; // GLOBAL - Source File
Lexem_string *lexem_string; // GLOBAL - Initialize lexem string
Stack_indent_dedent *stack_main;
char top_char;

/** Classic return exit function
 *
 *  @param error - is exit notification if was scan SUCCESS or DENIED
 *  @param string - lexem string
 *  **/
static int free_resources(int error, Lexem_string *string)
{
    lexem_string_clear(string); // dealloce memory from lexem string
    return error; // error code
}

/** Aply process if lexem is IDENTIFIER or KEYWORD
 *
 * @param string - research lexem if is identifier or keyword
 * @param token - saving attributes and types to token
 * @return - scan was accepted or denied
 */
static int process_identifier_keyword(Lexem_string *str, Token *token)
{
    // if lexem is qual some keyword then assign attritube (keyword)
    if (cmp_lexem_string_with_another_string(str, "return")) token->attribute.keyword = keyword_return;
    else if (cmp_lexem_string_with_another_string(str, "pass")) token->attribute.keyword = keyword_pass;
    else if (cmp_lexem_string_with_another_string(str,"else")) token->attribute.keyword = keyword_else;
    else if (cmp_lexem_string_with_another_string(str,"if")) token->attribute.keyword = keyword_if;
    else if (cmp_lexem_string_with_another_string(str,"def")) token->attribute.keyword = keyword_def;
    else if (cmp_lexem_string_with_another_string(str,"float")) token->attribute.keyword = keyword_float;
    else if (cmp_lexem_string_with_another_string(str,"int")) token->attribute.keyword = keyword_int;
    else if (cmp_lexem_string_with_another_string(str,"str")) token->attribute.keyword = keyword_str;
    else if (cmp_lexem_string_with_another_string(str,"while")) token->attribute.keyword = keyword_while;
    else if (cmp_lexem_string_with_another_string(str,"inputs")) token->attribute.keyword = keyword_inputs;
    else if (cmp_lexem_string_with_another_string(str,"inputi")) token->attribute.keyword = keyword_inputi;
    else if (cmp_lexem_string_with_another_string(str,"inputf")) token->attribute.keyword = keyword_inputf;
    else if (cmp_lexem_string_with_another_string(str,"print")) token->attribute.keyword = keyword_print;
    else if (cmp_lexem_string_with_another_string(str,"len")) token->attribute.keyword = keyword_len;
    else if (cmp_lexem_string_with_another_string(str,"substr")) token->attribute.keyword = keyword_substr;
    else if (cmp_lexem_string_with_another_string(str, "chr")) token->attribute.keyword = keyword_chr;
    else if (cmp_lexem_string_with_another_string(str, "ord")) token->attribute.keyword = keyword_ord;
    else token->type = token_type_identifier;

    if(token->type != token_type_identifier)
    {
           token->type = token_type_keyword;
           return free_resources(token_scan_accepted, str);
    }

    // Assign lexem to token attribute
    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s))
    {
        return free_resources(error_internal, str);
    }

    return free_resources(token_scan_accepted, str);
}

/** Aply process if lexem is integer
 *
 * @param string - research lexem if is identifier
 * @param token - saving attributes and types to token
 * @return - scan was accepted or denied
 */
static int process_int(Lexem_string *string, Token *token)
{
    char *endptr;
    int val = (int) strtol(string->string, &endptr, 5);
    if (*endptr)
    {
        return free_resources(error_internal, string);
    }

    token->attribute.int_number = val;
    token->type = token_type_identifier;

    return free_resources(token_scan_accepted, string);
}

/** Aply process, if lexem is float number
 *
 * @param string - research lexem if is float number
 * @param token - saving attributes and types to token
 * @return - scan was accepted or denied
 */
static int process_float(Lexem_string *string, Token *token){

    char *endptr;
    int value = (int) strtol(string->string, &endptr, 5);
    if (*endptr)
    {
        return free_resources(error_internal, string);
    }

    token->attribute.float_number = value;
    token->type = token_type_float;
    return free_resources(token_scan_accepted, string);

}

/** Initialize source file
 *
 * @param f - source file
 */
void set_source_file(FILE *f)
{
    source_file = f;
}

/** Set lexem string
 *
 * @param string - set lexem string
 */
void set_string(Lexem_string *string)
{
    lexem_string = string;
}

void set_stack(Stack_indent_dedent *stack)
{
    stack_main = stack;
}


/** Function give token, make lexems, assign attributes and types
 *
 * @param token - saving informations about this token to structure
 * @return - scan was accepted or denied
 */
int get_token(Token *token)
{
    if (source_file == NULL)
    {
        return error_internal;
    }

    if (lexem_string == NULL)
    {
        return error_internal;
    }

    token->attribute.s = lexem_string;

    Stack_indent_dedent stack;
    Stack_indent_dedent *stk = &stack;
    if(init == 0)
    {
        stackInit(stk);
        init = 1;
    }

    // Inicialization LEXEM_STRING
    Lexem_string string;
    Lexem_string *str = &string;
    if (!lexem_string_init(str)) {
        return error_internal;
    }

    char c;
    char *endptr;
    char strnum[3] = {0};
    char top_char;
    int cnt_ws = 0;

    int state = state_start;
    token->type = token_type_empty;

    while (true) {

        c = (char) getc(source_file);

        switch (state) {
            case (state_start):
                if (isalpha(c)) {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                    state = state_identifier_keyword;
                }
                else if (isdigit(c) || c == '_')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        if (!add_char_to_lexem_string(lexem_string, c))
                        {
                            free_resources(error_internal, str);
                        }
                    }
                    state = state_int;
                }
                else if (c == '\r')
                {
                    state = state_comment_EOL;
                }
                else if ((isspace(c)))
                {
                    if (EOL_actived == 1) {
                        goto indent_dedent;
                    }
                    else
                    {
                        state = state_start;
                    }
                }
                else if (c == '#')
                {
                    state = state_comment;
                }
                else if (c == '\'')
                {
                    state = state_string;
                }
                else if (c == '<')
                {
                    state = state_less_then;
                }
                else if (c == '>')
                {
                    state = state_more_then;
                }
                else if (c == '!')
                {
                    state = state_exclamation_mark;
                }
                else if (c == '=')
                {
                    state = state_equal;
                }
                else if (c == '+')
                {
                    token->type = token_type_plus;
                    return free_resources(token_scan_accepted, str);
                }
                else if (c == '-')
                {
                    token->type = token_type_minus;
                    return free_resources(token_scan_accepted, str);
                }
                else if (c == '*')
                {
                    token->type = token_type_mul;
                    return free_resources(token_scan_accepted, str);
                }
                else if (c == '/')
                {
                    token->type = token_type_div;
                    return free_resources(token_scan_accepted, str);
                }
                else if (c == '(')
                {
                    token->type = token_type_left_bracket;
                    return free_resources(token_scan_accepted, str);
                }
                else if (c == ')')
                {
                    token->type = token_type_right_bracket;
                    return free_resources(token_scan_accepted, str);
                }
                else if (c == ':')
                {
                    token->type = token_type_colon;
                }
                else if (c == ',')
                {
                    token->type = token_type_comma;
                }
                else if (c == EOF)
                {
                    token->type = token_type_EOF;
                    return free_resources(token_scan_accepted, str);
                }
                else
                {
                    return free_resources(error_lexical, str);
                }
                break;

            case (state_identifier_keyword):
                if (isalnum(c) || c == '_')
                {
                    if (!add_char_to_lexem_string(str, (char) tolower(c)))
                    {
                        return free_resources(error_lexical, str);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    return process_identifier_keyword(str, token);
                }

                break;
            case (state_int):
                if (isdigit(c))
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_resources(error_internal, str);
                    }
                }
                else if (c == '.')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_resources(error_internal, str);
                    }
                    state = state_point;
                }
                else if (c == 'E' || c == 'e')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_resources(error_internal, str);
                    }
                    state = state_exponent;
                }
                else {
                    ungetc(c, source_file);
                    process_int(str, token);
                    state = state_start;
                }

                break;

            case(state_comment):

                if (c == '\n')
                {
                    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s))
                    {
                        return free_resources(error_internal, str);
                    }
                    token->type = token_type_str;
                    state = state_start;
                }
                else if (c == '\\')
                {
                    state = state_escape;
                }
                else if (c < 32)
                {
                    return free_resources(error_lexical, str);
                }
                else
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                }
                break;

            case (state_point):
                if (isdigit(c)) {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                    state = state_float;
                } else {
                    free_resources(error_lexical, str);
                }

                break;

            case (state_exponent):

                if (c == '+' || c == '-') {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                    state = state_exponent_assigned;
                } else if (isdigit(c)) {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                    state = state_exponent_finish;
                } else {
                    free_resources(error_lexical, str);
                }

                break;

            case (state_exponent_assigned):

                if (isdigit(c)) {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                    state = state_exponent_finish;
                } else {
                    ungetc(c, source_file);
                    free_resources(error_lexical, str);
                }

                break;

            case (state_float):

                if (isdigit(c)) {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                } else if (c == 'E' || c == 'e') {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                    state = state_exponent;
                } else {
                    ungetc(c, source_file);
                    return process_float(str, token);
                }

                break;

            case (state_exponent_finish):

                if (isdigit(c)) {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                } else {
                    ungetc(c, source_file);
                    return (process_float(str, token));
                }

                break;

            case (state_string):

                if (c < 32)
                {
                    return free_resources(error_lexical, str);
                }
                else if (c == '\'')
                {
                    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s))
                    {
                        return free_resources(error_internal, str);
                    }
                    token->type = token_type_str;
                }
                else if (c == '\\')
                {
                    state = state_escape;
                }
                else
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                }
                break;

            case (state_escape):

                if (c < 32)
                {
                    return free_resources(error_lexical, str);
                }
                else if (c == 'n')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                    return free_resources(error_internal, str);
                    }
                    c = '\n';
                    state = state_string;
                }
                else if (c == '\'')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                    c = '\'';
                    state = state_string;
                }
                else if (c == '"')
                {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_resources(error_internal, str);
                    }
                    c = '\"';
                    state = state_string;
                }
                else if (c == 't')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                    c = '\t';
                    state = state_string;
                }
                else if (c == '\\')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                    c = '\\';
                    state = state_string;
                }
                else if (c == 'x')
                {
                    if(!add_char_to_lexem_string(str, c))
                    {
                        free_resources(error_internal, str);
                    }
                    strnum[1] = c;
                    state = state_escape_hexadecimal_first;
                }

                    // Tuto state na ESCAPE SEKVENCE PRE HEXADECIMALNU SUSTAVU

                else
                {
                    return free_resources(error_lexical, str);
                }
                break;

            case(state_escape_hexadecimal_first):
                if ((c >= 0 && c <= 9) || (c >= 'A' && c <= 'F'))
                {
                    strnum[2] = c;
                    state = state_escape_hexadecimal_second;
                }
                else
                {
                    return free_resources(error_lexical, str);
                }
                break;

            case(state_escape_hexadecimal_second):

                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
                {
                    strnum[2] = c;
                    state = state_string;
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_resources(error_internal, str);



                    }
                }
                else
                {
                    return free_resources(error_lexical, str);
                }
                break;




            case (state_less_then):

                if (c == '=') {
                    token->type = token_type_LEQ;
                } else {
                    token->type = token_type_LT;
                    ungetc(c, source_file);
                }
                break;

            case (state_more_then):

                if (c == '=') {
                    token->type = token_type_MEQ;
                } else {
                    token->type = token_type_LT;
                    ungetc(c, source_file);
                }
                break;

            case (state_equal):

                if (c == '=')
                {
                    token->type = token_type_EQ;
                    return free_resources(token_scan_accepted, str);
                }
                else
                {
                    token->type = token_type_assign;
                    ungetc(c, source_file);
                    return free_resources(token_scan_accepted, str);
                }
                break;

            case (state_exclamation_mark):

                if (c == '=')
                {
                    token->type = token_type_NEQ;
                }
                else
                {
                    ungetc(c, source_file);
                    return free_resources(error_lexical, str);
                }

            case (state_indent_dedent):

            indent_dedent:
                if(isspace(c))
                {
                    cnt_ws = cnt_ws + 1;
                    state = state_indent_dedent;
                    break;
                }
                else
                {
                    stackTop(stk, &top_char);
                    if (cnt_ws > top_char)
                    {
                        stackPush(stk, cnt_ws);
                        token->type = token_type_indent;
                        cnt_ws = 0;
                        return free_resources(token_scan_accepted, str);
                    }
                    else if (cnt_ws < top_char)
                    {
                        stackTop(stk, &top_char);
                        while(cnt_ws != top_char)
                        {
                            stackTop(stk, &top_char);
                            if(cnt_ws < top_char)
                            {
                                stackPop(stk);
                                token->type = token_type_dedent;
                            }
                            else
                            {
                                cnt_ws = 0;;
                                free(stk);
                                return free_resources(error_lexical, str);
                            }
                        }
                        free(stk);
                    }
                    cnt_ws = 0;
                    ungetc(c, source_file);
                    return free_resources(token_scan_accepted, str);
                }
                break;

            case (state_comment_EOL):

                token->type = token_type_EOL;
                EOL_actived = 1;
                return free_resources(token_scan_accepted, str);
        }
    }
}


/*
            case(state_identifier || state_keyword):

                if(isalnum(c) || c == '_'){
                    if(!add_char_to_input_string(expression_string, c)){
                        free(expression_string);
                        return error_lexical;
                    }
                else
                    {
                    ungetc(source_file, c);
                    return;
                }

                }
        }
        }
}
*/





/*
static int treat_identifier(token *token, Input_string *s){

    int result = NO;

    if(cmp_input_string_with_another_string(s, "def"))
    {
        token->attribute.keyword = keyword_def;
        result = KEYWORD;
    }
    else if(cmp_input_string_with_another_string(s, "else"))
    {
        token->attribute.keyword = keyword_else;
        result = KEYWORD;
    }
    else if(cmp_input_string_with_another_string(s, "if"))
    {
        token->attribute.keyword = keyword_if;
        result = KEYWORD;
    }
    else if(cmp_input_string_with_another_string(s, "None"))
    {
        token->attribute.keyword = keyword_None;
        result = KEYWORD;
    }
    else if(cmp_input_string_with_another_string(s, "pass"))
    {
        token->attribute.keyword = keyword_pass;
        result = KEYWORD;
    }
    else if(cmp_input_string_with_another_string(s, "return"))
    {
        token->attribute.keyword = keyword_return;
        result = KEYWORD;
    }
    else
    {
        result = IDENTIFIER;
    }

    if(result == KEYWORD){
        token->type = token_type_keyword;
        free(s);
        return token_scan_accepted;
    }
    else if(result == IDENTIFIER){
        token->type = token_type_identifier;
        if((copy_input_string_to_attribute_string(s, token->attribute.s)) == false){
            free(s);
            return error_internal;
        }
    }

    free(s);
    return token_scan_accepted;
}

static int integer(token *token, Input_string *s){
    int var = atoi(s);

    token->attribute.int_number = var;
    token->type = token_type_int;
    return token_scan_accepted;
}

static int decimal(token *token, Input_string*s){
    int var = atoll(s);

    token->attribute.float_number = var;
    token->type = token_type_float;
    return token_scan_accepted;
}*/