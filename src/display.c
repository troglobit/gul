/* display.c - Display engine
 *
 * Copyright (c) 2004-2015  Joachim Nilsson <troglobit@gmail.com>
 * Copyright (c) 1998-2000  Joachim Nilsson, Jakob Eriksson, Anders Bornäs
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *** Description ********************************************************
 * Borde visa lite mer än bara text & markör. Tex linjal och annat blaj
 ************************************************************************
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "plugin.h"
#include "display.h"
#include "memory.h"

static void display_ruler(buffer_t *currentp);
static void display_toolbar(buffer_t *currentp);
static void display_cursor(buffer_t *currentp);
static void display_text(buffer_t *currentp);


void display(buffer_t *currentp)
{
	screen_get_dim(&(currentp->screen.max_row), &(currentp->screen.max_col));
/* Reservera sista raden som statusrad... */
	currentp->screen.max_row -= 3;	/* Kolla först om noll!! */
	currentp->screen.max_col -= 0;

	display_text(currentp);
	display_ruler(currentp);
	display_toolbar(currentp);

	/* Kanske göra om det här lite granna?
	 * Förslag:
	 * screenRefresh (currentp);
	 * Vet om var markören är så den kan ritas dit sist av allt!
	 */
	/* Nuvarande kladdiga röra */
	screen_update();
	display_cursor(currentp);
}


static void display_cursor(buffer_t *currentp)
{
	screen_set_cursor(currentp->screen.x - currentp->screen.hpos, currentp->screen.y - currentp->screen.vpos);

/*  screen_set_cursor(currentp->screen.x, currentp->screen.y);
 */
}

static void display_text(buffer_t *currentp)
{
	int x, y, xmax, ymax;
	int i = 0;
	char ch = ' ';
	char *genbuf;

	genbuf = editor_chargen(currentp);

//   LOG("currentp->screen.max_row= %d, currentp->screen.max_col= %d\n", currentp->screen.max_row, currentp->screen.max_col);
//   LOG("%s\displayText() - nSlut på genbuf.\n", genbuf);

	ymax = currentp->screen.max_row;
	xmax = currentp->screen.max_col + currentp->screen.hpos;

	for (y = 0; y < ymax; y++) {
		for (x = 0; x < xmax; x++) {
			ch = genbuf[i];
			if (0 != ch)
				i++;

/* EOL? Fill out with zeros. */
			if ('\n' == ch) {
				while (x < xmax)
					screen_put_pixchar(x++, y, 0);
			} else {
				screen_put_pixchar(x, y, ch);
			}
		}
// HAr vi redan detekterad newline? Eller finns det "skräp" kvar till nyrad?
		if ('\n' != ch)
			while (0 != ch && '\n' != (ch = genbuf[i++])) ;
	}

	free(genbuf);
/*
  for (y= 0; y < currentp->screen.max_row; y++)
  {
  for (x= 0; x < currentp->screen.max_col; x++)
  {
  LOG("%c", screen_get_pixchar(x, y));
  }
  LOG("\n");
  }
*/
#if DEBUG == 1
	screen_debug();
#endif
}


void popUp_OK(char *message)
{
	LOG("%s", message);
}

static char *noname = "<noname>";

/* OBS måste ta hänsyn till max_col && klara resize! */
static void display_ruler(buffer_t *currentp)
{
	int i;
	char *ruler, *filename = currentp->filename;

	/* Rita direkt i framebuffern/virtuella skärmen ?! :) */

	ruler = calloc(currentp->screen.max_col, sizeof(char));
	if (!ruler) {
		perror("Failed allocating ruler");
		exit(1);
	}

	/* Trött, skiter i säkerhetsbältet :P */
	if (!filename)
		filename = noname;

	sprintf(ruler, "-GUL--%s-- %s --L%d-C%d-O%d",
		(currentp->dirty ? "**" : "--"), filename, currentp->screen.y, currentp->screen.x, currentp->core.position);

	for (i = strlen(ruler); i < currentp->screen.max_col; i++)
		ruler[i] = '-';

	for (i = 0; i < currentp->screen.max_col; i++)
		screen_put_pixchar(i, currentp->screen.max_row + 1, ruler[i]);

	free(ruler);
}


/* 1/Help 2/Save 3/Open 4/Unused 5/FindPrev 6/FindNext 7/Find 8/Unused 9/Menu 0/Quit */
static void display_toolbar(buffer_t *currentp)
{
	int x, i, empty, len, extra;
	char toolbar[] = "1/Help 2/Save 3/Open 4/Unused 5/Unused 6/Unused 7/Find 8/Unused 9/Unused 0/Quit";

	len = strlen(toolbar);
	empty = 0;
	for (i = 0; i < len; i++) {
		if (' ' == toolbar[i])
			empty++;
	}
	extra = (currentp->screen.max_col - len) / empty;

	for (x = 0, i = 0; x < currentp->screen.max_col && i < len; i++) {
		if (' ' == toolbar[i]) {
			empty = extra;
			while (empty--)
				screen_put_pixchar(x++, currentp->screen.max_row + 2, ' ');
		}

		screen_put_pixchar(x++, currentp->screen.max_row + 2, toolbar[i]);
	}
}



int display_status(buffer_t *currentp, char *message)
{
	int i;
	int status_y = currentp->screen.max_row + 2;

	memset(currentp->screen.status_field, (int)' ', STATUS_WIDTH);
	strncpy(currentp->screen.status_field, message, STATUS_WIDTH);

	for (i = 0; i < currentp->screen.max_col; i++) {
		if (i < STATUS_WIDTH)
			screen_put_pixchar(i, status_y, currentp->screen.status_field[i]);
		else
			screen_put_pixchar(i, status_y, ' ');
	}

	screen_update();
	display_cursor(currentp);

	return status_y;
}


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
