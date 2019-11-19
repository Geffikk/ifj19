//
// Created by maros on 11/18/2019.
//

#ifndef IFJ_ERROR_H
#define IFJ_ERROR_H

typedef enum
{
    ERR_ENDOFCOMMENT,
    ERR_IDENTTOOLONG,
    ERR_INVALIDCHARCONSTANT,
    ERR_INVALIDSYMBOL,
    ERR_NUMBERTOOLONG,
} ErrorCode;

#define ERM_ENDOFCOMMENT "End of comment expected"
#define ERM_INDENTTOOLONG "Identification too long"
#define ERM_INVALIDCHARCONSTANT "Invalid const char"
#define ERM_INVALIDSYMBOL "Invalid symbol"
#define ERM_NUMBERTOOLONG "Number too long"

void error(ErrorCode err, int lineNo, int colNo);

#endif //IFJ_ERROR_H
