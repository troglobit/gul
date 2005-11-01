#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "editor.h"

mark_t *loadFile(char *fileName);
int saveFile(mark_t *currentp, char *filename);
void redirect(FILE *file_in, char *file_out);


#endif /* IO_H */
