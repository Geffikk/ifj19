
/************************ MAIN *************************
 * @author : Maros Geffert <xgeffe00@stud.fit.vutbr.cz>
***********************************************************/
#include <stdio.h>
#include "parser.h"

int main()
{
    FILE* source_file;


    source_file = fopen("/home/patres/CLionProjects/IFJ_2019/test.txt", "r");
    if (source_file == NULL){
        return -1;
    }



    //source_file = stdin;
    int result;

    set_source_file(source_file);

    result = analyza();

    return result;
}
