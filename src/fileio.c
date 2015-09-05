/* -*- Mode: C; indent-tabs-mode: t; tab-width: 3; c-basic-offset: 3 -*- vim:set ts=3 sw=3 ruler: */
/*** io.c ***************************************************************
 * I/O - functions.
 *
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


/*** loadFile() *********************************************************
 * Tries to load the given file.
 *
 *** Error **************************************************************
 * When something goes wrong this function returns the "empty file"...
 *
 ************************************************************************
 */
buffer_t *loadFile(char *fileName)
{
	buffer_t *newBuffer;
	char tempString[1024];
	FILE *filePtr = NULL;
	struct stat statusBuffer;

	newBuffer = calloc(sizeof(buffer_t), sizeof(char));
	if (!newBuffer) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	/* If there was a filename given on the commandline/prompt then save
	   it for future use, even if the file does not exist now.
	 */
	if (fileName) {
		/* Get filespecific information, size, nodes etc.
		 * AND ... does the file exist??
		 */
		if (-1 == stat(fileName, &statusBuffer)) {
			sprintf(tempString, "%s\nCould not open the file %s\n", strerror(errno), fileName);
			popUp_OK(tempString);
		} else {
			filePtr = fopen(fileName, "r");
			if (NULL == filePtr) {
				/* Couldn't open the file anyway ... even though stat
				 * returned ok.  Start up with an empty file.
				 */
				sprintf(tempString, "Unknown error!\nCould not open the file %s\n", fileName);
				popUp_OK(tempString);
			}
		}

		/* Extract the filename itself, no pathinfo.
		 *  o Find the last slash ...
		 *    => set 'CurrentFile' to point at the filename.
		 *    No slash found
		 *    => No pathinfo specified, equivalence.
		 */
		/*
		   if (NULL != (pos= strrchr(fileName, '/'))){
		   *pos= '\0';
		   newBuffer->filename= strDup(pos + 1);
		   }else{
		   newBuffer->filename= fileName;
		   }
		 */
		newBuffer->filename = fileName;
	}

	/* If the file could not be opened, return the empty buffer. */
	if (!filePtr) {
		newFile(newBuffer, 0);
	} else {
		readFile(filePtr, newBuffer);
	}

	coreNewScreen(newBuffer);

	return newBuffer;
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
