#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>
#include "editor.h"

buffer_t *loadFile(char *fileName);
int saveFile(buffer_t * currentp, char *filename);
void redirect(FILE *file_in, char *file_out);


#endif				/* __IO_H__ */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
