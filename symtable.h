#ifndef _SYMTABLE_H
#define _SYMTABLE_H


#include <stdbool.h>

#include "lexem_string.h"


#define MAX_SYMTABLE_SIZE 50


/**
 * @enum Data types.
 */
typedef enum
{
	DATA_TYPE_NOT_DEFINED,	/// Data type undefined
	DATA_TYPE_INTEGER,		/// Integer data type
	DATA_TYPE_FLOAT,	/// Float data type
	DATA_TYPE_STRING,	/// String data type
} Data_type;

/**
 * @struct Item data representation.
 */
typedef struct
{
	Data_type type;			/// Data type of symbol / return type of function
	bool defined;			/// Defined if current function was defined
	Lexem_string *params;   /// parameters in string form
	char *identifier;		/// Data identifier (key).
	bool global;			/// Global (internal) variable.
	bool is_function;
	bool is_variable;
} TData;

/**
 * @struct Symbol table item representation.
 */
typedef struct htab_list_entry
{
	char *key; /// identifier
	TData data; /// data
	struct htab_list_entry *next; /// ptr to next item
} Sym_table_entry;

// Symbol table
typedef Sym_table_entry* Sym_table[MAX_SYMTABLE_SIZE];


/**
 * Initialisation of symbol table.
 *
 * @param table Pointer to table.
 */
void sym_table_init(Sym_table *table);

/**
 * Appends item to symbol table of.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @param alloc_success True if allocation failed, otherwise true. 
 * @return Returns NULL if error or item is existing else returns pointer to added item.
 */
TData *sym_table_add_symbol(Sym_table *table, const char *key, bool* malloc_failed);

/**
 * Function appends parameter to symbol.
 *
 * @param token Pointer to output token.
 * @param data_type Data type of parameter of function.
 * @return Returns true if adding was succesfull else returns false.
 */
bool sym_table_add_parameter(TData *data, int data_type);

/**
 * Function finds symbol and renturns its data.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @return NULL if symbol is not existing or pointer to data of symbol if it was successfuly found.
 */
TData *sym_table_search(Sym_table *table, const char *key);

/**
 * Function removes symbol.
 *
 * @param table Pointer to table.
 * @param key Identifier of function or variable.
 * @return Returns true if removal was successfull esle returns false.
 */
bool sym_table_remove_symbol(Sym_table *table, const char *key);

/**
 * Function frees all used resources for symbol table.
 *
 * @param table Pointer to table.
 */
void sym_table_free(Sym_table *table);


#endif //_SYMTABLE_H
