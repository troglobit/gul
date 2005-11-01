/*
 * editor.h
 *
 * As made out in the design specification (spec.tex) we decided to use JAkovs
 * standard command definitions for edit/movement/block & notepad marking.
 *
 * These functions should be supplied by any editor-core that is used!
 *
 * 23/4 1999 Crash <joachim.nilsson@mds.mdh.se>
 *
 */

#ifndef EDITOR_H
#define EDITOR_H

#include "config.h"

#if defined (ORIGINAL_MODE)
#include "original.h"
#elif defined (BUFFER_GAP_MODE)
#include "buffer_gap.h"
#endif

typedef struct screenSt{
  int maxX, maxY;  /* This buffersize */
  int x, y;
  int hpos;        /* Horisontal panning position */
  int vpos;        /* Vertical offset */
}screen;

/* XXX - Oh, maybe we can do a neat bitfield here of some, like dirty? */
typedef struct currentSt{
  char *filename;
  int dirty;
  screen scr;
  int pos;
  text *node;
  text *stx;                /* ... Start of TeXt and list.             */
  text *top;                /* First "line" to be displayed.           */
}mark_t;


char *editorCharacterGenerator(mark_t *currentp);

void left(mark_t *currentp);
void right(mark_t *currentp);
void up(mark_t *currentp);
void down(mark_t *currentp);
void end_of_line(mark_t *currentp);
void beginning_of_line(mark_t *currentp);
void page_up (mark_t *currentp);
void page_down (mark_t *currentp);
void insert(mark_t *currentp, int thisCommand);
void delete(mark_t *currentp);


#endif /* EDITOR_H */
