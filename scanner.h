//
// Created by maros on 10/19/2019.
//

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#include <stdio.h>
#include "lexem_string.h"
#include "stack_indent_dedent.h"

#define token_scan_accepted 0
#define token_syntax_accepted 0
#define error_lexical 1
#define error_syntax 2
#define error_semantic 3
#define error_semantic_compatibility 4
#define error_semantic_bad_count_param 5
#define error_semantic_others 6
#define error_div_zero 9
#define error_internal 99

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
    token_type_exponent,
    token_type_left_bracket,
    token_type_right_bracket,
    token_type_assign,
    token_type_colon,

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
} Token;

void set_source_file(FILE *f);
void set_lexem_string(Lexem_string *string);
int get_token(Token *token);

#endif //IFJ_SCANNER_H
