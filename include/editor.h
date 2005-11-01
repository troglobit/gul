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

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "config.h"

#include <stdio.h>

#if defined (DLL_MODE)
#include "dll.h"
#elif defined (BUFFER_GAP_MODE)
#include "buffer_gap.h"
#endif

typedef struct screenSt{
  int maxX, maxY;               /* Screen dimensions */
  int x, y;                     /* Cursor pos. */

  /* Virtual placement */
  int hpos;                     /* Horisontal panning position */
  int vpos;                     /* Vertical offset */

#if defined (DLL_MODE)          /* First line to be (re)displayed */
  dll_t *top;                   /* Either a char*, or a dll_t* node */
#elif defined (BUFFER_GAP_MODE) /* if buffer_gap or dll, resp. */
  text_t *top;
#endif

  char ruler [RULER_WIDTH];        /* --GUL--**-- <filename> --L0-C0----   */
  char status_field [STATUS_WIDTH]; /* Bottom status/io field */
  /* Add int status_expiry here to remove status messages after timeout */
} screen_t;

/* Definition of our buffer type
 *
 *
 */
/* XXX - Oh, maybe we can do a neat bitfield here of some, like dirty? */
typedef struct currentSt{
  char    *filename;            /* Filename of this buffer */
  int      dirty;               /* Buffer dirty, needs to be saved. */
  int      position;            /* byte position in file/buffer */
  screen_t screen;              /* The screen */

  text_t  core;                 /* Depends on editor core, holds the file  */

  struct currentSt *prev;       /* With >1 buffer we might have prev. here. */
  struct currentSt *next;       /* With >1 buffer we have next buffer here. */
} buffer_t;


char *editorCharacterGenerator(buffer_t *currentp);

void left(buffer_t *currentp);
void right(buffer_t *currentp);
void up(buffer_t *currentp);
void down(buffer_t *currentp);
void end_of_line(buffer_t *currentp);
void beginning_of_line(buffer_t *currentp);
void page_up (buffer_t *currentp);
void page_down (buffer_t *currentp);
void insert(buffer_t *currentp, int thisCommand);
void delete(buffer_t *currentp);

/* Marking, copying and deleting text */
void set_mark  (buffer_t *currentp);
void set_point (buffer_t *currentp);
void cut       (buffer_t *currentp);
void copy      (buffer_t *currentp);
void paste     (buffer_t *currentp);

int newFile (buffer_t *new, size_t size);
int readFile(FILE* filep, buffer_t *new);
int saveFile(buffer_t *currentp, char *filename);
void coreNewScreen (buffer_t *newBuffer);
void freeBuffer (buffer_t *buf);

#endif /* __EDITOR_H__ */


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
