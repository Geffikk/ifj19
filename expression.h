/**
*
*
*/

#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "analysis.h"

typedef enum
{
	SYMBOL_EQUAL,		// =					0
	SYMBOL_NOT_EQUAL,	// != 					1
	SYMBOL_LESS_EQUAL,	// <=					2
	SYMBOL_MORE_EQUAL,	// >=					3 
	SYMBOL_LESS,		// <					4
	SYMBOL_MORE,		// >					5
	SYMBOL_PLUS,		// +					6
	SYMBOL_MINUS,		// -					7
	SYMBOL_MULTIPLY,	// *					8
	SYMBOL_DIVIDE,		// /  (NORMAL DIVIDING)			9
	SYMBOL_DIVIDE_INTEGER,	// // (DIVIDING INT NUMBERS) 		10
	SYMBOL_RIGHT_BRACKET,	// )					11
	SYMBOL_LEFT_BRACKET,	// (					12
	SYMBOL_INTEGER,		// type int				13
	SYMBOL FLOAT,		// type double				14
	SYMBOL_STRING,		// type char/string			15
	SYMBOL_IDENTIFIER,	// ID					16
	SYMBOL_DOLLAR,		// symbol for something			17
	SYMBOL_STOP, 		// stop symbol on stack			18
	SYMBOL_NOT_DEFINED	// not a symbol				19
} 
Symbol_enumeration;

typedef enum
{
	RULE_EQUAL,		// RULE -> X = Y	0
	RULE_NOT_EQUAL,		// RULE -> X <> Y	1
	RULE_LESS_EQUAL,	// RULE -> X <= Y	2
	RULE_MORE_EQUAL,	// RULE -> X >= Y  	3
	RULE_LESS,		// RULE -> X < Y	4
	RULE_MORE,		// RULE -> x > Y	5
	RULE_OPERAND,		// RULE -> i		6
	RULE_PLUS,		// RULE -> X + Y	7
	RULE_MINUS,		// RULE -> X - Y	8
	RULE_MULTIPLY,		// RULE -> X * Y	9
	RULE_DIVIDE,		// RULE -> X / Y	10
	RUlE_DIVIDE_INT,	// RULE -> X // Y 	11
	RULE_BRACKETS,		// RULE -> (X)		12
	RULE_NOT_DEFINED	// not a rule		13
} 
Rule_enumeration;

int expression(PData* data);

#endif
