/* main.c - This is where it all begins ...
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
 * The one and only, here it is, would you please welcome ... GUL!
 *
 *** Authors ************************************************************
 * General:
 * Joachim "Crash Override" Nilsson <joachim.nilsson@mds.mdh.se>
 *
 * Movement & semantics:
 * Jakob "JAkov af Wallby" Eriksson <jakob.eriksson@quicknet.se>
 *
 ************************************************************************
 * 8 July 1999  -  Update. The keyboard plugin must return a defined
 *                 meaining ENTER. Right now it accepts only ASCII 13.
 *                 My terminal sent ASCII 10. It did not work.
 *                 Hence I define it now:
 *                 A keyboard plugin _must_ send ASCII 13 when meaning ENTER.
 * 26th of September 1999 - OK in SLang plugin. /Crash
 */

#include "config.h"

#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "editor.h"
#include "fileio.h"
#include "keyboard.h"
#include "memory.h"
#include "misc.h"
#include "plugin.h"

/* Liten fånig text för att retas med den som läser i binären... :) */
char teaser[] = "Nyfiken gul? (Swedish for: \"Curious yellow?\" ;-)";

extern char *__progname;


static void ignore(int sig)
{
	LOG("Ignored signal %d\n", sig);
}

static void finish(int sig)
{
	plugin_exit();
	exit(0);
}

static int usage(int status)
{
	printf("Usage: %s [OPTIONS] [FILE]\n", __progname);
	printf("Options:\n"
               "      --verbose     Verbose logging\n"
               "  -h, --help        This help text\n"
               "  -v, --version     Show version information\n"
               "=============================================\n");
	printf("%s - The one true GUL editor!\n", __progname);

	return status;
}

int main(int argc, char *argv[])
{
	int c, quit = 0;
	buffer_t *current;
	char *filename = NULL;
	keyevent_t ev;
        struct option long_options[] = {
           {"verbose", no_argument, 0, 'V'},
           {"help",    no_argument, 0, 'h'},
           {"version", no_argument, 0, 'v'},
           {NULL, 0, NULL, 0}
        };

	while ((c = getopt_long(argc, argv, "vhv", long_options, (int *)0)) != EOF) {
		switch (c) {
		case 'v':
			printf("gul %s\n", VERSION);
			return 0;

		case 'h':
			return usage(0);

		default:
			return usage(EXIT_FAILURE);
		}
	}

	/* Take care of Ctrl-C */
	signal(SIGINT, ignore);
	signal(SIGTERM, finish);

	if (argv[optind])
		filename = strDup(argv[optind]);

	current = file_open(filename);

	/* Main screen turn on ... */
	screen_create();

	/* Debugging sent to /dev/null or stderr ... */
#if DEBUG == 0
	redirect(stderr, "/dev/null");
#endif

	LOG("main() - display(current);\n");
	display(current);

	/* Main editor loop. */
	while (!quit) {
		quit = keyboard_loop(current);
		display(current);
	}

	/* Try saving if dirty */
	if (current->dirty) {
		display_status(current, "Discard changes (Y/n)?");
		do {
			ev = keyboard_event();
		} while (GUL_NO_EVENT == ev.event);

		if ('Y' != ev.keydata && 'y' != ev.keydata)
			try_save(current);
	}

	/* XXX - free whole chain of active buffers as well... */
	buffer_free(current);
	plugin_exit();

	return 0;
}

void try_save(buffer_t *buf)
{
	if (!buf->filename) {
		int y;
		y = display_status(buf, "Save As: ");
		buf->filename = keyboard_gets(9, y);
	}

	file_save(buf, buf->filename);
}

buffer_t *try_load(buffer_t *buf)
{
	int y;
	char *filename;

	y = display_status(buf, "Load file: ");
	filename = keyboard_gets(11, y);

	buffer_free(buf);
	return file_open(filename);
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
