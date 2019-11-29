
	////////////////////////////////////////////////////
	// Author: Andrej Pavlovic <xpavlo14@vutbr.cz>
	////////////////////////////////////////////////////

#ifndef CODE_GENERATOR_HEAD
#define CODE_GENERATOR_HEAD

#include <stdbool.h>


bool Gen_Start ();
bool Gen_Finish ();
char* Term_adjustment (const char *term, const int data_type);
bool Gen_var_def (const char *term);
bool Gen_assignment (const char *dst_term, const char *src_term);
bool Gen_return (const char *term);
bool Gen_expression (const char *term);
bool Gen_if_head ();
bool Gen_else_head ();
bool Gen_else_foot ();
bool Gen_while_head ();
bool Gen_while_foot ();
bool Gen_function_def_head (const char *fun_id);
bool Gen_pop_arg (const char *term);
bool Gen_function_def_foot (const char *fun_id);
bool Gen_push_arg (const char *term);
bool Gen_function_call (const char *fun_id);
bool Gen_print (const char *term);




////////////////////////////////////////////////////////////////
//			START OF DEBUGGING SECTION
//

#include "lexem_string.h"

Lexem_string IFJcode19; // Output dynamic string


//			END OF DEBUGGING SECTION
////////////////////////////////////////////////////////////////


#endif // CODE_GENERATOR_HEAD
