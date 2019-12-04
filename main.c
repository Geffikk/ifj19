
/************************ MAIN *************************
 * @author : Patrik Tomov <xtomov02@stud.fit.vutbr.cz>
***********************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "analyza.h"
#include "code_generator.h"



int main()
{
    FILE* source_file;
    source_file = fopen("/home/patres/CLionProjects/IFJ_2019/test1.txt", "r");
    if (source_file == NULL){
        return -1;
    }

    int result;

    set_source_file(source_file);

    result = analyza();

    return result;
}