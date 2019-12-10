
/************************ MAIN *************************
 * @author : Patrik Tomov <xtomov02@stud.fit.vutbr.cz>
***********************************************************/
#include <stdio.h>
#include "parser.h"




int main()
{
    int result;
    FILE* source_file;
    source_file = stdin;


    set_source_file(source_file);

    result = analyza();

    return result;
}
