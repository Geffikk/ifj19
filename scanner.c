
/************************ SCANNER *************************
 * @author : Maros Geffert <xgeffe00@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Indentation stack
***********************************************************/

#include "scanner.h"
#include "error.h"

#define state_start 100 // Starting state where starting every new token and initializes other states
#define state_identifier_keyword 101 // Start with letter
#define state_comment 103 // When symbol is (') , then this is start of string

// States - perations with numbers
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

// States - operations with strings
#define state_string 118
#define state_escape 120
#define state_escape_hexadecimal_first 124
#define state_escape_hexadecimal_second 125
#define state_EOL 127
#define state_documentation_string_first 128
#define state_documentation_string_second 129
#define state_documentation_string_finish_first 130
#define state_documentation_string_finish_second 131

FILE *source_file; // Global variable for source file because of using in scanner.c
Lexem_string *lexem_string; // Global variable for lexem string because of using in scanner.c
tStack *indentation_stack; // Global variable for stack because of using in scanner.c

int indentation_count = 0; // Counting indentation and compare if indentation is correctly

bool dedent_flag = false; // FLAG for generating dedent tokens
bool documentation_flag = false; // FLAG for report that is documentation string
bool indentation_flag = false;
bool first_line_indented = false;


/** Classic return exit function
 *
 *  @param error - is exit notification if was scan SUCCESS or DENIED
 *  @param string - lexem string
 *  **/
static int free_source(int error, Lexem_string *string)
{
    lexem_string_clear(string); // Dealloc memory from lexem string
    return error; // Error code
}

/** Aply process if lexem is IDENTIFIER or KEYWORD
 *
 * @param string - research lexem if is identifier or keyword
 * @param token - saving attributes and types to token
 * @return - scan was accepted or denied
 */
static int isIdentifier_Keyword(Lexem_string *str, Token *token)
{
    // If lexem is equal some keyword then assign attritube (keyword)
    if (cmp_lexem_string_with_another_string(str, "return")) token->attribute.keyword = keyword_return;
    else if (cmp_lexem_string_with_another_string(str ,"None")) token->attribute.keyword = keyword_None;
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

    if(token->type != token_type_identifier) // When nothing is assigned then word is identifier
    {
           token->type = token_type_keyword;
           return free_source(token_scan_accepted, str);
    }

    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s)) // Assign lexem string to token attribute
    {
        return free_source(error_internal, str);
    }

    return free_source(token_scan_accepted, str); // Return token scan was succes
}

/** Aply process if lexem is integer
 *
 * @param string - research lexem if is identifier
 * @param token - saving attributes and types to token
 * @return - scan was accepted or denied
 */
static int isInteger(Lexem_string *string, Token *token)
{

    // Convert value from lexem string to integer and assign value to attribute
    int val = (int) strtol(string->string, (char **)NULL, 10);
    token->attribute.int_number = val;
    token->type = token_type_int; // Set responsive token type

    return free_source(token_scan_accepted, string); // Return token scan was succes
}

/** Aply process, if lexem is float number
 *
 * @param string - research lexem if is float number
 * @param token - saving attributes and types to token
 * @return - scan was accepted or denied
 */
static int isFloat(Lexem_string *string, Token *token)
{
    // Convert value from lexem string to float number and assign value to attribute
    char *endptr;
    float value = strtof(string->string, &endptr);
    if (*endptr)
    {
        return free_source(error_internal, string);
    }
    token->attribute.float_number = value;
    token->type = token_type_float; // Set responsive token type
    return free_source(token_scan_accepted, string); // Return token scan was succes
}

void set_source_file(FILE *f)
{
    source_file = f; // Set source file for scanner
}

void set_string(Lexem_string *string)
{
    lexem_string = string; // Set lexem string for scanner
}

void set_stack(tStack *stack)
{
    indentation_stack = stack; // Set indentation stack for scanner
}

/** Function give token, make lexems, assign attributes and types
 *
 * @param token - saving informations about this token to structure
 * @return - scan was accepted or denied
 */
int get_token(Token *token, tStack *stack) {

    if (source_file == NULL) // If opening source file was failed than error
    {
        return error_internal; // Error code
    }

    if (lexem_string == NULL) // If initialization was failed than error
    {
        return error_internal; // Error code
    }

    if (indentation_stack == NULL) // If initialization was failed than error
    {
        return error_internal; // Error code
    }

    token->attribute.s = lexem_string; // Set attribute string empty

    Lexem_string string; // Initialization local lexem string
    Lexem_string *str = &string;

    if (!lexem_string_init(str)) // If initialization was failed than error
    {
        return error_internal; // Error code
    }

    char c; // One character from source file
    char hex_number[2] = {0}; // Array where im saving escape sequence (hexadecimal number)
    char stack_top_char = 0; // Assign top char from indentation stack
    char excess_zero = 0; // Assign first number from number and cmp if is 0 && (excess)

    int state = state_start; // Initial start state
    token->type = token_type_empty; // Set token type that is empty

    if(indentation_flag == true)
    {
        state = state_EOL;
    }

    // Control if we have to generate some other dedent token
    if(dedent_flag == true)
    {
        if (stackEmpty(stack) == 0)
        {
            stackTop(stack, &stack_top_char); // Get top char from stack
        }
        if (indentation_count < stack_top_char) // Cmp "white spaces" from dedent with top value from stack
        {
            state = state_EOL; // We have to control if we should generate next dedent token
        }
        else
        {
            if(indentation_count != stack_top_char)
            {
                indentation_count = 0; // Count of white space set on zero
                fprintf(stderr, "Inconsistent dedent \n");
                return free_source(error_lexical, str);
            }
            dedent_flag = false;
            indentation_count = 0; // Count of white space set on zero
        }
    }

    // Reading characters from source file
    while (true)
    {
        c = (char) getc(source_file); // Give me one character from source file

        if(c == ' ' && first_line_indented == false)
        {
            state = state_EOL;
        }
        first_line_indented = true;

        switch (state)
        {
            case (state_start):
                if (isalpha(c) || c == '_') // If first char of word is alpha or '_'
                {
                    if (!add_char_to_lexem_string(str, c)) // Add char to lexem string
                    {
                        free_source(error_internal, str);
                    }
                    state = state_identifier_keyword;
                }
                else if (isdigit(c)) // If first char of word is digit
                {
                    excess_zero = c; // Assign first number from number (cmp if is excess zero)
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_int;
                }
                else if (c == '\n')
                {
                    indentation_flag = true;
                    token->type = token_type_EOL;
                    return free_source(token_scan_accepted, str);
                }
                else if ((isspace(c)))
                {
                    state = state_start;
                }
                else if (c == '#')
                {
                    state = state_comment;
                }
                else if (c == '"')
                {
                    state = state_documentation_string_first;
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
                    return free_source(token_scan_accepted, str);
                }
                else if (c == '-')
                {
                    token->type = token_type_minus;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == '*')
                {
                    token->type = token_type_mul;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == '/')
                {
                    token->type = token_type_div;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == '(')
                {
                    token->type = token_type_left_bracket;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == ')')
                {
                    token->type = token_type_right_bracket;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == ':')
                {
                    token->type = token_type_colon;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == ',')
                {
                    token->type = token_type_comma;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == EOF)
                {
                    token->type = token_type_EOF;
                    return free_source(token_scan_accepted, str);
                }
                else
                {
                    return free_source(error_lexical, str);
                }
                break;


            case (state_identifier_keyword):

                if (isalnum(c) || c == '_') // If next characters are 0-9 A-Z  or '_' then add to lexem string
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_source(error_lexical, str);
                    }
                }
                else // When next character will be someting other
                {
                    ungetc(c, source_file); // Get back char to source file
                    return isIdentifier_Keyword(str, token); // Check types and return token
                }
                break;


            case (state_int):

                // If first number from some number is zero and then is another number in queue then error (EXCESS ZERO)
                if (excess_zero == '0' && isdigit(c))
                {
                    fprintf(stderr, "Excess zero !\n");
                    return free_source(error_lexical, str);
                }

                if (isdigit(c))
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_source(error_internal, str);
                    }
                }
                else if (c == '.')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_source(error_internal, str);
                    }
                    state = state_point;
                }
                else if (c == 'E' || c == 'e')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_source(error_internal, str);
                    }
                    state = state_exponent;
                }
                else
                {
                    ungetc(c, source_file);
                    return isInteger(str, token);
                }
                break;


            case (state_comment):

                // Control if is just line commentar or documentation string
                if (c == '\n' && documentation_flag == false)
                {
                    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s))
                    {
                        return free_source(error_internal, str);
                    }
                    token->type = token_type_line_comment;
                    ungetc(c, source_file);
                    return free_source(token_scan_accepted, str);
                }
                else if(c == '\\')
                {
                    c = getc(source_file);
                    if (c == '"')
                    {
                        c = '\"';
                        if(!add_char_to_lexem_string(str, c))
                        {
                            free_source(error_internal, str);
                        }
                        break;
                    }
                }
                else if (c == '"')
                {
                    state = state_documentation_string_finish_first;
                }
                else if (c == EOF)
                {
                    ungetc(c, source_file);
                    return free_source(error_lexical, str);
                }
                else
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                }
                break;


            case (state_point):

                if (isdigit(c))
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_float;
                }
                else
                {
                    free_source(error_lexical, str);
                }
                break;


            case (state_exponent):

                if (c == '+' || c == '-') {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_exponent_assigned;
                }
                else if (isdigit(c))
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_exponent_finish;
                }
                else
                {
                    free_source(error_lexical, str);
                }
                break;


            case (state_exponent_assigned):

                if (isdigit(c)) {
                    if (!add_char_to_lexem_string(str, c)) {
                        free_source(error_internal, str);
                    }
                    state = state_exponent_finish;
                } else {
                    ungetc(c, source_file);
                    free_source(error_lexical, str);
                }
                break;


            case (state_float):

                if (isdigit(c))
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                }
                else if (c == 'E' || c == 'e')
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_exponent;
                }
                else
                {
                    ungetc(c, source_file);
                    return isFloat(str, token);
                }
                break;


            case (state_exponent_finish):

                if (isdigit(c))
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                }
                else
                {
                    ungetc(c, source_file);
                    return (isFloat(str, token));
                }
                break;


            case (state_string):

                if (c < 32)
                {
                    return free_source(error_lexical, str);
                }
                else if (c == '\'')
                {
                    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s))
                    {
                        return free_source(error_internal, str);
                    }
                    token->type = token_type_str;
                    return free_source(token_scan_accepted, str);
                }
                else if (c == '\\')
                {
                    state = state_escape;
                }
                else
                {
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                }
                break;


            case (state_escape):

                if (c < 32)
                {
                    return free_source(error_lexical, str);
                }
                else if (c == 'n')
                {
                    c = '\n';
                    if (!add_char_to_lexem_string(str, c))
                    {
                        return free_source(error_internal, str);
                    }
                    state = state_string;

                }
                else if (c == '\'')
                {
                    c = '\'';
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_string;
                }
                else if (c == '"')
                {
                    c = '\"';
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_string;
                }
                else if (c == 't')
                {
                    c = '\t';
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                        state = state_string;
                }
                else if (c == '\\')
                {
                    c = '\\';
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_string;
                }
                else if (c == 'x')
                {
                    state = state_escape_hexadecimal_first;
                }
                else
                {
                    char tmp = '\\';
                    if (!add_char_to_lexem_string(str, tmp))
                    {
                        free_source(error_internal, str);
                    }
                    if (!add_char_to_lexem_string(str, c))
                    {
                        free_source(error_internal, str);
                    }
                    state = state_string;
                }
                break;


            case (state_escape_hexadecimal_first):

                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
                {
                    hex_number[0] = c;
                    char bad_esc = c;
                    c = (char) getc(source_file);

                    if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
                    {
                        hex_number[1] = c;
                        if(documentation_flag == true)
                        {
                            state = state_comment;
                        }
                        else
                        {
                            state = state_string;
                        }

                        int value = (int) strtol(hex_number, NULL, 16);
                        c = (char) value;
                        if (!add_char_to_lexem_string(str, c))
                        {
                            return free_source(error_internal, str);
                        }
                    }
                    else
                    {
                        if(documentation_flag == true)
                        {
                            state = state_comment;
                        }
                        else
                        {
                            state = state_string;
                        }
                        char tmp = '\\';
                        char tmp2 = 'x';
                        ungetc(c, source_file);
                        if (!add_char_to_lexem_string(str, tmp))
                        {
                            return free_source(error_internal, str);
                        }
                        if (!add_char_to_lexem_string(str, tmp2))
                        {
                            return free_source(error_internal, str);
                        }
                        if (!add_char_to_lexem_string(str, bad_esc))
                        {
                            return free_source(error_internal, str);
                        }
                    }
                }
                else
                {
                    return free_source(error_lexical, str);
                }
                break;


            case (state_less_then):

                if (c == '=')
                {
                    token->type = token_type_LEQ;
                    return free_source(token_scan_accepted, str);
                }
                else
                {
                    token->type = token_type_LT;
                    ungetc(c, source_file);
                    return free_source(token_scan_accepted, str);
                }


            case (state_more_then):

                if (c == '=')
                {
                    token->type = token_type_MEQ;
                    return free_source(token_scan_accepted, str);
                }
                else
                {
                    token->type = token_type_MT;
                    ungetc(c, source_file);
                    return free_source(token_scan_accepted, str);
                }


            case (state_equal):

                if (c == '=')
                {
                    token->type = token_type_EQ;
                    return free_source(token_scan_accepted, str);
                }
                else
                {
                    token->type = token_type_assign;
                    ungetc(c, source_file);
                    return free_source(token_scan_accepted, str);
                }


            case (state_exclamation_mark):

                if (c == '=')
                {
                    token->type = token_type_NEQ;
                    return free_source(token_scan_accepted, str);
                }
                else
                {
                    ungetc(c, source_file);
                    return free_source(error_lexical, str);
                }


            case (state_EOL):
                if (c == ' ')
                {
                    indentation_count++; // Counting white spaces before first no white char
                    state = state_EOL;
                }
                else
                {
                    if(stackEmpty(stack) == 0)
                    {
                        stackTop(stack, &stack_top_char);
                    }
                    if(indentation_count == stack_top_char || c == '#' || c == '"' || c == EOF || c == '\n')
                    {
                        ungetc(c, source_file);
                        indentation_count = 0; // set counting lines on zero
                        dedent_flag = false; // set indentation flag again on false
                        indentation_flag = false;
                        state = state_start;
                    }
                    else if(indentation_count > stack_top_char ) // If new identation is greater then push it to stack
                    {
                        stackPush(stack, indentation_count);
                        ungetc(c, source_file);
                        indentation_count = 0; // set counting lines on zero
                        dedent_flag = false; // set indentation flag again on false
                        indentation_flag = false;
                        token->type = token_type_indent;
                        return free_source(token_scan_accepted, str);
                    }
                    else if(indentation_count < stack_top_char) // Came dedent so we will poping characters from stack
                    {
                        if(stackEmpty(stack) == 1 && indentation_count > 0)
                        {
                            indentation_count = 0;
                            dedent_flag = false;
                            indentation_flag = false;
                            fprintf(stderr, "Incorrect indentation \n");
                            return free_source(error_lexical, str);
                        }
                        if(indentation_count == stack_top_char)
                        {
                            ungetc(c, source_file);
                            dedent_flag = false;
                            indentation_flag = false;
                            indentation_count = 0;
                            token->type = token_type_dedent;
                            return free_source(token_scan_accepted, str);
                        }
                        else if(indentation_count > stack_top_char)
                        {
                            ungetc(c,source_file);
                            indentation_count = 0;
                            dedent_flag = false;
                            indentation_flag = false;
                            fprintf(stderr, "Incorrect indentation \n");
                            return free_source(error_lexical, str);
                        }
                        else if(indentation_count < stack_top_char)
                        {
                            stackPop(stack);
                            ungetc(c, source_file);
                            dedent_flag = true;
                            indentation_flag = false;
                            token->type = token_type_dedent;
                            return free_source(token_scan_accepted, str);
                        }
                    }
                }
                break;


            case (state_documentation_string_first):
                if(c == '"')
                {
                    state = state_documentation_string_second;
                }
                else
                {
                    ungetc(c , source_file);
                    fprintf(stderr, "Documentation string wrong format !");
                    return free_source(error_lexical, str);
                }
                break;


            case (state_documentation_string_second):
                if(c == '"')
                {
                    documentation_flag = true;
                    state = state_comment;
                }
                else
                {
                    ungetc(c , source_file);
                    fprintf(stderr, "Documentation string wrong format !");
                    return free_source(error_lexical, str);
                }
                break;


            case (state_documentation_string_finish_first):
                if (c == '"')
                {
                    state = state_documentation_string_finish_second;
                }
                else
                {
                    ungetc(c , source_file);
                    fprintf(stderr, "Documentation string wrong format !");
                    return free_source(error_lexical, str);
                }
                break;


            case (state_documentation_string_finish_second):
                if (c == '"')
                {
                    documentation_flag = false;

                    if (!copy_lexem_string_to_attribute_string(str, token->attribute.s))
                    {
                        return free_source(error_internal, str);
                    }

                    token->type = token_type_documentation_comment;
                    return free_source(token_scan_accepted, str);
                }
                else
                {
                    ungetc(c , source_file);
                    fprintf(stderr, "Documentation string wrong format !");
                    return free_source(error_lexical, str);
                }
        }
    }
}




























