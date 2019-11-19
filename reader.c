//
// Created by maros on 11/18/2019.
//

#include <stdio.h>
#include "reader.h"

FILE *source_file;
int lineNo, colNo;
int currentChar;

int read_char(void)
{
    currentChar = getc(source_file);
    colNo++;
    if(currentChar == '\n'){
        lineNo ++;
        colNo = 0;
    }

    return currentChar;
}


