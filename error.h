//
// Created by maros on 11/18/2019.
//

#ifndef IFJ_ERROR_H
#define IFJ_ERROR_H

#define token_scan_accepted 0
#define token_syntax_accepted 0
#define error_lexical 1
#define error_syntax 2
#define error_semantic 3
#define error_semantic_compatibility 4
#define error_semantic_bad_count_param 5
#define error_semantic_others 6
#define error_div_zero 9
#define error_internal 99

#define MAX_SERR 3
#define SERR_INIT 1
#define SERR_PUSH 2
#define SERR_TOP 3

#endif //IFJ_ERROR_H
