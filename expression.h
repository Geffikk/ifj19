
/************************ SCANNER *************************
 * @author : Martin Valach <xvalac12@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Expression header
***********************************************************/

#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "analyza.h"


typedef enum
{
    SFT,	// < SHIFT 	0
    MCH,	// = MATCH 	1
    RED,	// > REDUCE 2
    ERR	    // # ERROR 	3
}
Sign_enumeration;

typedef enum
{

    INDEX_EQUAL,                    // index for ==                         0
    INDEX_NOT_EQUAL,                // index for !=                         1
    INDEX_LESS_EQUAL,               // index for <=                         2
    INDEX_MORE_EQUAL,               // index for >=                         3
    INDEX_LESS,                     // index for <                          4
    INDEX_MORE,                     // index for >                          5
    INDEX_PLUS_OR_MINUS,			// index for +, - 			            6
    INDEX_MULTIPLY_OR_DIVIDE,		// index for *, /, // 			        7
    INDEX_RIGHT_BRACKET,			// index for ) 				            8
    INDEX_LEFT_BRACKET,			    // index for ( 				            9
    INDEX_DATA,				        // index for identifier and data types 	10
    INDEX_DOLLAR				    // index for $				            11
}
Index_enumeration;

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
	SYMBOL_IDENTIFIER,	    // ID					        16
	SYMBOL_DOLLAR,		    // symbol for something			17
	SYMBOL_STOP, 		    // stop symbol on stack			18
	SYMBOL_NOT_DEFINED	    // not a symbol				    19
} 
Symbol_enumeration;

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
	RULE_DIVIDE,		// RULE E -> E / E	1   0
	RULE_DIVIDE_INT,	// RULE E -> E // E 	11
	RULE_BRACKETS,		// RULE E -> (E)		12
	RULE_NOT_DEFINED	// not a rule		    13
} 
Rule_enumeration;




int expression(Parser_data* data);

#endif
