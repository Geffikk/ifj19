
/************************* ANALYZA *************************
 * @author : Patrik Tomov <xtomov0200@stud.fit.vutbr.cz>
***********************************************************/

#ifndef IFJ_19_ANALYZA_H
#define IFJ_19_ANALYZA_H

#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"





typedef struct
{
    Sym_table global_table;
    Sym_table local_table;

    TData* current_process_id;
    TData* left_side_id;
    TData* right_side_id;

    Token token;

    int param_index;

    bool in_function;
    bool in_while_or_if;
    bool was_in_main;

} Parser_data;

// declaration of functions (rules)
int prog_body(Parser_data* data); //definicie funkcii
int main_body(Parser_data* data); //hlavne telo programu
int end_main(Parser_data* data); // koniec hlavneho tela
int par_list(Parser_data* data);  //spravovanie parametru ->
int par_list2(Parser_data* data); // ak ich je viac
int statement(Parser_data* data); //if, while, etc..
int def_value(Parser_data* data); // id = <def_value>
int arg_list(Parser_data* data); //argument pri volani funkcii ->
int arg_list2(Parser_data* data); //ak ich je viac
int value(Parser_data* data);
int print_rule(Parser_data* data);
int analyza();


#endif //IFJ_19_ANALYZA_H
