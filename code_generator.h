
/***********************************************************
 * @author : Andrej Pavlovic <xpavlo14@stud.fit.vutbr.cz>
***********************************************************/

#ifndef CODE_GENERATOR_HEAD
#define CODE_GENERATOR_HEAD

#include <stdbool.h>

#include "parser.h"

Lexem_string IFJcode19;

typedef enum {
	INT,
	FLOAT,
	NONE,
	GLOBAL,
	LOCAL,
	STRING
} type;

/** Function for generation head of program after complier start
 *
 ***/
bool Gen_Start ();


/** Function for adjust term to IFJcode19 shape
 *
 *  @param term - term
 *  @param type - term type
 *  @return adjusted term
 ***/
char* Term_adjustment (const char *term, const int type);


/** Function for return generation
 *
 ***/
bool Gen_return ();


/** Function for cast most upper stack operand from int to float
 *
 ***/
bool Gen_cast_stack_op1 ();


/** Function for cast second most upper stack operand from int to float
 *
 ***/
bool Gen_cast_stack_op2 ();


/** Function for push operand to stack
 *
 *  @param term - term
 ***/
bool Gen_push_stack_op (const char *term);


/** Function for variable definition
 *
 *  @param var_id - variable identificator
 ***/
bool Gen_var_def (const char *var_id);


/** Function for expression or function return value save to variable
 *
 *  @param var_id - variable identificator
 ***/
bool Gen_save_expr_or_retval (const char *var_id);


/** Function for string concatenation
 *
 ***/
bool Gen_string_concat ();


/** Function for data type control of variables in functions
 *
 ***/
bool Gen_type_control ();


/** Function for calculate two operands in stack
 *
 *  @param rule - type of operator
 *  @param in_function - information if operator is in function
 ***/
bool Gen_expr_calc (Rule_enumeration rule, bool in_function);


/** Function for generation of if head
 *
 ***/
unsigned long int Gen_if_head ();


/** Function for generation of else head
 *
 ***/
unsigned long int Gen_else_head ();


/** Function for generation of else foot
 *
 ***/
bool Gen_else_foot ();


/** Function for generation while label
 *
 ***/
bool Gen_while_label ();


/** Function for generation of while head
 *
 ***/
bool Gen_while_head ();


/** Function for generation of while foot
 *
 ***/
bool Gen_while_foot ();


/** Function for generation function definition head
 *
 *  @param fun_id - function idetificator
 ***/
bool Gen_function_def_head (const char *fun_id);


/** Function for pop argument after function call
 *
 *  @param term - term
 ***/
bool Gen_pop_arg (const char *term);


/** Function for generation function definition foot
 *
 *  @param fun_id - function idetificator
 ***/
bool Gen_function_def_foot (const char *fun_id);


/** Function for argument push before function call
 *
 *  @param term - term
 ***/
bool Gen_push_arg (const char *term);


/** Function for function call
 *
 *  @param fun_id - function idetificator
 ***/
bool Gen_function_call (const char *fun_id);


#endif // CODE_GENERATOR_HEAD