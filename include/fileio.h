#ifndef FILEIO_H_
#define FILEIO_H_

#include <stdio.h>
#include "editor.h"

buffer_t *loadFile(char *fileName);
int       saveFile(buffer_t *currentp, char *filename);
void      redirect(FILE *file_in, char *file_out);

#endif /* FILEIO_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
