
	////////////////////////////////////////////////////
	// Author: Andrej Pavlovic <xpavlo14@vutbr.cz>
	////////////////////////////////////////////////////



#include "code_generator.h"
#include "lexem_string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define GEN_CHAR(code) \
	if ( !add_char_to_lexem_string(&IFJcode19, code) ) return false

#define GEN_EOL() \
	if ( !add_string_to_lexem_string(&IFJcode19, "\n") ) return false

#define GEN_STRING(code) \
	if ( !add_string_to_lexem_string(&IFJcode19, code) ) return false

#define GEN_CONST_STRING_AND_EOL(code) \
	if ( !add_string_to_lexem_string(&IFJcode19, code "\n") ) return false

#define GEN_INT(code) \
	{char str[1000]; sprintf(str, "%lu", code); GEN_STRING(str);} // <------------


bool Gen_Start () {

	GEN_CONST_STRING_AND_EOL(".IFJcode19"); GEN_EOL();

	GEN_CONST_STRING_AND_EOL("DEFVAR GF@%expr_result");
	GEN_CONST_STRING_AND_EOL("CREATEFRAME"); GEN_EOL();

	GEN_CONST_STRING_AND_EOL("JUMP ?_main"); GEN_EOL();


	////////////		VESTAVENE FUNKCE

	GEN_CONST_STRING_AND_EOL("######################    build-in functions    ######################");

	// Vstavana fukcia input
	GEN_CONST_STRING_AND_EOL("# build-in function input");
	GEN_CONST_STRING_AND_EOL("LABEL input");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Vstavana fukcia len *
	GEN_CONST_STRING_AND_EOL("# build-in function len");
	GEN_CONST_STRING_AND_EOL("LABEL len");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@s");
	GEN_CONST_STRING_AND_EOL("POPS TF@s");
	GEN_CONST_STRING_AND_EOL("STRLEN LF@%retval TF@s");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Vstavana fukcia substr
	GEN_CONST_STRING_AND_EOL("# build-in function substr");
	GEN_CONST_STRING_AND_EOL("LABEL substr");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@n");
	GEN_CONST_STRING_AND_EOL("POPS TF@n");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@i");
	GEN_CONST_STRING_AND_EOL("POPS TF@i");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@s");
	GEN_CONST_STRING_AND_EOL("POPS TF@s");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Vstavana fukcia ord *
	GEN_CONST_STRING_AND_EOL("# build-in function ord");
	GEN_CONST_STRING_AND_EOL("LABEL ord");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@i");
	GEN_CONST_STRING_AND_EOL("POPS TF@i");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@s");
	GEN_CONST_STRING_AND_EOL("POPS TF@s");
	Gen_push_arg("TF@s");
	Gen_function_call("len");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@bool");
	GEN_CONST_STRING_AND_EOL("GT TF@bool TF@%retval TF@i");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_ord TF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_ord");
	GEN_CONST_STRING_AND_EOL("GT TF@bool TF@i int@0");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_ord2 TF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_ord2");
	GEN_CONST_STRING_AND_EOL("GETCHAR LF@%retval TF@s TF@i");
	GEN_CONST_STRING_AND_EOL("STRI2INT LF@%retval LF@%retval int@0");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Vstavana fukcia chr *
	GEN_CONST_STRING_AND_EOL("# build-in function chr");
	GEN_CONST_STRING_AND_EOL("LABEL chr");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@i");
	GEN_CONST_STRING_AND_EOL("POPS TF@i");
	GEN_CONST_STRING_AND_EOL("INT2CHAR LF@%retval TF@i");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Main
	GEN_CONST_STRING_AND_EOL("######################    main    ######################");
	GEN_EOL();
	GEN_CONST_STRING_AND_EOL("LABEL ?_main");

	return true;
}

bool Gen_Finish () {
	GEN_STRING("EXIT int@0");
	return true;
}

char* Term_adjustment (const char *term, const int data_type) {

	static char new_term[1000]; // max + 7
	
	switch (data_type) {
		case 1 :
			strcpy(new_term, "string@");
			break;
		case 2 :
			strcpy(new_term, "int@");
			break;
		case 3 :
			strcpy(new_term, "float@");
			break;
		case 4 :
			strcpy(new_term, "nil@");
			break;
		case 5 :
			strcpy(new_term, "GF@");
			break;
		case 6 :
			strcpy(new_term, "TF@");
			break;
	}

	strcat(new_term, term);
	
	return new_term;
}

bool Gen_var_def (const char *term) {

	GEN_STRING("DEFVAR ");
	GEN_STRING(term); GEN_EOL();

	return true;
}

bool Gen_assignment (const char *dst_term, const char *src_term) {

	GEN_STRING("MOVE ");
	GEN_STRING(dst_term);
	GEN_CHAR(' ');
	GEN_STRING(src_term);
	GEN_EOL();

	return true;
}

bool Gen_return (const char *term) {

	GEN_STRING("MOVE LF@%retval ");
	GEN_STRING(term); GEN_EOL();
	GEN_CONST_STRING_AND_EOL("RETURN");

	return true;
}

bool Gen_expression (const char *term) {

}

bool Gen_if_head () {

	static unsigned long int i = 0;

	GEN_CONST_STRING_AND_EOL("# if head");
	GEN_STRING("JUMPIFEQ ?_else_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%expr_result bool@false");

	i++;

	return true;
}

bool Gen_else_head () {
	
	static unsigned long int i = 0;

	GEN_CONST_STRING_AND_EOL("# else head");
	GEN_STRING("JUMP ?_else_end_");
	GEN_INT(i); GEN_EOL();
	GEN_STRING("LABEL ?_else_");
	GEN_INT(i); GEN_EOL();

	i++;

	return true;
}

bool Gen_else_foot () {
	
	static unsigned long int i = 0;

	GEN_CONST_STRING_AND_EOL("# else foot");
	GEN_STRING("JUMP ?_else_end_");
	GEN_INT(i); GEN_EOL();

	i++;

	return true;
}

bool Gen_while_head () {

	static unsigned long int i = 0;
	
	GEN_CONST_STRING_AND_EOL("# while head");
	GEN_STRING("LABEL ?_while_");
	GEN_INT(i); GEN_EOL();
	GEN_STRING("JUMPIFEQ ?_while_end_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%expr_result bool@false");

	i++;

	return true;
}

bool Gen_while_foot () {
	
	static unsigned long int i = 0;

	GEN_CONST_STRING_AND_EOL("# while foot");
	GEN_STRING("JUMP ?_while_");
	GEN_INT(i); GEN_EOL();
	GEN_STRING("LABEL ?_while_end_");
	GEN_INT(i); GEN_EOL();

	i++;

	return true;
}

bool Gen_function_def_head (const char *fun_id) {
	
	GEN_CONST_STRING_AND_EOL("# function head");
	GEN_STRING("JUMP ");
	GEN_STRING(fun_id);
	GEN_CONST_STRING_AND_EOL("_end");
	GEN_STRING("LABEL ");
	GEN_STRING(fun_id); GEN_EOL();

	return true;
}

bool Gen_pop_arg (const char *term) { // popuje sa od konca

	GEN_CONST_STRING_AND_EOL("# pop (pass) function's argument");
	GEN_STRING("DEFVAR ");
	GEN_STRING(term); GEN_EOL();

	GEN_STRING("POPS ");
	GEN_STRING(term); GEN_EOL();

	return true;
}

bool Gen_function_def_foot (const char *fun_id) {
	
	GEN_CONST_STRING_AND_EOL("# function foot");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_STRING("LABEL ");
	GEN_STRING(fun_id);
	GEN_CONST_STRING_AND_EOL("_end");

	return true;
}

bool Gen_push_arg (const char *term) { // pushuje sa od zaciatku

	GEN_CONST_STRING_AND_EOL("# push function's argument");
	GEN_STRING("PUSHS ");
	GEN_STRING(term); GEN_EOL();

	return true;
}

bool Gen_function_call (const char *fun_id) {
	
	GEN_CONST_STRING_AND_EOL("# function call");
	GEN_CONST_STRING_AND_EOL("DEFVAR TF@%retval");
	GEN_CONST_STRING_AND_EOL("PUSHFRAME");
	GEN_CONST_STRING_AND_EOL("CREATEFRAME");

	GEN_STRING("CALL ");
	GEN_STRING(fun_id); GEN_EOL();

	GEN_CONST_STRING_AND_EOL("POPFRAME");

	return true;
}

bool Gen_print (const char *term) { // vraciat none
	
		GEN_STRING("WRITE ");

		char tmp_str[5];
		long int len = strlen(term);

		for ( int i = 0 ; i < len ; i++ ) {

			if ( (term[i] <= 32 && term[i] >= 0) || term[i] == 35 ) {
				sprintf(tmp_str, "\\%03d", (int) term[i]);
				GEN_STRING(tmp_str);
			}

			else if ( term[i] == 92 ) {

				switch ( term[++i] ) {
					case '\"' :
					case '\'' :
						GEN_CHAR(term[i]);
						break;

					case 'n' :
						GEN_STRING("\\010");
						break;

					case 't' :
						GEN_STRING("\\009");
						break;

					case '\\' :
						GEN_STRING("\\092");
						break;

					case 'x' :
						GEN_CHAR('\\');

						tmp_str[0] = term[++i];
						tmp_str[1] = term[++i];
						tmp_str[2] = '\0';

						long int dec = strtol(tmp_str, NULL, 16);

						GEN_CHAR((dec/100)+'0');
						GEN_CHAR(((dec/10)%10)+'0');
						GEN_CHAR((dec%10)+'0');
						break;

					default :
						GEN_STRING("\\092");
						GEN_CHAR(term[i]);
				}
			}

			else {
				GEN_CHAR(term[i]);
			}
		}

	GEN_EOL();
	return true;
}

////////////////////////////////////////////////////////////////
//			START OF DEBUGGING SECTION
//

// EXIT kody poriesit

// IFJ19 -> _
// IFJcode19 -> _, -, $, &, %, *, !, ?

bool main() {

	lexem_string_init(&IFJcode19); // Init output dynamic string
	Gen_Start (); // HEAD
//==================================================================


	Gen_push_arg("string@HAHAHA");
	Gen_push_arg("int@1");
	Gen_function_call("ord");
	Gen_print("TF@%retval");
	Gen_print("string@\n");






/*									gen_ord
	GEN_STRING("DEFVAR GF@A");
	Gen_ord("GF@A", "string@IHVUIGIUIU", 2); // V -> 86
	GEN_STRING("WRITE GF@A");
*/
//==================================================================
	printf("%s\n", IFJcode19.string); // Print output dynamic variable
}
//			END OF DEBUGGING SECTION
////////////////////////////////////////////////////////////////