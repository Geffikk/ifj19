/************************ LEXEM_STRING *************************
 * @author : Maros Geffert <xgeffe00@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Lexem_string header
***********************************************************/

#ifndef IFJ_LEXEM_STRING_H
#define IFJ_LEXEM_STRING_H

#include <stdbool.h>

typedef struct {
    char* string;
    int length_of_lexem_string;
    int allocated_size_of_lexem_string;
} Lexem_string;

/** Delete all characters from LEXEM_STRING
 *
 * @param s pointer on INPUT_STRING */
void lexem_string_clear(Lexem_string *s);

/** Incialize LEXEM_STRING
 *
 * @param s is pointer on INPUT_STRING
 * @return TRUE if inicializatiom was successful */
bool lexem_string_init(Lexem_string *s);

/** Add character to LEXEM_STRING
 *
 * @param s is pointer on INPUT_STRING
 * @param c is character which i am adding
 * @return is TRUE if adding was completed successful */
bool add_char_to_lexem_string(Lexem_string *s ,char c);

/** Add string to LEXEM_STRING
 *
 * @param s is pointer on INPUT_STRING
 * @param add_string is a string which i am adding
 * @return is TRUE if adding a string was completed successful */
bool add_string_to_lexem_string(Lexem_string *s, const char *add_string);

 /** Compare a LEXEM_STRING with some another STRING
  *
  * @param s is pointer on INPUT_STRING
  * @param cmp_string is string which i am comparing
  * @return is TRUE if both strings are equal otherwise FALSE
  */
bool cmp_lexem_string_with_another_string(Lexem_string *s, const char *cmp_string);

/** Copy a LEXEM_STRING to ATTRIBUTE_STRING
 *
 * @param s is pointer on LEXEM_STRING
 * @param d is pointer on ATTRIBUTE_STRING
 * @return is TRUE if copy wass succesfull
 */
bool copy_lexem_string_to_attribute_string(Lexem_string *s, Lexem_string *d);

#endif //IFJ_LEXEM_STRING_H