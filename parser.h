
/************************* Syntactic and semantic analysis *************************
 * @author : Patrik Tomov <xtomov02@stud.fit.vutbr.cz>
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
***********************************************************/

#ifndef IFJ_19_ANALYZA_H
#define IFJ_19_ANALYZA_H

#include <stdbool.h>
#include "symtable.h"
#include "scanner.h"


/** Type of SIGN **/
typedef enum
{
    LSS,	// LESS           <  	0
    MCH,	// MATCH          = 	1
    GRT,	// REDUCE         >     2
    END	    // END or ERROR  	    3
}
Sign_enumeration;

/** Type of INDEX **/
typedef enum
{
    INDEX_EQUAL,                    // index for ==                         0
    INDEX_NOT_EQUAL,                // index for !=                         1
    INDEX_LESS_EQUAL,               // index for <=                         2
    INDEX_MORE_EQUAL,               // index for >=                         3
    INDEX_LESS,                     // index for <                          4
    INDEX_MORE,                     // index for >                          5
    INDEX_PLUS,        			    // index for +			                6
    INDEX_MINUS,		        	// index for -			                7
    INDEX_MULTIPLY,         		// index for *      			        8
    INDEX_DIVIDE,	            	// index for /              	        9
    INDEX_DIVIDE_INTEGER,   		// index for // 	                    10
    INDEX_RIGHT_BRACKET,			// index for ) 				            11
    INDEX_LEFT_BRACKET,			    // index for ( 				            12
    INDEX_DATA,				        // index for identifier and data types 	13
    INDEX_DOLLAR				    // index for $				            14
}
Index_enumeration;

/** Type of SYMBOL **/
typedef enum
{
    SYMBOL_EQUAL,		    // =			        		0
    SYMBOL_NOT_EQUAL,   	// != 					        1
    SYMBOL_LESS_EQUAL,	    // <=	    		    		2
    SYMBOL_MORE_EQUAL,	    // >=		        			3
    SYMBOL_LESS,    		// <		    	    		4
    SYMBOL_MORE,	    	// >	    			    	5
    SYMBOL_PLUS,		    // +    					    6
    SYMBOL_MINUS,   		// -				    	    7
    SYMBOL_MULTIPLY,    	// *					        8
    SYMBOL_DIVIDE,		    // /  (NORMAL DIVIDING)		   	9
    SYMBOL_DIVIDE_INTEGER,	// // (DIVIDING INT NUMBERS) 	10
    SYMBOL_RIGHT_BRACKET,	// )	    		    		11
    SYMBOL_LEFT_BRACKET,	// (		    		    	12
    SYMBOL_INTEGER,		    // type int		    		    13
    SYMBOL_FLOAT,		    // type double		    		14
    SYMBOL_STRING,		    // type char/string		    	15
    SYMBOL_NONE,            // type None                    16
    SYMBOL_IDENTIFIER,	    // ID					        17
    SYMBOL_DOLLAR,		    // symbol for something			18
    SYMBOL_STOP, 		    // stop symbol on stack			19
    SYMBOL_NOT_DEFINED	    // not a symbol				    20
}
Symbol_enumeration;

/** Type of RULE **/
typedef enum
{
    RULE_EQUAL,		    // RULE E -> E = E	    0
    RULE_NOT_EQUAL,		// RULE E -> E != E	    1
    RULE_LESS_EQUAL,	// RULE E -> E <= E	    2
    RULE_MORE_EQUAL,	// RULE E -> E >= E  	3
    RULE_LESS,		    // RULE E -> E < E      4
    RULE_MORE,		    // RULE E -> E > E	    5
    RULE_OPERAND,		// RULE E -> i		    6
    RULE_PLUS,		    // RULE E -> E + E	    7
    RULE_MINUS,	    	// RULE E -> E - E	    8
    RULE_MULTIPLY,		// RULE E -> E * E	    9
    RULE_DIVIDE,		// RULE E -> E / E	    10
    RULE_DIVIDE_INT,	// RULE E -> E // E 	11
    RULE_BRACKETS,		// RULE E -> (E)		12
    RULE_NOT_DEFINED	// not a known rule    13
}
Rule_enumeration;

/** Parser data **/
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

/** Declaration of functions (rules) **/
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
