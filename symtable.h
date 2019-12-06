#ifndef _SYMTABLE_H
#define _SYMTABLE_H


#include <stdbool.h>

#include "lexem_string.h"


#define MAX_SYMTABLE_SIZE 50


/** Type of DATA_TYPE **/
typedef enum
{
	DATA_TYPE_NOT_DEFINED,
	DATA_TYPE_INTEGER,
	DATA_TYPE_FLOAT,
	DATA_TYPE_STRING,
} Data_type;


/** Representation of item **/
typedef struct
{
	Data_type type;		  // Data type of symbol / return type of function
	bool defined;		  // Defined if current function or variable was defined
	Lexem_string *params; // parameters in string
	char *identifier;	  // Data identifier(key)
	bool global;		  // Global or local variable.
	bool is_function;     // Is function
	bool is_variable;     // Is variable
} TData;


/** Representation of item in table **/
typedef struct htab_list_entry
{
	char *key;                       // identifier
	TData data;                      // data
	struct htab_list_entry *next;    // pointer to next item
} Sym_table_entry;

/** Symbol table **/
typedef Sym_table_entry* Sym_table[MAX_SYMTABLE_SIZE];


/** Incialization of table
 *
 * @param table - is pointer to table
 */
void sym_table_init(Sym_table *table);


/** Function adds item to table
 *
 * @param table - is pointer to table
 * @param key - is pointer to key
 * @return null if error or item is existing, otherwise pointer to added item
 */


TData *sym_table_add_symbol(Sym_table *table, const char *key, bool* malloc_failed);

/** Function adds parameter to function
 *
 * @param data - is pointer to data of function
 * @param data_type - is type of parameter to add
 * @return true if parameter was successfully added, otherwise, false
 */
bool sym_table_add_parameter(TData *data, int data_type);


/** Function finds symbol and return its data
 *
 * @param table - is pointer to table
 * @param key - is pointer to key
 * @return null if identifier(key) was not found or pointer to data of symbol if was found
 */
TData *sym_table_search(Sym_table *table, const char *key);


/** Free all variables from table
 *
 * @param table - pointer to table
 */
void sym_table_free(Sym_table *table);

#endif //_SYMTABLE_H
