//
// Created by maros on 10/19/2019.
//

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "error.h"

#include "lexem_string.h"
#include "stack.h"

/** Type of TOKEN **/
typedef enum {
    token_type_keyword,
    token_type_identifier,

    token_type_comma,
    token_type_indent,
    token_type_dedent,
    token_type_EOL,
    token_type_EOF,
    token_type_empty,

    token_type_int,
    token_type_float,
    token_type_str,

    token_type_EQ,
    token_type_NEQ,
    token_type_MT,
    token_type_LT,
    token_type_MEQ,
    token_type_LEQ,
    token_type_plus,
    token_type_minus,
    token_type_mul,
    token_type_div,
    token_type_left_bracket,
    token_type_right_bracket,
    token_type_assign,
    token_type_colon,
    token_type_line_comment,
    token_type_documentation_comment,

} Token_types;

/** Type of KEYWORD  **/
typedef enum
{
    keyword_def,
    keyword_else,
    keyword_if,
    keyword_None,
    keyword_pass,
    keyword_return,
    keyword_while,
    keyword_str,
    keyword_int,
    keyword_float,
    keyword_inputs,
    keyword_inputi,
    keyword_inputf,
    keyword_print,
    keyword_len,
    keyword_substr,
    keyword_ord,
    keyword_chr,

} Keywords;

/** Attributes of TOKEN **/
typedef union{
    int int_number;
    float float_number;
    Lexem_string *s;
    Keywords keyword;
} token_attributes;

/** TOKEN **/
typedef struct{
    token_attributes attribute;
    Token_types type;
    int lineNo, colNo;
} Token;

/** Set SOURCE FILE (because of using in scanner.c)
 *
 * @param f - source file
 */
void set_source_file(FILE *f);

/** Set LEXEM STRING (because of using in scanner.c)
 *
 * @param string - lexem string
 */
void set_string(Lexem_string *string);

/** Set INDENTATION STACK (because of using in scanner.c)
 *
 * @param stack - indentation stack
 */
void set_stack(tStack *stack);

/** Fuction to get ONE TOKEN
 *
 * @param token - input empty token
 * @param stack - indentation stack
 * @return - output token with some (attributes, types etc.)
 */
int get_token(Token *token, tStack *stack);

#endif //IFJ_SCANNER_H
