
	////////////////////////////////////////////////////
	// Author: Andrej Pavlovic <xpavlo14@vutbr.cz>
	////////////////////////////////////////////////////

#ifndef CODE_GENERATOR_HEAD
#define CODE_GENERATOR_HEAD

#include <stdbool.h>

#include "expression.h"

Lexem_string IFJcode19;

bool Gen_Start ();
bool Gen_Finish ();
char* Term_adjustment (const char *term, const int data_type);
bool Gen_return ();
bool Gen_cast_stack_op1 ();
bool Gen_cast_stack_op2 ();
bool Gen_push_stack_op (const char *term);
bool Gen_var_def (const char *var_id);
bool Gen_save_expr_or_retval (const char *var_id);
bool Gen_string_concat ();
bool Gen_expr_calc (Rule_enumeration rule);
bool Gen_if_head ();
bool Gen_else_head ();
bool Gen_else_foot ();
bool Gen_while_label ();
bool Gen_while_head ();
bool Gen_while_foot ();
bool Gen_function_def_head (const char *fun_id);
bool Gen_pop_arg (const char *term);
bool Gen_function_def_foot (const char *fun_id);
bool Gen_push_arg (const char *term);
bool Gen_function_call (const char *fun_id);

#endif // CODE_GENERATOR_HEAD