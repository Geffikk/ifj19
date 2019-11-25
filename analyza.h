//
// Created by patres on 25. 10. 2019.
//

#ifndef IFJ_19_ANALYZA_H
#define IFJ_19_ANALYZA_H

#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"

//POMOCNE FUNKCIE




typedef struct
{
    Sym_table global_table;		/// Global symbol table
    Sym_table local_table;		/// Local symbol table

    Token token;				/// Token

    TData* current_id;			/// ID of currently processed function
    TData* lhs_id;				/// ID of left-hand-side variable
    TData* rhs_id;				/// ID of right-hand-side function (expression?)

    int param_index;		/// Index of currently checked param
    int label_index;			/// Index for generating unique labels.
    int label_deep;				/// Deep of labes.

    bool in_function;			/// Defines if the parser is in function
    //bool in_declaration;		/// Defines if param rule should add or check it's params
    bool in_while_or_if;		/// Defines if the parser is in construction while, if or then
    //bool non_declared_function;	/// Function that has been only defined
} PData;

int analyza();

#endif //IFJ_19_ANALYZA_H
