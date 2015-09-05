/* display.c - Display engine                    vim:set ts=3 sw=3 ruler:
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

static void display_ruler(buffer_t * currentp);
static void display_toolbar(buffer_t * currentp);
static void display_cursor(buffer_t * currentp);
static void display_text(buffer_t * currentp);


void display(buffer_t * currentp)
{
	getScreenMaxYX(&(currentp->screen.maxY), &(currentp->screen.maxX));
/* Reservera sista raden som statusrad... */
	currentp->screen.maxY -= 3;	/* Kolla först om noll!! */
	currentp->screen.maxX -= 0;

	display_text(currentp);
	display_ruler(currentp);
	display_toolbar(currentp);

	/* Kanske göra om det här lite granna?
	 * Förslag:
	 * screenRefresh (currentp);
	 * Vet om var markören är så den kan ritas dit sist av allt!
	 */
	/* Nuvarande kladdiga röra */
	screenUpdate();
	display_cursor(currentp);
}


static void display_cursor(buffer_t * currentp)
{
	screenPositionCursor(currentp->screen.x - currentp->screen.hpos, currentp->screen.y - currentp->screen.vpos);

/*  screenPositionCursor(currentp->screen.x, currentp->screen.y);
 */
}

static void display_text(buffer_t * currentp)
{
	int x, y, xmax, ymax;
	int i = 0;
	char ch = ' ';
	char *genbuf;

	genbuf = editorCharacterGenerator(currentp);

//   LOG("currentp->screen.maxY= %d, currentp->screen.maxX= %d\n", currentp->screen.maxY, currentp->screen.maxX);
//   LOG("%s\displayText() - nSlut på genbuf.\n", genbuf);

	ymax = currentp->screen.maxY;
	xmax = currentp->screen.maxX + currentp->screen.hpos;

	for (y = 0; y < ymax; y++) {
		for (x = 0; x < xmax; x++) {
			ch = genbuf[i];
			if (0 != ch)
				i++;

/* EOL? Fill out with zeros. */
			if ('\n' == ch) {
				while (x < xmax)
					putPixchar(x++, y, 0);
			} else {
				putPixchar(x, y, ch);
			}
		}
// HAr vi redan detekterad newline? Eller finns det "skräp" kvar till nyrad?
		if ('\n' != ch)
			while (0 != ch && '\n' != (ch = genbuf[i++])) ;
	}

	free(genbuf);
/*
  for (y= 0; y < currentp->screen.maxY; y++)
  {
  for (x= 0; x < currentp->screen.maxX; x++)
  {
  LOG("%c", getPixchar(x, y));
  }
  LOG("\n");
  }
*/
#if DEBUG == 1
	screenDebugDisplay();
#endif
}


void popUp_OK(char *message)
{
	LOG("%s", message);
}

static char *noname = "<noname>";

/* OBS måste ta hänsyn till maxX && klara resize! */
static void display_ruler(buffer_t * currentp)
{
	int i;
	char *ruler, *filename = currentp->filename;

	/* Rita direkt i framebuffern/virtuella skärmen ?! :) */

	ruler = calloc(currentp->screen.maxX, sizeof(char));
	if (!ruler) {
		perror("Failed allocating ruler");
		exit(1);
	}

	/* Trött, skiter i säkerhetsbältet :P */
	if (!filename)
		filename = noname;

	sprintf(ruler, "-GUL--%s-- %s --L%d-C%d-O%d",
		(currentp->dirty ? "**" : "--"), filename, currentp->screen.y, currentp->screen.x, currentp->core.position);

	for (i = strlen(ruler); i < currentp->screen.maxX; i++)
		ruler[i] = '-';

	for (i = 0; i < currentp->screen.maxX; i++)
		putPixchar(i, currentp->screen.maxY + 1, ruler[i]);

	free(ruler);
}


/* 1/Help 2/Save 3/Open 4/Unused 5/FindPrev 6/FindNext 7/Find 8/Unused 9/Menu 0/Quit */
static void display_toolbar(buffer_t * currentp)
{
	int x, i, empty, len, extra;
	char toolbar[] = "1/Help 2/Save 3/Open 4/Unused 5/Unused 6/Unused 7/Find 8/Unused 9/Unused 0/Quit";

	len = strlen(toolbar);
	empty = 0;
	for (i = 0; i < len; i++) {
		if (' ' == toolbar[i])
			empty++;
	}
	extra = (currentp->screen.maxX - len) / empty;

	for (x = 0, i = 0; x < currentp->screen.maxX && i < len; i++) {
		if (' ' == toolbar[i]) {
			empty = extra;
			while (empty--)
				putPixchar(x++, currentp->screen.maxY + 2, ' ');
		}

		putPixchar(x++, currentp->screen.maxY + 2, toolbar[i]);
	}
}



int display_status(buffer_t * currentp, char *message)
{
	int i;
	int status_y = currentp->screen.maxY + 2;

	memset(currentp->screen.status_field, (int)' ', STATUS_WIDTH);
	strncpy(currentp->screen.status_field, message, STATUS_WIDTH);

	for (i = 0; i < currentp->screen.maxX; i++) {
		if (i < STATUS_WIDTH)
			putPixchar(i, status_y, currentp->screen.status_field[i]);
		else
			putPixchar(i, status_y, ' ');
	}

	screenUpdate();
	display_cursor(currentp);

	return status_y;
}


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
