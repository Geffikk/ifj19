//
// Created by maros on 11/18/2019.
//

#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void error(ErrorCode err, int lineNo, colNo)
{
    switch (err)
    {
        case ERR_ENDOFCOMMENT:
            printf("%d-%d:%s\n", lineNo, colNo, ERM_ENDOFCOMMENT);
            break;

        case ERR_IDENTTOOLONG:
            printf("%d-%d:%s\n", lineNo, colNo, ERM_INDENTTOOLONG);
            break;

        case ERR_INVALIDCHARCONSTANT:
            printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCHARCONSTANT);
            break;

        case ERR_INVALIDSYMBOL:
            printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSYMBOL);
            break;

        case ERR_NUMBERTOOLONG:
            printf("%d-%d:%s\n", lineNo, colNo, ERM_NUMBERTOOLONG);
            break;
    }
    exit(-1);
}
