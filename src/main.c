/* gul.c                                         vim:set ts=3 sw=3 ruler:
 * This is where it all begins ...
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
	pluginFinish();
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
	buffer_t *currentBuffer;
	char *fileName = NULL;
	keyevent_t thisEvent;
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
		fileName = strDup(argv[optind]);

	currentBuffer = loadFile(fileName);

	/* Main screen turn on ... */
	createScreen();

	/* Debugging sent to /dev/null or stderr ... */
#if DEBUG == 0
	redirect(stderr, "/dev/null");
#endif

	LOG("main() - display(currentBuffer);\n");
	display(currentBuffer);

	/* Main editor loop. */
	while (!quit) {
		quit = keyboard_loop(currentBuffer);
		display(currentBuffer);
	}

	/* Try saving if dirty */
	if (currentBuffer->dirty) {
		display_status(currentBuffer, "Discard changes (Y/n)?");
		do {
			thisEvent = read_key();
		} while (GUL_NO_EVENT == thisEvent.event);

		if ('Y' != thisEvent.keydata && 'y' != thisEvent.keydata)
			trySave(currentBuffer);
	}

	/* XXX - free whole chain of active buffers as well... */
	freeBuffer(currentBuffer);
	pluginFinish();

	return 0;
}

void trySave(buffer_t *buf)
{
	if (!buf->filename) {
		int y;
		y = display_status(buf, "Save As: ");
		buf->filename = read_string(9, y);
	}

	saveFile(buf, buf->filename);
}

buffer_t *tryLoad(buffer_t *buf)
{
	int y;
	char *fileName;

	y = display_status(buf, "Load file: ");
	fileName = read_string(11, y);

	freeBuffer(buf);
	return loadFile(fileName);
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
