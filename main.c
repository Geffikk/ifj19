
/************************ MAIN *************************
 * @author : Patrik Tomov <xtomov02@stud.fit.vutbr.cz>
***********************************************************/
#include <stdio.h>
#include "parser.h"




int main()
{
    FILE* source_file;
    source_file = stdin;
    int result;

    set_source_file(source_file);

    result = analyza();

    return result;
}