#include <stdlib.h>
#include <string.h>

#include "symtable.h"

static unsigned long hash_function(const char *str)
{
	int hash = 0, x = 0;

	for (char c = *str; c != '\0'; c = *(++str))
	{
		hash = (hash << 4) + c;
		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
		}
		hash &= ~x;
	}

	return hash % MAX_SYMTABLE_SIZE;
}

void sym_table_init(Sym_table *table)
{
	if (table == NULL)
    {
	    return;
    }

	int i = 0;
	while(i < MAX_SYMTABLE_SIZE)
    {
        (*table)[i] = NULL;
        i++;
    }
}

IData *sym_table_add_symbol(Sym_table *table, const char *key, bool* malloc_failed)
{
	*malloc_failed = false;

	if (table == NULL || key == NULL)
	{
		*malloc_failed = true;
		return NULL;
	}

	int index = hash_function(key);
	Sym_table_item *tmp_last = NULL;

	for (Sym_table_item *tmp = (*table)[index]; tmp != NULL; tmp = tmp->next)
	{
		if (!strcmp(key, tmp->key))
		{
			return NULL;
		}

		tmp_last = tmp;
	}

	Sym_table_item *new_entry = (Sym_table_item *)malloc(sizeof(Sym_table_item));
	if (new_entry == NULL)
	{
		*malloc_failed = true;
		return NULL;
	}

	if (!(new_entry->key = (char *)malloc((strlen(key) + 1) * sizeof(char))))
	{
		free(new_entry);
		*malloc_failed = true;
		return NULL;
	}
	if (!(new_entry->data.params = (Lexem_string *)malloc(sizeof(Lexem_string))))
	{
		free(new_entry->key);
		free(new_entry);
		*malloc_failed = true;
		return NULL;
	}
	if (!lexem_string_init(new_entry->data.params))
	{
		free(new_entry->key);
		free(new_entry);
		free(new_entry->data.params);
		*malloc_failed = true;
		return NULL;
	}

	strcpy(new_entry->key, key);
	new_entry->data.identifier = new_entry->key;
	new_entry->data.type = DATA_TYPE_NOT_DEFINED;
	new_entry->data.defined = false;
	new_entry->data.global = false;
	new_entry->data.is_variable = false;
	new_entry->data.is_function = false;
	new_entry->next = NULL;

	if (tmp_last == NULL)
    {
        (*table)[index] = new_entry;
    }
	else{
        tmp_last->next = new_entry;
	}

	return &new_entry->data;
}

bool sym_table_add_parameter(IData *data, int data_type)
{
	if (data == NULL){
        return false;
	}


	if(data_type == DATA_TYPE_INTEGER)
    {
        if (!add_char_to_lexem_string(data->params, 'i'))
        {
            return false;
        }
    }
	else if(data_type == DATA_TYPE_FLOAT)
    {
        if (!add_char_to_lexem_string(data->params, 'f'))
        {
            return false;
        }
    }
	else if(data_type == DATA_TYPE_STRING)
    {
        if (!add_char_to_lexem_string(data->params, 's'))
        {
            return false;
        }
    }
    else if(data_type == DATA_TYPE_NOT_DEFINED)
    {
        if (!add_char_to_lexem_string(data->params, 'n'))
        {
            return false;
        }
    }

	return true;
}

IData *sym_table_search(Sym_table *table, const char *key)
{
	if (table == NULL || key == NULL)
    {
	    return NULL;
    }

	unsigned long index = hash_function(key);

	Sym_table_item *tmp = (*table)[index];
	while(tmp != NULL)
    {
        if (!strcmp(key, tmp->key))
        {
            return &tmp->data;
        }

        tmp = tmp->next;
    }

	return NULL;
}

void sym_table_free(Sym_table *table)
{
	if (table == NULL)
    {
	    return;
    }

	Sym_table_item *tmp_next = NULL;

	for (int i = 0; i < MAX_SYMTABLE_SIZE; i++)
	{
		for (Sym_table_item *tmp = (*table)[i]; tmp != NULL; tmp = tmp_next)
		{
			tmp_next = tmp->next;
			free(tmp->key);

			if (tmp->data.params != NULL)
			{
                lexem_string_clear(tmp->data.params);
				free(tmp->data.params);
			}

			free(tmp);
		}

		(*table)[i] = NULL;
	}
}
