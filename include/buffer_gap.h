#include "config.h"

#ifndef __BUFFER_GAP_H__
#define __BUFFER_GAP_H__

#define GAP_SIZE 1024

/* @buffer holds the file data
 * @gap is an empty void (cursor) that can be moved inside @buffer
 * @point end pointer when marking
 * @mark start pointer when marking, set with C-@ or C-<space>
 * @buffer_size == file size
 * @gap_size == how much more can be fed to @buffer before realloc.
 */
typedef struct _text{
  char        *buffer;
  char        *gap;
  char        *point, *mark;
  unsigned int position;   /* byte position in file/buffer */
  unsigned int buffer_size, gap_size;
} text_t;

#endif /* __BUFFER_GAP_H__ */
