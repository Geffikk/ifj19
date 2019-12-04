
/****************** LEXEM_STRING **************************
 * @author : Maros Geffert <xgeffe00@stud.fit.vutbr.cz>
 * Subject : IFJ
***********************************************************/

#include <string.h>
#include <stdlib.h>

#include "lexem_string.h"

void lexem_string_clear(Lexem_string *s)
{
    s->length_of_lexem_string = 0;
    s->string[s->length_of_lexem_string] = '\0';
}
bool lexem_string_init(Lexem_string *s)
{
    s->string = (char*)malloc(sizeof(char));
    if (!s->string)
    {
        return false;
    }

    s->length_of_lexem_string = 0;
    s->string[s->length_of_lexem_string] = '\0';
    s->allocated_size_of_lexem_string = sizeof(char);

    return true;
}

bool add_char_to_lexem_string(Lexem_string *s, char c)
{
    if(s->length_of_lexem_string + 1 >= s->allocated_size_of_lexem_string)
    {
        if(!(s->string = (char*)realloc(s->string, s->length_of_lexem_string + 8)))
        {
            return false;
        }
        s->allocated_size_of_lexem_string = s->length_of_lexem_string + 8;
    }
    s->string[s->length_of_lexem_string++] = c;
    s->string[s->length_of_lexem_string] = '\0';
    return true;
}

bool add_string_to_lexem_string(Lexem_string *s, const char *add_string)
{
    unsigned int length_of_new_string = (unsigned int) strlen(add_string);

    if (s->length_of_lexem_string + length_of_new_string + 1 >= s->allocated_size_of_lexem_string)
    {
        unsigned int new_size = length_of_new_string + s->length_of_lexem_string + 1;
        s->string = (char*)realloc(s->string, new_size);
        if (!s->string)
        {
            return false;
        }
        s->allocated_size_of_lexem_string = new_size;
    }

    s->length_of_lexem_string = s->length_of_lexem_string + length_of_new_string;
    strcat(s->string, add_string);
    s->string[s->length_of_lexem_string] = '\0';
    return true;
}

bool cmp_lexem_string_with_another_string(Lexem_string *s, const char *cmp_string)
{
    int result = strcmp(s->string, cmp_string);

    if (result == 0)
    {
        return true;
    }

    return false;
}

bool copy_lexem_string_to_attribute_string(Lexem_string *s, Lexem_string *d)
{

    if (s->length_of_lexem_string + 1 >= d->allocated_size_of_lexem_string)
    {
        d->string = (char *) realloc(d->string, s->length_of_lexem_string+1);
        if(!d->string)
        {
            return false;
        }
    }

    strcpy(d->string, s->string);
    d->length_of_lexem_string = s->length_of_lexem_string;
    return true;
}
