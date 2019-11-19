//
// Created by maros on 11/18/2019.
//

#include <stdlib.h>
#include <ctype.h>
#include "scanner.h"

Token* make_token(int lineNo, int colNo)
{
    Token *token = (Token*)malloc(sizeof(Token));
    token->lineNo = lineNo;
    token->colNo = colNo;
    return token;
}