/* io.c - I/O functions.
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
 * 26/9 1999 This does not look that good right now. It's a mix of all
 *           the different modes' load/save functions. Very messy indeed.
 *           /Crash
 ************************************************************************
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "config.h"
#include "editor.h"
#include "fileio.h"
#include "display.h"
#include "memory.h"
#include "misc.h"

/*** Global variables **************************************************/
extern int errno;


/*** file_open() *********************************************************
 * Tries to load the given file.
 *
 *** Error **************************************************************
 * When something goes wrong this function returns the "empty file"...
 *
 ************************************************************************
 */
buffer_t *file_open(char *filename)
{
	buffer_t *buf;
	char tmp[256];
	FILE *fp = NULL;
	struct stat statusBuffer;

	buf = calloc(sizeof(buffer_t), sizeof(char));
	if (!buf) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	/* If there was a filename given on the commandline/prompt then save
	   it for future use, even if the file does not exist now.
	 */
	if (filename) {
		/* Get filespecific information, size, nodes etc.
		 * AND ... does the file exist??
		 */
		if (-1 == stat(filename, &statusBuffer)) {
			sprintf(tmp, "%s\nCould not open the file %s\n", strerror(errno), filename);
			popUp_OK(tmp);
		} else {
			fp = fopen(filename, "r");
			if (!fp) {
				/* Couldn't open the file anyway ... even though stat
				 * returned ok.  Start up with an empty file.
				 */
				sprintf(tmp, "Unknown error!\nCould not open the file %s\n", filename);
				popUp_OK(tmp);
			}
		}

		/* Extract the filename itself, no pathinfo.
		 *  o Find the last slash ...
		 *    => set 'CurrentFile' to point at the filename.
		 *    No slash found
		 *    => No pathinfo specified, equivalence.
		 */
		/*
		   if (NULL != (pos= strrchr(filename, '/'))){
		   *pos= '\0';
		   buf->filename= strDup(pos + 1);
		   }else{
		   buf->filename= filename;
		   }
		 */
		buf->filename = filename;
	}

	/* If the file could not be opened, return the empty buffer. */
	if (!fp)
		file_new(buf, 0);
	else
		file_read(buf, fp);

	screen_new(buf);

	return buf;
}


/*** redirect() *********************************************************
 * Tries to redirect a file to another.
 ************************************************************************
 */
void redirect(FILE *file_in, char *file_out)
{
	int fd;

	fd = open(file_out, O_WRONLY);
	if (-1 == fd) {
		LOG("Couldn't redirect %d to %s... \n", fileno(file_in), file_out);
	} else {
		dup2(fd, fileno(file_in));
	}
}


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
