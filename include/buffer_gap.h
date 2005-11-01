#include "config.h"

#ifndef BUFFER_GAP_H
#define BUFFER_GAP_H

#define GAP_SIZE 1024

/* @buffer holds the file data
 * @gap is an empty void (cursor) that can be moved inside @buffer
 * @buffer_size == file size
 * @gap_size == how much more can be fed to @buffer before realloc.
 */
typedef struct _text{
  char *buffer, *gap;
  unsigned int buffer_size, gap_size;
}text;

/*
int calculateXposition(text *thisp, int i);
text *splitNode(mark_t *currentp);
text *deleteRightNode(mark_t *currentp);
void insertCommand(mark_t *currentp, int command);
mark_t *backupBufferInfo(mark_t *currentp);
void restoreBufferInfo(mark_t *backup_info, mark_t *currentp);
int getNextNode(mark_t *currentp);
int getPrevNode(mark_t *currentp);
int movetoNextChar(mark_t *currentp);
int movetoPrevChar(mark_t *currentp);
int atSTX(mark_t *currentp);
char getCurrentChar(mark_t *currentp);
int getCurrentColumn(mark_t *currentp);
int movetoEOL(mark_t *currentp);
int movetoBOL(mark_t *currentp);
int movetoSTX(mark_t *currentp);
int movetoCol(mark_t *currentp, int col);
*/

#endif /* BUFFER_GAP_H */
