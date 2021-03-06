/* ncurses.c - NCurses-plugin for the Gul Editor.
 *
 * Copyright (c) 2004-2015  Joachim Nilsson <troglobit@gmail.com>
 * Copyright (c) 1998-2000  Joachim Nilsson, Jakob Eriksson, Anders Born�s
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
 * This can be seen as rather simple "tutorial" on how to write other
 * plugins such as SLang & vt100 and others...
 *
 * 23/4 1999 - Crash <joachim.nilsson@mds.mdh.se>
 *
 */

#include "config.h"
#ifdef NCURSES_PLUGIN

#include <ncurses.h>
#include <stdlib.h>
#include "plugin.h"

/* Div. inst�llningar och s�nt... */
void screen_plugin_init(void)
{
	initscr();
	nonl();
	scrollok(stdscr, FALSE);

//   attrset(WA_NORMAL);
//   attroff(A_REVERSE);
//   attroff(A_UNDERLINE);
//   attroff(A_BLINK);
}

/* Call this when quitting from editor. */
void screen_plugin_exit(void)
{
	endwin();
}

/* Returnera sk�rmens storlek i just det �gonblicket. */
void screen_plugin_get_dim(int *max_row, int *max_col)
{
	/* Sjuka getmaxyx �r et MACRO! sjukt sjukt sjukt sjukt sjukt sjukt sjukt sjukt sjukt sjukt !!! */
	getmaxyx(stdscr, (*max_row), (*max_col));
}


/* Positions the cursor */
void screen_plugin_set_cursor(int x, int y)
{
	move(y, x);
}


/* L�ser ur framebuffern/virtuella sk�rmen och skriver ut p� sk�rmen */
void screen_plugin_update(void)
{
	int virtualXmax, virtualYmax;
	int stdscrXmax, stdscrYmax;
	int x, y;
	char ch;

	LOG("screen_plugin_update()\n");

	/* Bara f�r "s�kerhets" skull s� kollar jag att f�nstret inte resize:ats. */
	screen_get_dim(&virtualYmax, &virtualXmax);
//   getmaxyx(stdscr, stdscrYmax, stdscrXmax);
//   LOG("screen_plugin_update() - virtualXmax= %d, virtualYmax= %d\n", virtualXmax, virtualYmax);

	/* Kanske �r det on�digt, men vadd� ... :P */
//   for (y= 0; y < (virtualYmax > stdscrYmax ? stdscrYmax : virtualYmax); y++){
//      for (x= 0; y < (virtualXmax >  stdscrXmax ? stdscrXmax : virtualXmax); x++){
	move(0, 0);
	for (y = 0; y < virtualYmax; y++) {
		for (x = 0; x < virtualXmax; x++) {
			ch = screen_get_pixchar(x, y);

			if (0 == ch)
				addch(' ');
			else
				addch(ch);
		}
	}

	touchwin(stdscr);
	refresh();
}



/* Initierar tangentbordspluginet */
void keyboard_plugin_init(void)
{
	keypad(stdscr, TRUE);	/* Enable keypad/movement-keys */
	meta(stdscr, TRUE);	/* eight bit input */
	noecho();		/* No echo to screen. */
//   raw();                /* Unformatted input */
	cbreak();		/* Somewhat formatted, break/resize/others catched */
	//   nodelay(stdscr, TRUE);/* Non-blocking input! */
	nodelay(stdscr, FALSE);	/* Blocking input! */
}

/*
 * keyboard_plugin_exit ()
 * Reset all special keyboard handling back to that previous.
 */
void keyboard_plugin_exit(void)
{
	/* XXX - Need to clear up this because otherwise C-z and friends stop
	 * working after exiting from GUL
	 */
}

char *plugin_read_str(int x, int y)
{
	char *str;


	str = calloc(80, sizeof(char));
	if (!str) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	echo();
	mvgetnstr(y, x, str, 80);
	noecho();

	return str;
}

/**
 * plugin_read_key - Check for a key command.
 *
 * Checks for a key in the inbuffer, if
 * mo key available at that time returns
 * %GUL_NO_EVENT.
 *
 * Non-blocking call.
 */

keyevent_t plugin_read_key(void)
{
	int ch = getch();
	keyevent_t input = { GUL_NO_EVENT, GUL_NO_DATA };

	ch = (ch == ERR ? GUL_NO_EVENT : ch);

	/* All control keys, except BS, HT, LF, CR */
	if ((ch >= KEY_CODE_YES || ch < ' ')
	    && (ch != 8 && ch != 9 && ch != 10 && ch != 13)) {
		input.event = ch;
		input.keydata = GUL_NO_DATA;
	} else {
		input.event = GUL_DATA;
		input.keydata = (13 == ch ? 10 : ch);
	}

	return input;
}




/* Ganska viktig ty g�rs inte den ena eller den andra s� kajkar det ur
 * r�tt fint ...
 */
void plugin_init(void)
{
	screen_plugin_init();
	keyboard_plugin_init();
}

/**
 * plugin_exit:
 *
 * Releases all memory and resets the screen and keyboard back to normal.
 */
void plugin_exit(void)
{
	keyboard_plugin_exit();
	screen_plugin_exit();
}

#endif				/* NCURSES_PLUGIN */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
