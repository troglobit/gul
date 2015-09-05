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

#ifndef EDITOR_H_
#define EDITOR_H_

#include "config.h"

#include <stdio.h>

#if defined (DLL_MODE)
#include "dll.h"
#elif defined (BUFFER_GAP_MODE)
#include "buffer_gap.h"
#endif

typedef struct screenSt {
	int max_col, max_row;	/* Screen dimensions */
	int x, y;		/* Cursor pos. */

	/* Virtual placement */
	int hpos;		/* Horisontal panning position */
	int vpos;		/* Vertical offset */

#if defined (DLL_MODE)		/* First line to be (re)displayed */
	dll_t *top;		/* Either a char*, or a dll_t* node */
#elif defined (BUFFER_GAP_MODE)	/* if buffer_gap or dll, resp. */
	text_t *top;
#endif

	char ruler[RULER_WIDTH];	/* --GUL--**-- <filename> --L0-C0----   */
	char status_field[STATUS_WIDTH];	/* Bottom status/io field */
	/* Add int status_expiry here to remove status messages after timeout */
} screen_t;

/* Definition of our buffer type
 *
 *
 */
/* XXX - Oh, maybe we can do a neat bitfield here of some, like dirty? */
typedef struct currentSt {
	char *filename;		/* Filename of this buffer */
	char dirty;		/* Buffer dirty, needs to be saved. */
	screen_t screen;	/* The screen */

	text_t core;		/* Depends on editor core, holds the file  */

	struct currentSt *prev;	/* With >1 buffer we might have prev. here. */
	struct currentSt *next;	/* With >1 buffer we have next buffer here. */
} buffer_t;


char *editor_chargen  (buffer_t *buf);

void left             (buffer_t *buf);
void right            (buffer_t *buf);
void up               (buffer_t *buf);
void down             (buffer_t *buf);
void end_of_line      (buffer_t *buf);
void beginning_of_line(buffer_t *buf);
void page_up          (buffer_t *buf);
void page_down        (buffer_t *buf);
void insert           (buffer_t *buf, int ch);
void delete           (buffer_t *buf);

void goto_line        (buffer_t *buf, int line);

/* Marking, copying and deleting text */
void set_mark         (buffer_t *buf);
void set_point        (buffer_t *buf);
void cut              (buffer_t *buf);
void copy             (buffer_t *buf);
void paste            (buffer_t *buf);

void search           (buffer_t *buf, char *pattern, int dir);

int  file_new         (buffer_t *buf, size_t len);
int  file_read        (buffer_t *buf, FILE *fp);
int  file_save        (buffer_t *buf, char *filename);
void screen_new       (buffer_t *buf);
void buffer_free      (buffer_t *buf);

#endif /* EDITOR_H_ */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
