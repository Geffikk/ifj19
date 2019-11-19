//
// Created by maros on 11/18/2019.
//

#ifndef IFJ_READER_H
#define IFJ_READER_H

#define  IO_ERROR 0
#define IO_SUCCESS 1

int read_char(void);
int open_source_file(char *filename);
void close_source_file(void);

#endif //IFJ_READER_H
