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
char teaser[] = "Nyfiken gul? (Which is swedish for: \"Curious yellow?\" ;-)";

/* The name the program was run with, stripped of any leading path. */
char *program_name;

/* getopt_long return codes */
enum { DUMMY_CODE = 129
};

/* Option flags and variables */

int want_verbose;		/* --verbose */

static struct option const long_options[] = {
	{"verbose", no_argument, 0, 'v'},
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'V'},
	{NULL, 0, NULL, 0}
};

/*** Prototypes *********************************************************
 */
static void ignore(int sig);

static void finish(int sig);
static void usage(int status);
static int decode_switches(int argc, char **argv);



int main(int argc, char *argv[])
{
	int i, quit = 0;
	buffer_t *currentBuffer;
	char *fileName = NULL;
	keyevent_t thisEvent;

	program_name = argv[0];

	i = decode_switches(argc, argv);

	if (argv[i])
		fileName = strDup(argv[i]);

	currentBuffer = loadFile(fileName);

	/* openDisplay(); */
	createScreen();

	/* Take care of Ctrl-C */
	signal(SIGINT, ignore);

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

	return (0);
}

static void ignore(int sig)
{
	LOG("Ignored signal %d\n", sig);
}


static void finish(int sig)
{
	pluginFinish();

	exit(0);
}



void trySave(buffer_t * buf)
{
	if (!buf->filename) {
		/* #include <unistd.h>
		   getcwd (str, 80);
		 */
		int y;
		y = display_status(buf, "Save As: ");
		buf->filename = read_string(9, y);
	}

	saveFile(buf, buf->filename);
}

buffer_t *tryLoad(buffer_t * buf)
{
	int y;
	char *fileName;

	/* #include <unistd.h>
	   getcwd (str, 80);
	 */
	y = display_status(buf, "Load file: ");
	fileName = read_string(11, y);

	freeBuffer(buf);
	return loadFile(fileName);
}


/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.  */

static int decode_switches(int argc, char **argv)
{
	int c;


	while ((c = getopt_long(argc, argv, "v"	/* verbose */
				"h"	/* help */
				"V",	/* version */
				long_options, (int *)0)) != EOF) {
		switch (c) {
		case 'v':	/* --verbose */
			want_verbose = 1;
			break;
		case 'V':
			printf("gul %s\n", VERSION);
			exit(0);

		case 'h':
			usage(0);

		default:
			usage(EXIT_FAILURE);
		}
	}

	return optind;
}


static void usage(int status)
{
	printf("%s - The one true GUL editor!\n", program_name);
	printf("Usage: %s [OPTION]... [FILE]...\n", program_name);
	printf("\
Options:\n\
  --verbose                  print more information\n\
  -h, --help                 display this help and exit\n\
  -V, --version              output version information and exit\n\
");

	exit(status);
}


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
