/* keyboard.c - Minimal set of key bindings.
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
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "display.h"
#include "editor.h"
#include "gul.h"
#include "help.h"
#include "plugin.h"


void do_search(buffer_t *currentp);

/**
 * keyboard_loop - Read keyboard input and act upon it.
 *
 * This function is responsible for its keybinding layout
 * and to act accordingly upon input.  As such it is the
 * second most important function after the main loop.
 *
 * To quit the editor this function returns non-zero value.
 */
int keyboard_loop(buffer_t *currentBuffer)
{
	int quit = 0;		/* Quit the application. */
	int closure = 1;	/* Keybinding closure */
	keyevent_t thisEvent;

	do {
		/* Maybe add some neat stuff to not monopolize the CPU
		 * with vt100 - or perhaps leave this to the plugin to
		 * deal with?
		 */
		thisEvent = keyboard_event();

		switch (thisEvent.event) {
		case GUL_DATA: /* Regular textual data to go into the file/buffer. */
			if (thisEvent.keydata) {
				LOG("[%c]\n", (char)thisEvent.keydata);
				insert(currentBuffer, thisEvent.keydata);
			}
			break;

		case GUL_C_SPACE: /* Set mark! */
			display_status(currentBuffer, "Mark set.");
			LOG("Mark set\n");
			set_mark(currentBuffer);
			break;

		case GUL_C_w: /* copy */
			display_status(currentBuffer, "Cut text");
			LOG("Cut text\n");
			cut(currentBuffer);
			break;

		case GUL_C_y:
			display_status(currentBuffer, "Pasting.");
			LOG("Pasting from internal edit buffer\n");
			paste(currentBuffer);
			break;

		case GUL_C_g:	/* goto_line */
			{
				int y;
				int line;
				char *tmp;

				y = display_status(currentBuffer, "Goto line: ");
				tmp = keyboard_gets(11, y);
				sscanf(tmp, "%d", &line);
				free(tmp);
				goto_line(currentBuffer, line);
				break;
			}

		case GUL_C_f:	/* i-search */
			do_search(currentBuffer);
			break;

		case GUL_ENTER:
			insert(currentBuffer, 10);
			break;

		case GUL_LEFT:
			left(currentBuffer);
			break;

		case GUL_RIGHT:
			right(currentBuffer);
			break;

		case GUL_UP:
			up(currentBuffer);
			break;

		case GUL_DOWN:
			down(currentBuffer);
			break;

		case GUL_C_h:
		case GUL_BACKSPACE:
			LOG("[BS]\n");
			left(currentBuffer);
			delete(currentBuffer);
			break;

		case GUL_DELETE:
			LOG("[DEL]\n");
			delete(currentBuffer);
			break;

		case GUL_F1:
			LOG("[F1]\n");
			display(&help);
			keyboard_event();
			break;

		case GUL_F2:
			LOG("[F2]\n");
			try_save(currentBuffer);
			break;

		case GUL_F3:
			LOG("[F3]\n");
			currentBuffer = try_load(currentBuffer);
			break;

		case GUL_F4:
			LOG("[F4]\n");
			break;

		case GUL_F5:
			LOG("[F5]\n");
			break;

		case GUL_F6:
			LOG("[F6]\n");
			break;

		case GUL_F7:
			LOG("[F7]\n");
			do_search(currentBuffer);
			break;

		case GUL_F8:
			LOG("[F8]\n");
			break;

		case GUL_F9:
			LOG("[F9]\n");
			break;

		case GUL_F10:
			LOG("[F10]\n");
			quit = 1;
			break;

		case GUL_F11:
			LOG("[F11]\n");
			break;

		case GUL_F12:
			LOG("[F12]\n");
			break;

		case GUL_HOME:
			LOG("[HOME]\n");
			beginning_of_line(currentBuffer);
			break;

		case GUL_END:
			LOG("[END]\n");
			end_of_line(currentBuffer);
			break;

		case GUL_PPAGE:
			LOG("[PGUP]\n");
			page_up(currentBuffer);
			break;

		case GUL_NPAGE:
			LOG("[PGDN]\n");
			page_down(currentBuffer);
			break;

		case GUL_INSERT:
			LOG("[INS]\n");
			break;

		case GUL_C_l:
			LOG("Refresh\n");
			/* XXX - Re-center on screen and then let display() */
			break;

		default:
			LOG("Not a GUL_NO_EVENT ... default PANIC! :/ \n");
			break;
		}
	}
	while (!closure);

	/* Tell main loop to quit/continue. */
	return quit;
}


void do_search(buffer_t *currentp)
{
	int y;
	int line;
	char *tmp;

	y = display_status(currentp, "i-search: ");
	tmp = keyboard_gets(10, y);
	search(currentp, tmp, 0);
	free(tmp);
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
