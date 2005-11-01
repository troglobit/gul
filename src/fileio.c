/* -*- Mode: C; indent-tabs-mode: nil; tab-width: 3; c-basic-offset: 3 -*- vim:set ts=3 sw=3 ruler: */
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

#include "config.h"
#include "editor.h"
#include "fileio.h"
#include "display.h"
#include "memory.h"
#include "misc.h"

/*** Global variables **************************************************/
extern int errno;

/*** Local prototypes **************************************************/
static text* readFile(FILE* filep);


/*** loadFile() *********************************************************
 * Tries to load the given file.
 *
 *** Error **************************************************************
 * When something goes wrong this function returns the "empty file"...
 *
 ************************************************************************
 */
mark_t *loadFile(char *fileName)
{
   mark_t *newBuffer;
   char tempString[1024];
   FILE *filePtr= NULL;
   struct stat statusBuffer;

   /* Get filespecific information, size, nodes etc.
    * AND ... does the file exist??
    */
   if (-1 == stat(fileName, &statusBuffer)){
      sprintf(tempString,
              "%s\nCould not open the file %s\n",
              strerror(errno),
              fileName);
      popUp_OK(tempString);
   }else{
      filePtr= fopen(fileName, "r");
      if (NULL == filePtr){
         /* Couldn't open the file anyway ... even though stat returned ok.
          * Start up with the empty file.
          */
         sprintf(tempString,
                 "Unknown error!\nCould not open the file %s\n",
                 fileName);
         popUp_OK(tempString);
      }
   }

   newBuffer= allocate(sizeof(mark_t), "loadFile()");

   /* If there was a filename given on the commandline/prompt then save
      it for future use, even if the file does not exist now.
   */
   if (fileName){
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
      newBuffer->filename= fileName;
   }

   /* If the file could not be opened, return the empty buffer. */
   if (!filePtr){
      newBuffer->stx= allocate(sizeof(text), "loadFile()");
      newBuffer->stx->buffer = allocate (GAP_SIZE, "loadFile()");
      newBuffer->stx->buffer_size = GAP_SIZE;

      newBuffer->stx->gap = newBuffer->stx->buffer;
      newBuffer->stx->gap_size = GAP_SIZE;
   }
   else{
      newBuffer->stx= readFile(filePtr);
   }

#ifdef BUFFER_GAP_MODE
   /* BUFFER_GAP needs this copy for the virtual screen
    * XXX - Fixme later ... ! /Crash
    */
   newBuffer->top = allocate (sizeof (text), "loadFile()");
   memcpy (newBuffer->top, newBuffer->stx, sizeof (text));
   /* Invisible cursor that indicates top of screen */
   newBuffer->top->gap_size = 0;
#endif

   newBuffer->pos = 0;
   newBuffer->scr.x= 0;
   newBuffer->scr.y= 0;

   return newBuffer;
}

#if ! defined (BUFFER_GAP_MODE)
static text* readFile(FILE* filep)
{
   text *headp= NULL, *currentp= NULL, *previousp= NULL;
   size_t bytesRead;

   do {
      currentp= allocate(sizeof(text), "readFile()");
      //    currentp->line = allocate(MAX_LINE_LENGTH, "readFile()");
      bytesRead= fread(currentp->line, sizeof(char), MAX_LINE_LENGTH, filep);
      currentp->begin= 0;
      currentp->end= bytesRead;

      if (headp == NULL)
         headp= currentp;
      else
         previousp->next= currentp;

      currentp->previous= previousp;
      previousp= currentp;
   }while (MAX_LINE_LENGTH == bytesRead);

   return (headp);
}


/*** saveFile(text *start, char *filename) ******************************
 * Sparar på ett väldigt simpelt sätt hela klabbet i en fil.
 * Börjar spara från start till slutet... borde kanske göras om s.a. den
 * klarar att spara markerad text från valfria pos. och annat jox.
 ************************************************************************
 */
int saveFile(mark_t *currentp, char *filename)
{
   text *thisp;
   size_t bytesWritten;
   FILE *filep;

   if (NULL == (filep= fopen(filename, "w"))){
      LOG("Could not open %s for saving.\n", filename);
      return 0;
   }
   for (thisp= currentp->stx; thisp != NULL; thisp= thisp->next){
      bytesWritten= fwrite(&(thisp->line[thisp->begin]),
                           sizeof(char), thisp->end - thisp->begin, filep);
      if (bytesWritten != (thisp->end - thisp->begin)){
         LOG("Oups, difference between bytes in node and bytes written to %s...\n", filename);
      }
   }

   fclose(filep);

   currentp->dirty = 0;

   return 1;
}
#endif

#if defined (BUFFER_GAP_MODE)

#define GAP_SIZE 1024

/***
 * readFile():
 * @filep: Regular file pointer.
 *
 * Reads the current file into the array used in the buffer-gap scheme by
 * BUFFER_GAP. Very simple, indeed.
 ***/
static text* readFile(FILE* filep)
{
   text  *currentp;
   size_t bytesRead;
   struct stat filestatus;

   if (0 != fstat (fileno (filep), &filestatus))
      {
         LOG("Could not fstat file before opening!\n");
         return NULL;
      }

   currentp= allocate(sizeof(text), "readFile()");
   currentp->buffer= allocate (filestatus.st_size + GAP_SIZE, "readFile()");
   currentp->buffer_size= filestatus.st_size + GAP_SIZE;

   /* Initial size and location of the gap */
   currentp->gap = currentp->buffer;
   currentp->gap_size= GAP_SIZE;

   /* Start reading in the file after the gap. */
   bytesRead= fread (&(currentp->buffer[GAP_SIZE]),
                     filestatus.st_size,
                     1,
                     filep);
   if (bytesRead != filestatus.st_size)
      {
         LOG("readFile() - Read: %d, file size: %d ?\n",
                  bytesRead,
                  (int)filestatus.st_size);
      }

   return (currentp);
}

/***
 * saveFile():
 * @currentp: Ponter to the buffer.
 * @filename: The filename to be used for saving.
 *
 * This function can save the buffer-gap structure of BUFFER_GAP to a file.
 *
 * Returns: True/False depending on the result.
 ***/
int saveFile(mark_t *currentp, char *filename)
{
   size_t bytesWritten;
   FILE *filep;

   unsigned int bs, gs;
   char *b, *g;

   if (NULL == (filep= fopen(filename, "w"))){
      LOG("Could not open %s for saving.\n", filename);
      return 0;
   }

   g= currentp->stx->gap;
   b= currentp->stx->buffer;
   gs= currentp->stx->gap_size;
   bs= currentp->stx->buffer_size;

   /* XXX: Check this ... */
   if (g == b)
      {
         bytesWritten= fwrite (&(g[gs]), bs - gs, 1, filep);
         if (bs - gs != bytesWritten)
            {
               LOG("saveFile() - Only wrote %d bytes to %s!\n",
                        bytesWritten,
                        filename);
            }
      }
   else
      {
         bytesWritten= fwrite (b, g - b, 1, filep);
         if (b - g != bytesWritten)
            {
               LOG("saveFile() - Only wrote %d bytes to %s!\n",
                        bytesWritten,
                        filename);
            }
         bytesWritten= fwrite (&(g[gs]), bs - gs - (g - b), 1, filep);
         if (bs - gs - (g - b) != bytesWritten)
            {
               LOG("saveFile() - Only wrote %d bytes to %s!\n",
                        bytesWritten,
                        filename);
            }
      }

   fclose(filep);

   currentp->dirty = 0;

   return 1;
}
#endif /* BUFFER_GAP_MODE */


/*** redirect() *********************************************************
 * Tries to redirect a file to another.
 ************************************************************************
 */
void redirect(FILE *file_in, char *file_out)
{
   int fd;

   fd=open(file_out, O_WRONLY);
   if (-1 == fd){
      LOG("Couldn't redirect %d to %s... \n",
              fileno(file_in), file_out);
   }else{
      dup2(fd, fileno(file_in));
   }
}
