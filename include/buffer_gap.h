#include "config.h"

#ifndef BUFFER_GAP_H_
#define BUFFER_GAP_H_

#define GAP_SIZE 1024

/* @buffer holds the file data
 * @gap is an empty void (cursor) that can be moved inside @buffer
 * @point end pointer when marking
 * @mark start pointer when marking, set with C-@ or C-<space>
 * @buffer_size == file size
 * @gap_size == how much more can be fed to @buffer before realloc.
 */
typedef struct {
	char *buffer;
	char *gap;
	char *point, *mark;
	unsigned int position;	/* byte position in file/buffer */
	unsigned int buffer_size, gap_size;
} text_t;

#endif /* BUFFER_GAP_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
