
/***********************************************************
 * @author : Andrej Pavlovic <xpavlo14@stud.fit.vutbr.cz>
 * Subject : IFJ
 * Project : Compiler implementation imperativ language IFJ
 * @brief : Code generation
***********************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "code_generator.h"
#include "lexem_string.h"


#define GEN_EOL() \
	if ( !add_string_to_lexem_string(&IFJcode19, "\n") ) return false

#define GEN_STRING(code) \
	if ( !add_string_to_lexem_string(&IFJcode19, code) ) return false

#define GEN_CONST_STRING_AND_EOL(code) \
	if ( !add_string_to_lexem_string(&IFJcode19, code "\n") ) return false

#define GEN_INT(code) \
	{char str[1000]; sprintf(str, "%lu", code); GEN_STRING(str);}


bool Gen_Start () {

	GEN_CONST_STRING_AND_EOL(".IFJcode19"); GEN_EOL();

	GEN_CONST_STRING_AND_EOL("DEFVAR GF@%expr_result");
	GEN_CONST_STRING_AND_EOL("DEFVAR GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("DEFVAR GF@%tmp2");
	GEN_CONST_STRING_AND_EOL("DEFVAR GF@%tmp3");
	GEN_CONST_STRING_AND_EOL("DEFVAR GF@%tmp4"); GEN_EOL();

	GEN_CONST_STRING_AND_EOL("JUMP ?_main"); GEN_EOL();


	////////////		BUILD-IN FUNCTIONS

	GEN_CONST_STRING_AND_EOL("######################    build-in functions    ######################");

	// Build-in function print
	GEN_CONST_STRING_AND_EOL("# build-in function print");
	GEN_CONST_STRING_AND_EOL("LABEL print");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@s");
	GEN_CONST_STRING_AND_EOL("POPS LF@s");
	GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp1 LF@s");
	GEN_CONST_STRING_AND_EOL("EQ GF@%tmp1 GF@%tmp1 string@nil");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_print GF@%tmp1 bool@false");
	GEN_CONST_STRING_AND_EOL("WRITE string@None");
	GEN_CONST_STRING_AND_EOL("JUMP ?_end_print");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_print");
	GEN_CONST_STRING_AND_EOL("WRITE LF@s");
	GEN_CONST_STRING_AND_EOL("LABEL ?_end_print");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function inputs
	GEN_CONST_STRING_AND_EOL("# build-in function inputs");
	GEN_CONST_STRING_AND_EOL("LABEL inputs");
	GEN_CONST_STRING_AND_EOL("READ LF@%retval string");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function inputi
	GEN_CONST_STRING_AND_EOL("# build-in function inputi");
	GEN_CONST_STRING_AND_EOL("LABEL inputi");
	GEN_CONST_STRING_AND_EOL("READ LF@%retval int");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function inputf
	GEN_CONST_STRING_AND_EOL("# build-in function inputf");
	GEN_CONST_STRING_AND_EOL("LABEL inputf");
	GEN_CONST_STRING_AND_EOL("READ LF@%retval float");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function len
	GEN_CONST_STRING_AND_EOL("# build-in function len");
	GEN_CONST_STRING_AND_EOL("LABEL len");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@s");
	GEN_CONST_STRING_AND_EOL("POPS LF@s");
	GEN_CONST_STRING_AND_EOL("STRLEN LF@%retval LF@s");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function substr
	GEN_CONST_STRING_AND_EOL("# build-in function substr");
	GEN_CONST_STRING_AND_EOL("LABEL substr");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@n");
	GEN_CONST_STRING_AND_EOL("POPS LF@n");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@i");
	GEN_CONST_STRING_AND_EOL("POPS LF@i");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@s");
	GEN_CONST_STRING_AND_EOL("POPS LF@s");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@bool");
	GEN_CONST_STRING_AND_EOL("GT LF@bool LF@n int@-1");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_substr1 LF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_substr1");
	GEN_CONST_STRING_AND_EOL("PUSHS LF@s");
	GEN_CONST_STRING_AND_EOL("CREATEFRAME");
	GEN_CONST_STRING_AND_EOL("PUSHFRAME");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@%retval");
	GEN_CONST_STRING_AND_EOL("CALL len");
	GEN_CONST_STRING_AND_EOL("POPFRAME");
	GEN_CONST_STRING_AND_EOL("GT LF@bool LF@i int@-1");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_substr2 LF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_substr2");
	GEN_CONST_STRING_AND_EOL("GT LF@bool TF@%retval LF@i");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_substr3 LF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_substr3");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval string@");
	GEN_CONST_STRING_AND_EOL("ADD LF@n LF@n LF@i");
	GEN_CONST_STRING_AND_EOL("LABEL ?_while_start");
	GEN_CONST_STRING_AND_EOL("LT LF@bool LF@i LF@n");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_while_end LF@bool bool@false");
	GEN_CONST_STRING_AND_EOL("LT LF@bool LF@i TF@%retval");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_while_end LF@bool bool@false");
	GEN_CONST_STRING_AND_EOL("GETCHAR LF@bool LF@s LF@i");
	GEN_CONST_STRING_AND_EOL("CONCAT LF@%retval LF@%retval LF@bool");
	GEN_CONST_STRING_AND_EOL("ADD LF@i LF@i int@1");
	GEN_CONST_STRING_AND_EOL("JUMP ?_while_start");
	GEN_CONST_STRING_AND_EOL("LABEL ?_while_end");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function ord
	GEN_CONST_STRING_AND_EOL("# build-in function ord");
	GEN_CONST_STRING_AND_EOL("LABEL ord");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@i");
	GEN_CONST_STRING_AND_EOL("POPS LF@i");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@s");
	GEN_CONST_STRING_AND_EOL("POPS LF@s");
	GEN_CONST_STRING_AND_EOL("PUSHS LF@s");
	GEN_CONST_STRING_AND_EOL("CREATEFRAME");
	GEN_CONST_STRING_AND_EOL("PUSHFRAME");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@%retval");
	GEN_CONST_STRING_AND_EOL("CALL len");
	GEN_CONST_STRING_AND_EOL("POPFRAME");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@bool");
	GEN_CONST_STRING_AND_EOL("GT LF@bool TF@%retval LF@i");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_ord1 LF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_ord1");
	GEN_CONST_STRING_AND_EOL("GT LF@bool LF@i int@-1");
	GEN_CONST_STRING_AND_EOL("JUMPIFEQ ?_continue_ord2 LF@bool bool@true");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_CONST_STRING_AND_EOL("LABEL ?_continue_ord2");
	GEN_CONST_STRING_AND_EOL("GETCHAR LF@%retval LF@s LF@i");
	GEN_CONST_STRING_AND_EOL("STRI2INT LF@%retval LF@%retval int@0");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Build-in function chr
	GEN_CONST_STRING_AND_EOL("# build-in function chr");
	GEN_CONST_STRING_AND_EOL("LABEL chr");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@i");
	GEN_CONST_STRING_AND_EOL("POPS LF@i");
	GEN_CONST_STRING_AND_EOL("INT2CHAR LF@%retval LF@i");
	GEN_CONST_STRING_AND_EOL("RETURN"); GEN_EOL();

	// Main
	GEN_CONST_STRING_AND_EOL("######################    main    ######################");
	GEN_EOL();
	GEN_CONST_STRING_AND_EOL("LABEL ?_main");

	return true;
}

char* Term_adjustment (const char *term, const int type) {

	static char new_term[1000];
	
	switch (type) {
		case INT :
			strcpy(new_term, "int@");
			strcat(new_term, term);
			break;

		case FLOAT : 0; // label can be followed only by statement (declaration != statement)
			char tmp[1000];
			strcpy(new_term, "float@");
			sprintf(tmp, "%a", strtod(term, NULL));
			strcat(new_term, tmp);
			break;

		case NONE :
			strcpy(new_term, "nil@nil");
			break;

		case GLOBAL :
			strcpy(new_term, "GF@");
			strcat(new_term, term);
			break;

		case LOCAL :
			strcpy(new_term, "LF@");
			strcat(new_term, term);
			break;
			
		case STRING :
			strcpy(new_term, "string@");

			char tmp_str[5];
			long int len = strlen(term);

			for ( int i = 0 ; i < len ; i++ ) {

				if ( (term[i] <= 32 && term[i] >= 0) || term[i] == 35 ) {
					sprintf(tmp_str, "\\%03d", (int) term[i]);
					strcat(new_term, tmp_str);
				}

				else if ( term[i] == 92 ) {

					switch ( term[++i] ) {
						case '\"' :
						case '\'' :
							tmp_str[0] = term[i];
							tmp_str[1] = '\0';
							strcat(new_term, tmp_str);
							break;

						case 'n' :
							strcat(new_term, "\\010");
							break;

						case 't' :
							strcat(new_term, "\\009");
							break;

						case '\\' :
							strcat(new_term, "\\092");
							break;

						case 'x' :
							strcat(new_term, "\\");

							tmp_str[0] = term[++i];
							tmp_str[1] = term[++i];
							tmp_str[2] = '\0';

							long int dec = strtol(tmp_str, NULL, 16);

							tmp_str[0] = ((char)(dec/100)+'0');
							tmp_str[1] = ((char)((dec/10)%10)+'0');
							tmp_str[2] = ((char)(dec%10)+'0');
							tmp_str[3] = '\0';

							strcat(new_term, tmp_str);

							break;

						default :
							strcat(new_term, "\\092");
							tmp_str[0] = term[i];
							tmp_str[1] = '\0';
							strcat(new_term, tmp_str);
					}
				}

				else {
					tmp_str[0] = term[i];
					tmp_str[1] = '\0';
					strcat(new_term, tmp_str);
				}
			}
			break;
	}
	
	return new_term;
}

bool Gen_return () {

	GEN_CONST_STRING_AND_EOL("POPS LF@%retval");
	GEN_CONST_STRING_AND_EOL("RETURN");

	return true;
}

bool Gen_cast_stack_op1 () {

	GEN_CONST_STRING_AND_EOL("INT2FLOATS");

	return true;
}

bool Gen_cast_stack_op2 () {

	GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("INT2FLOATS");
	GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");

	return true;
}

bool Gen_push_stack_op (const char *term) {

	GEN_STRING("PUSHS ");
	GEN_STRING(term); GEN_EOL();

	return true;
}

bool Gen_var_def (const char *var_id) {

	GEN_STRING("DEFVAR ");
	GEN_STRING(var_id); GEN_EOL();

	return true;
}

bool Gen_save_expr_or_retval (const char *var_id) {

	GEN_STRING("POPS ");
	GEN_STRING(var_id); GEN_EOL();
	GEN_CONST_STRING_AND_EOL("CLEARS"); // Preventive stack clear

	return true;
}

bool Gen_string_concat () {

	GEN_CONST_STRING_AND_EOL("# string concat");
	GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("POPS GF@%tmp2");
	GEN_CONST_STRING_AND_EOL("CONCAT GF@%tmp2 GF@%tmp2 GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");

	return true;
}

bool Gen_type_control () {

	static unsigned long int i = 0;

	GEN_CONST_STRING_AND_EOL("# type control");
	GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("POPS GF@%tmp2");
	GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp3 GF@%tmp2");
	GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
	GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp2 GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("EQ GF@%tmp1 GF@%tmp2 GF@%tmp3");
	GEN_STRING("JUMPIFEQ ?_type_control_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%tmp1 bool@true");
	GEN_CONST_STRING_AND_EOL("EQ GF@%tmp1 GF@%tmp2 string@float");
	GEN_CONST_STRING_AND_EOL("EQ GF@%tmp4 GF@%tmp3 string@int");
	GEN_STRING("JUMPIFNEQ ?_type_control2_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%tmp1 GF@%tmp4");
	GEN_STRING("JUMPIFEQ ?_type_control2_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%tmp1 bool@false");
	GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
	GEN_CONST_STRING_AND_EOL("INT2FLOATS");
	GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
	GEN_STRING("JUMP ?_type_control_");
	GEN_INT(i); GEN_EOL();
	GEN_STRING("LABEL ?_type_control2_");
	GEN_INT(i); GEN_EOL();
	GEN_CONST_STRING_AND_EOL("EQ GF@%tmp1 GF@%tmp2 string@int");
	GEN_CONST_STRING_AND_EOL("EQ GF@%tmp4 GF@%tmp3 string@float");
	GEN_STRING("JUMPIFNEQ ?_type_control3_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%tmp1 GF@%tmp4");
	GEN_STRING("JUMPIFEQ ?_type_control3_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%tmp1 bool@false");
	GEN_CONST_STRING_AND_EOL("INT2FLOATS");
	GEN_STRING("JUMP ?_type_control_");
	GEN_INT(i); GEN_EOL();
	GEN_STRING("LABEL ?_type_control3_");
	GEN_INT(i); GEN_EOL();
	GEN_CONST_STRING_AND_EOL("EXIT int@4");
	GEN_STRING("LABEL ?_type_control_");
	GEN_INT(i); GEN_EOL();

	i++;

	return true;
}

bool Gen_expr_calc (Rule_enumeration rule, bool in_function) {

	static unsigned long int i = 0;

	if ( in_function && (rule != RULE_PLUS) ) {

		GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
		GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp2 GF@%tmp1");
		GEN_CONST_STRING_AND_EOL("EQ GF@%tmp2 GF@%tmp2 string@string");
		GEN_STRING("JUMPIFEQ ?_is_it_too_tired_to_think_about_name_");
		GEN_INT(i);
		GEN_CONST_STRING_AND_EOL(" GF@%tmp2 bool@false");
		GEN_CONST_STRING_AND_EOL("EXIT int@4");
		GEN_STRING("LABEL ?_is_it_too_tired_to_think_about_name_");
		GEN_INT(i); GEN_EOL();
		GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");

		i++;
	}

	switch (rule) {

		case RULE_EQUAL :
			GEN_CONST_STRING_AND_EOL("EQS");
			break;

		case RULE_NOT_EQUAL :
			GEN_CONST_STRING_AND_EOL("EQS");
			GEN_CONST_STRING_AND_EOL("NOTS");
			break;

		case RULE_LESS_EQUAL :
			GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("POPS GF@%tmp2");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("LTS");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("EQS");
			GEN_CONST_STRING_AND_EOL("ORS");
			break;

		case RULE_MORE_EQUAL :
			GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("POPS GF@%tmp2");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("GTS");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("EQS");
			GEN_CONST_STRING_AND_EOL("ORS");
			break;

		case RULE_LESS :
			GEN_CONST_STRING_AND_EOL("LTS");
			break;

		case RULE_MORE :
			GEN_CONST_STRING_AND_EOL("GTS");
			break;

		case RULE_MINUS :
			GEN_CONST_STRING_AND_EOL("SUBS");
			break;

		case RULE_PLUS :

			if ( in_function ) {
				GEN_CONST_STRING_AND_EOL("# string concat");
				GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp2 GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("EQ GF@%tmp2 GF@%tmp2 string@string");
				GEN_STRING("JUMPIFEQ ?_is_it_string_");
				GEN_INT(i);
				GEN_CONST_STRING_AND_EOL(" GF@%tmp2 bool@false");
				GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("POPS GF@%tmp2");
				GEN_CONST_STRING_AND_EOL("CONCAT GF@%tmp2 GF@%tmp2 GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
				GEN_STRING("JUMP ?_is_it_string2_");
				GEN_INT(i); GEN_EOL();
				GEN_STRING("LABEL ?_is_it_string_");
				GEN_INT(i); GEN_EOL();
				GEN_CONST_STRING_AND_EOL("ADDS");
				GEN_STRING("LABEL ?_is_it_string2_");
				GEN_INT(i); GEN_EOL();

				i++;
			}

			else {
				GEN_CONST_STRING_AND_EOL("ADDS");
			}

			break;

			GEN_CONST_STRING_AND_EOL("SUBS");
			break;

		case RULE_MULTIPLY :
			GEN_CONST_STRING_AND_EOL("MULS");
			break;

		case RULE_DIVIDE :

			if ( in_function ) {
				GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp2 GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("EQ GF@%tmp2 GF@%tmp2 string@int");
				GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
				GEN_STRING("JUMPIFEQ ?_is_it_int_");
				GEN_INT(i);
				GEN_CONST_STRING_AND_EOL(" GF@%tmp2 bool@false");
				GEN_CONST_STRING_AND_EOL("POPS GF@%tmp2");
				GEN_CONST_STRING_AND_EOL("INT2FLOATS");
				GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp2");
				GEN_CONST_STRING_AND_EOL("INT2FLOATS");
				GEN_STRING("LABEL ?_is_it_int_");
				GEN_INT(i); GEN_EOL();
			}

			GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("EQ GF@%tmp2 GF@%tmp1 float@0x0p+0");
			GEN_STRING("JUMPIFEQ ?_is_it_zero_");
			GEN_INT(i);
			GEN_CONST_STRING_AND_EOL(" GF@%tmp2 bool@false");
			GEN_CONST_STRING_AND_EOL("EXIT int@9");
			GEN_STRING("LABEL ?_is_it_zero_");
			GEN_INT(i); GEN_EOL();
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("DIVS");

			i++;

			break;

		case RULE_DIVIDE_INT :

			if ( in_function ) {
				GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("TYPE GF@%tmp2 GF@%tmp1");
				GEN_CONST_STRING_AND_EOL("EQ GF@%tmp2 GF@%tmp2 string@float");
				GEN_STRING("JUMPIFEQ ?_is_it_float_");
				GEN_INT(i);
				GEN_CONST_STRING_AND_EOL(" GF@%tmp2 bool@false");
				GEN_CONST_STRING_AND_EOL("EXIT int@4");
				GEN_STRING("LABEL ?_is_it_float_");
				GEN_INT(i); GEN_EOL();
				GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			}

			GEN_CONST_STRING_AND_EOL("POPS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("EQ GF@%tmp2 GF@%tmp1 int@0");
			GEN_STRING("JUMPIFEQ ?_is_it_zero_");
			GEN_INT(i);
			GEN_CONST_STRING_AND_EOL(" GF@%tmp2 bool@false");
			GEN_CONST_STRING_AND_EOL("EXIT int@9");
			GEN_STRING("LABEL ?_is_it_zero_");
			GEN_INT(i); GEN_EOL();
			GEN_CONST_STRING_AND_EOL("PUSHS GF@%tmp1");
			GEN_CONST_STRING_AND_EOL("IDIVS");

			i++;

			break;

		default :
			break;
	}

	return true;
}

unsigned long int Gen_if_head () {

	static unsigned long int i = 1;

	GEN_CONST_STRING_AND_EOL("# if head");
	GEN_CONST_STRING_AND_EOL("POPS GF@%expr_result");
	GEN_STRING("JUMPIFEQ ?_else_");
	GEN_INT(i);
	GEN_CONST_STRING_AND_EOL(" GF@%expr_result bool@false");

	return i++;
}

unsigned long int Gen_else_head () {

	static unsigned long int i = 1;

	GEN_CONST_STRING_AND_EOL("# else head");
	GEN_STRING("JUMP ?_else_end_");
	GEN_INT(i); GEN_EOL();
	GEN_STRING("LABEL ?_else_");
	GEN_INT(i); GEN_EOL();

	return i++;
}

bool Gen_else_foot () {

	static unsigned long int i = 1;

	GEN_CONST_STRING_AND_EOL("# else foot");
	GEN_STRING("LABEL ?_else_end_");
	GEN_INT(i); GEN_EOL();

	i++;

	return true;
}

bool Gen_while_label () {

	static unsigned long int i = 0;
	
	GEN_CONST_STRING_AND_EOL("# while label");
	GEN_STRING("LABEL ?_while_");
	GEN_INT(i); GEN_EOL();

	i++;

	return true;
}

bool Gen_while_head () {

	static unsigned long int i = 0;
	
	GEN_CONST_STRING_AND_EOL("# while head");
	GEN_CONST_STRING_AND_EOL("POPS GF@%expr_result");
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

bool Gen_pop_arg (const char *term) { // popuje sa o

	GEN_CONST_STRING_AND_EOL("# pop (pass) function's argument");
	GEN_STRING("DEFVAR ");
	GEN_STRING(term); GEN_EOL();

	GEN_STRING("POPS ");
	GEN_STRING(term); GEN_EOL();

	return true;
}

bool Gen_function_def_foot (const char *fun_id) { // arguments are poped in reverse order
	
	GEN_CONST_STRING_AND_EOL("# function foot");
	GEN_CONST_STRING_AND_EOL("MOVE LF@%retval nil@nil");
	GEN_CONST_STRING_AND_EOL("RETURN");
	GEN_STRING("LABEL ");
	GEN_STRING(fun_id);
	GEN_CONST_STRING_AND_EOL("_end");

	return true;
}

bool Gen_push_arg (const char *term) { // arguments are pushed from the beginning

	GEN_CONST_STRING_AND_EOL("# push function's argument");
	GEN_STRING("PUSHS ");
	GEN_STRING(term); GEN_EOL();

	return true;
}

bool Gen_function_call (const char *fun_id) {
	
	GEN_CONST_STRING_AND_EOL("# function call");

	GEN_CONST_STRING_AND_EOL("CREATEFRAME");
	GEN_CONST_STRING_AND_EOL("PUSHFRAME");
	GEN_CONST_STRING_AND_EOL("DEFVAR LF@%retval");

	GEN_STRING("CALL ");
	GEN_STRING(fun_id); GEN_EOL();

	GEN_CONST_STRING_AND_EOL("POPFRAME");
	GEN_CONST_STRING_AND_EOL("PUSHS TF@%retval");

	return true;
}