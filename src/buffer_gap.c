/* buffer_gap.c - Yet Another Editor core Impl.        vim:set ts=3 sw=3 ruler:
 *
 * Yet another editor-core implementation.
 * Also known as "This ain't no disco!", courtesy of Depeche Mode.
 * First try at an implementation of the buffer gap method.
 *
 * Joachim Nilsson <crash@vmlinux.org>
 *
 ************************************************************************
 * Problems:
 ***********
 * Currently the API and the "chain of command" between the display and the
 * editor core is not as well defined as one might want. This needs looking
 * in to!
 *
 * What if the top->gap_size == 0 ???
 * Use a special movetoNextChar() and getCurrentChar() when moving around
 * chasing visible characters in the redraw() function???
 * Perhaps even move the "screen_gap" into the primary, stx, struct?
 * this would perhaps simplify things a bit.
 *
 * But all this relies most on the above statement!!
 *
 * Maybe support both a redraw() function AND a character generator, that
 * way bindings can be done at compile- or at run-time. Set up a bindings
 * table for each plugin to fill in. Similar to that of GWP! :)
 *
 * I've commented out all of the "move the virtual screen"-stuff. So now
 * the editor is only one (1) screensize big - That's it!
 */

#include "config.h"
#ifdef BUFFER_GAP_MODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fstat, fileno etc. */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "editor.h"
#include "buffer_gap.h"
#include "memory.h"

/*** Macros *************************************************************
 */

#define GAP_AT_START(this)                      \
   ((this)->gap == (this)->buffer)

#define GAP_AT_END(this)                                                \
   ((this)->gap + (this)->gap_size - (this)->buffer == (this)->buffer_size)

/*** Prototypes *********************************************************
 */
static char *backupBufferInfo    (text_t *thisp);
static char *restoreBufferInfo   (text_t *thisp, char *);
static int  movetoCol           (text_t *thisp, int col);
static int  movetoEOL           (text_t *thisp);
static int  movetoBOL           (text_t *thisp);
static int  movetoSTX           (text_t *thisp);
static int  movetoNextChar      (text_t *thisp);
static int  movetoPrevChar      (text_t *thisp);
static int  movetoNextLine      (text_t *thisp);
static int  movetoPrevLine      (text_t *thisp);
static char getCurrentChar      (text_t *thisp);
static int  getCurrentColumn    (text_t *thisp);
static int  atSTX               (text_t *thisp);
static int  move_virtual_screen (buffer_t *currentp, int offset);


/* Must be called after any movement {left,right,up,down}
 * XXX - Should perhaps be in display.c?
 */
void adjust_virtual_screen (buffer_t *currentp)
{
/* left */
   while ((currentp->screen.x - currentp->screen.hpos) <= 0 && currentp->screen.hpos >= 5)
   {
      currentp->screen.hpos -= 5;
   }
/* right */
   while ((currentp->screen.x - currentp->screen.hpos) >= (currentp->screen.maxX - 1))
   {
      currentp->screen.hpos += 5;
   }

/* up */
   while ((currentp->screen.y - currentp->screen.vpos) <= 0 && currentp->screen.vpos >= 5)
   {
      currentp->screen.vpos -= move_virtual_screen (currentp, -5);
   }
/* down */
   while ((currentp->screen.y - currentp->screen.vpos) >= currentp->screen.maxY)
   {
      currentp->screen.vpos += move_virtual_screen (currentp, 5);
   }
}

/*
 * left():
 * @currentp: The current buffer state.
 *
 * Moves the cursor left one character.
 */
void
left (buffer_t *currentp)
{
   if (movetoPrevChar (&currentp->core))
   {
/* Failed to move */
   }
   else
   {
      if (10 == getCurrentChar (&currentp->core))
      {
         currentp->screen.x= getCurrentColumn (&currentp->core);
         currentp->screen.y--;
      }
      else
      {
         currentp->screen.x--;
      }
   }

   adjust_virtual_screen (currentp);
}


/*
 * right():
 * @currentp: The current buffer state.
 *
 * Moves the cursor right one character.
 */
void
right (buffer_t *currentp)
{
   char prevChar;

   prevChar= getCurrentChar (&currentp->core);
   if (movetoNextChar (&currentp->core))
   {
/* Failed to move */
   }
   else
   {
      if (10 == prevChar)
      {
         currentp->screen.hpos = 0;
         currentp->screen.x= 0;
         currentp->screen.y++;
      }
      else
      {
         currentp->screen.x++;
      }
   }

   adjust_virtual_screen (currentp);
}


/*
 * up():
 * @currentp: The current buffer state.
 *
 * Moves the cursor up one line.
 */
void
up (buffer_t *currentp)
{
   if (movetoPrevLine (&currentp->core))
      ;
   else
   {
      currentp->screen.y--;
      currentp->screen.x= movetoCol(&currentp->core, currentp->screen.x);
   }

   adjust_virtual_screen (currentp);
}

/*
 * down():
 * @currentp: The current buffer state.
 *
 * Moves the cursor down one line.
 */
void
down (buffer_t *currentp)
{
   if (movetoNextLine (&currentp->core))
      ;
   else
   {
      currentp->screen.y++;
      currentp->screen.x= movetoCol(&currentp->core, currentp->screen.x);
   }

   adjust_virtual_screen (currentp);
}


void
end_of_line (buffer_t *currentp)
{
   movetoEOL (&currentp->core);
   currentp->screen.x = getCurrentColumn (&currentp->core);

   adjust_virtual_screen (currentp);
}

void
beginning_of_line (buffer_t *currentp)
{
   movetoBOL (&currentp->core);
   currentp->screen.x = 0;

   adjust_virtual_screen (currentp);
}

void
page_up (buffer_t *currentp)
{
   int i;
   int offset   = currentp->screen.maxY / 2; /* Move only half of screen */

   for (i = 0; i < offset; i++)
   {
      if (movetoPrevLine (&currentp->core))
      {
         break;
      }
      else
      {
         currentp->screen.y--;
      }
   }

   currentp->screen.x= movetoCol(&currentp->core, currentp->screen.x);
   adjust_virtual_screen (currentp);
}

void
page_down (buffer_t *currentp)
{
   int i;
   int offset   = currentp->screen.maxY / 2; /* Move only half of screen */

   for (i = 0; i < offset; i++)
   {
      if (movetoNextLine (&currentp->core))
      {
         break;
      }
      else
      {
         currentp->screen.y++;
      }
   }

   currentp->screen.x= movetoCol(&currentp->core, currentp->screen.x);
   adjust_virtual_screen (currentp);
}

/* try moving the screen vertically by @offset */
static int
move_virtual_screen (buffer_t *currentp, int offset)
{
   int counter = 0;

   if (0 > offset)
   {
      while (0 != offset)
      {
         if (movetoPrevLine (currentp->screen.top))
            return counter;
         else
            counter++;

         offset++;
      }
   }
   else
   {
      while (0 != offset)
      {
         if (movetoNextLine (currentp->screen.top))
            return counter;
         else
            counter++;

         offset--;
      }
   }

   return counter;
}



/*
 * allocate_new_gap():
 * @currentp: Pointer to the current edit buffer.
 *
 * Whenever the gap is filled it needs to be reallocated. This function
 * takes care of everything - fixing all pointers and keeping the whole
 * consistent. Messy code to follow ... /Jocke
 *
 */
static void
allocate_new_gap (buffer_t *currentp)
{
   int newsize    = currentp->core.buffer_size + GAP_SIZE;
   char *newbuffer= allocate (newsize, "allocate_new_gap()");

   char *first = currentp->core.buffer;
   int firstlen= currentp->core.gap - currentp->core.buffer;
   char *second = &(currentp->core.gap[currentp->core.gap_size]);
   int secondlen= currentp->core.buffer + currentp->core.buffer_size - second;

   memcpy (newbuffer, first, firstlen);
   memcpy (&(newbuffer[firstlen + GAP_SIZE]), second, secondlen);

   free (currentp->core.buffer);
   currentp->core.buffer      = newbuffer;
   currentp->core.buffer_size = newsize;
   currentp->core.gap         = newbuffer + firstlen + GAP_SIZE;
   currentp->core.gap_size    = GAP_SIZE;
}


/*
 * insert():
 * @currentp:    The current buffer state.
 * @thisCommand: The character/command to insert.
 *
 * Insert a charcter/command at the current cursor position.
 */
void
insert (buffer_t *currentp, int thisCommand)
{
   if (0 == currentp->core.gap_size)
      allocate_new_gap (currentp);

   currentp->core.gap[0]= (char)thisCommand;
   currentp->core.gap++;
   currentp->core.gap_size--;

   currentp->dirty = 1;

   switch (thisCommand)
   {
      case 10:                  /* NL */
         currentp->screen.x = 0;
         currentp->screen.y++;
         break;

      case 9:                   /* HT, TAB */
         currentp->screen.x +=8;
         break;

      default:
         currentp->screen.x++;
         break;
   }

   adjust_virtual_screen (currentp);
}


/*
 * delete():
 * @currentp: The current buffer state.
 *
 * Delete the charcter/command at the current cursor position.
 */
void
delete (buffer_t *currentp)
{
/* Detect if gap at end of buffer */
   if (GAP_AT_END (&currentp->core))
   {
/* Cursor at end of buffer, no delete */
   }
   else
   {
      currentp->core.gap_size++;
      currentp->dirty = 1;
   }

   adjust_virtual_screen (currentp);
}


void set_mark (buffer_t *currentp)
{
   currentp->core.mark = currentp->core.gap;
}

void set_point (buffer_t *currentp)
{
   currentp->core.point = currentp->core.gap;
}

void cut (buffer_t *currentp)
{
   set_point (currentp);

/* Do cutting of text here. */
}

void copy (buffer_t *currentp)
{
   set_point (currentp);
}

void paste (buffer_t *currentp)
{
/* here is where the actual copying of the
 * previously marked text is done.
 */
   char *runner = currentp->core.mark;

   while (runner < currentp->core.point)
   {
      insert (currentp, runner[0]);
      runner ++;
   }
}


/**
 * editorCharacterGenerator - Generate data to display engine
 * @currentp: Pointer to the current buffer.
 *
 * Generate a screenful of data from @currentp->screen.top to be written to the
 * display.
 *
 * Returns: A dynamically allocated character array of data to be freed
 *          after usage.
 */
char *
editorCharacterGenerator (buffer_t *currentp)
{
   char   curr;
   char  *genbuf= NULL;
   int    i, j, v_pos, h_pos, h_pos_max = currentp->screen.hpos + currentp->screen.maxX;
   int    nofbytes=  (currentp->screen.maxX) * (currentp->screen.maxY);
   text_t runner;    /* traverses the current virtual screen */

   memcpy (&runner, currentp->screen.top, sizeof (text_t));

/* We want an array of characters that is maxX * maxY large.
 * if we cannot get that much we want at least to have
 * an array with lots of NL and ended with a NUL.
 */
   genbuf= allocate(nofbytes, "editorCharacterGenerator");

   for (i = 0, v_pos = 0; v_pos < currentp->screen.maxY; v_pos ++)
   {
/* Adjust for horizontal starting position */
      h_pos = 0;
      while (10 != getCurrentChar (&runner) && h_pos < currentp->screen.hpos)
      {
         movetoNextChar (&runner);
         h_pos ++;
      }
      for (; h_pos < h_pos_max; h_pos++)
      {
/* End of buffer? */
         if (runner.gap ==
             &(currentp->core.buffer[currentp->core.buffer_size]))
         {
/* set exit condition for outer for() */
            v_pos = currentp->screen.maxY;

/* break out of inner for() */
            break;
         }

/* Inside the real gap? Skip the real gap! */
         if (runner.gap == currentp->core.gap)
         {
            runner.gap += currentp->core.gap_size;
         }

         curr = getCurrentChar (&runner);
         switch (curr)
         {
            case 10:            /* NL */
               h_pos = h_pos_max;
               break;

            case 9:             /* TAB */
               for (j = 0; j < 8; j++)
                  genbuf [i++] = ' ';
               break;

            default:
               genbuf [i++] = curr;
         }
         if (curr != 10)
            movetoNextChar (&runner);
      }
/* Always end every line with NL */
      genbuf [i++] = 10;         /* NL */

      movetoNextLine (&runner);
   }

/* Always terminate the buffer. */
   genbuf [i] = 0;

   return genbuf;
}


/*
 * backupBufferInfo():
 *
 * Saves current setting of the gap for the below
 * function to be able to restore it later.
 */
static char *
backupBufferInfo (text_t *thisp)
{
   return thisp->gap;
}


/*
 * restoreBufferInfo():
 *
 * Recalls old setting of the gap.
 * Returns: Gap pos. priot to running this func.
 */
static char *
restoreBufferInfo (text_t *thisp, char *backup_gap)
{
  char *gap_bis = thisp->gap;

   memmove (backup_gap, thisp->gap, thisp->gap_size);
   thisp->gap= backup_gap;

   return gap_bis;
}


/*
 * freeBufferInfo():
 *
 * Deadweight in this core, but still here to maintain
 * ... eh ... symmetry? *Dylan*
 */
static void
freeBufferInfo (buffer_t *backup_info)
{
}



/*
 * movetoNextChar():
 * @thisp: The current buffer state.
 *
 * Simply moves the gap/cursor to the next character. Only one possibility
 * of failure; when the gap is at the very end of the buffer.
 *
 * Returns: 0 if the operation was successful;
 *          1 otherwise.
 */
static int
movetoNextChar (text_t *thisp)
{
   if (GAP_AT_END (thisp))
      return 1;

   thisp->gap[0] = thisp->gap[thisp->gap_size];
   thisp->gap++;

   return 0;
}


/*
 * movetoPrevChar():
 * @thisp: The current buffer state.
 *
 * Simply moves the gap/cursor to the next character. Only one possibility
 * of failure; when the gap is as far as it can get to the "left on the tape",
 * i.e. when it is at the beginning of the file.
 *
 * Returns: 0 if the operation was successful;
 *          1 otherwise.
 */
static int
movetoPrevChar (text_t *thisp)
{
   if (GAP_AT_START (thisp))
      return 1;

   thisp->gap--;
   thisp->gap[thisp->gap_size] = thisp->gap[0];

   return 0;
}


static int
movetoNextLine (text_t *thisp)
{
   char *backup = backupBufferInfo (thisp);

   movetoEOL (thisp);
   if (movetoNextChar (thisp))
   {
     restoreBufferInfo (thisp, backup);
      return 1;
   }

   return 0;
}


static int
movetoPrevLine (text_t *thisp)
{
   char *backup = backupBufferInfo (thisp);

/* movetoBOL() should never (!) fail ... */
   movetoBOL (thisp);
   if(movetoPrevChar (thisp))
   {
     restoreBufferInfo (thisp, backup);
      return 1;
   }

/* ... neither should it fail here ... I hope! */
   movetoBOL (thisp);

   return 0;
}


/*
 * atSTX():
 * @thisp: The current buffer state.
 *
 * Answers the question wether the cursor is "home" or not.
 *
 * Returns: True or false depending on the success of the operation.
 */
static int
atSTX (text_t *thisp)
{
   return GAP_AT_START (thisp);
}


/*
 * getCurrentChar():
 * @thisp: The current buffer state.
 *
 * Retrieves the character which the gap is currently located "over".
 *
 * Returns: The character just outside the gap to the right.
 */
static char
getCurrentChar (text_t *thisp)
{
   return thisp->gap[thisp->gap_size];
}

/*
 * getCurrentColumn():
 * @thisp: Current state of buffer.
 *
 * This little "hack" calculates where the cursor is located x-wise. It
 * is actually just a function that can be used to determine the line
 * length. Which makes it extremely useful in a buffer-gap implementation.
 *
 * Returns: The character position.
 */
static int
getCurrentColumn (text_t *thisp)
{
   char curr;
   int counter = 0;

   char *backup = backupBufferInfo(thisp);

   while (!movetoPrevChar(thisp))
   {
      curr = getCurrentChar (thisp);
      if (10 == curr)
      {
         movetoNextChar (thisp);
         break;
      }
      if (9 == curr)
      {
         counter += 8;
         continue;
      }
      /* else */
      counter++;
   }

   restoreBufferInfo(thisp, backup);

   return counter;
}


/*
 * movetoEOL():
 * @thisp: The current buffer state.
 *
 * Tries to move the insertpoint to EOL.
 *
 * 1) We're already at EOL -> OK
 * 2) While not at EOL movetoNextChar (if that's possible)
 *
 * Returns: 0 if the operation was successful;
 *          1 otherwise.
 */
static int
movetoEOL (text_t *thisp)
{
   while (10 != getCurrentChar (thisp))
      if (movetoNextChar (thisp))
         return 1;

   return 0;
}


/*
 * movetoBOL():
 * @thisp: The current buffer state.
 *
 * Puts the cursor at the first column of the current line.
 *
 * There is really only one problem and that is when you are at the
 * beginning of the file. No previous nodes or previous newlines...
 * But none of these are really no problem since you always can find the
 * beginning of a line as well as the sister function movetoEOL()!
 *
 * Returns: 0 if the operation was successful;
 *          1 otherwise.
 */
static int
movetoBOL (text_t *thisp)
{
   while (!movetoPrevChar(thisp))
   {
      if (10 == getCurrentChar (thisp))
      {
         movetoNextChar (thisp);
         return 0;
      }
   }

   return 0;
}


/*
 * movetoSTX():
 * @thisp: The current buffer state.
 *
 * Moves to the beginning/start of the text.
 *
 * Returns: 0 if the operation was successful;
 *          1 otherwise.
 */
static int
movetoSTX (text_t *thisp)
{
   if (GAP_AT_START (thisp))
      return 0;

   memmove (thisp->buffer,
            thisp->gap,
            thisp->gap_size);

   return 0;
}


/*
 * movetoCol():
 * @thisp: The current state of the buffer.
 * @col:      The new column.
 *
 * Moves the gap to the specified column.
 *
 * Returns: How far the function managed to move the gap on the current line.
 */
static int
movetoCol (text_t *thisp, int col)
{
   int counter= 0;

/* This should NEVER happen!! Semantically that is ... :/ */
   if (movetoBOL(thisp))
      return counter;

   for (counter= 0;
        (counter < col) && (10 != getCurrentChar(thisp));
        counter++)
      movetoNextChar(thisp);

   return counter;
}


int newFile (buffer_t *new, size_t size)
{
  /* Allocate a data buffer */
  new->core.buffer      = allocate (size + GAP_SIZE, "newFile()");
  new->core.buffer_size = size + GAP_SIZE;

  /* Initial size and location of the gap */
  new->core.gap      = new->core.buffer;
  new->core.gap_size = GAP_SIZE;

  return 0;
}

/***
 * readFile()
 * @filep: Regular file pointer.
 * @core:  Pointer to the new buffer core.
 *
 * Reads the current file into the array used in the buffer-gap scheme by
 * BUFFER_GAP. Very simple, indeed.
 */
int readFile(FILE* filep, buffer_t *new)
{
   size_t  bytesRead;
   struct stat filestatus;

   if (0 != fstat (fileno (filep), &filestatus))
      {
         LOG("Could not fstat file before opening!\n");
         return -1;
      }

   newFile (new, filestatus.st_size);

   /* Start reading in the file after the gap. */
   bytesRead= fread (&(new->core.buffer[GAP_SIZE]),
                     filestatus.st_size,
                     1,
                     filep);
   if (bytesRead != filestatus.st_size)
      {
         LOG("readFile() - Read: %d, file size: %d ?\n",
                  bytesRead,
                  (int)filestatus.st_size);
      }

   return 0;
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
int saveFile(buffer_t *currentp, char *filename)
{
   size_t bytesWritten;
   FILE *filep;

   unsigned int bs, gs;
   char *b, *g;

   if (NULL == (filep= fopen(filename, "w"))){
      LOG("Could not open %s for saving.\n", filename);
      return 0;
   }

   g= currentp->core.gap;
   b= currentp->core.buffer;
   gs= currentp->core.gap_size;
   bs= currentp->core.buffer_size;

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


void
coreNewScreen (buffer_t *newBuffer)
{
   /* BUFFER_GAP needs this copy for the virtual screen */
   newBuffer->screen.top = allocate (sizeof (text_t), __FUNCTION__);
   memcpy (newBuffer->screen.top, &newBuffer->core, sizeof (text_t));

   /* Invisible cursor that indicates top of screen */
   newBuffer->screen.top->gap_size = 0;

   newBuffer->position = 0;
   newBuffer->screen.x = 0;
   newBuffer->screen.y = 0;
}


void
freeBuffer (buffer_t *buf)
{
  free (buf->filename);
  free (buf->core.buffer);
}


#endif /* BUFFER_GAP_MODE */






#ifdef TOTALLY_NEW_COOL_STUFF

/*
 * The screen sez to the core:
 * "Redraw thyself on this square!"
 *
 * The core looks upon the provided square and traverses its
 * "top" of page locator (points to the first visible screen row)
 * and writes to the screen using putPixchar().
 *
 * Actors in screenplay
 * ====================
 *   the top of screen locator:
 *     top  - recursive definition: "top of page"
 *   dimensions_and_offsets:
 *     hpos - horisontal offset.
 *     vpos - vertical offset.
 *     xmax - Maximum screen width
 *     ymax - Maximum screen height
 *     x    - cursor (insert point/gap) absolute X pos in document
 *     y    - cursor  absolute Y pos in document
 *
 *   screen:
 *     simply the virtual screen buffer with its primitives.
 *
 *   the_buffer:
 *     includes current screen cursor pos and the actual buffer.
 */
void
redraw (ptr *screen, ptr *dimensions_and_offsets, ptr *the_buffer)
{
   int sy = 0;

   while (sy < ymax)
   {
      if (movetoPosInLine (the_buffer, hpos))
/* Failed to move, line not that long -> next line please */
         ;
      else
         drawThisLine (screen, the_buffer, sy, xmax);

/* NExt screen line please */
      sy++;
   }

}


/*
 * movetoPosInLine():
 * Moves to the first visible character on a row.
 * Assumes that the gap is located over the first character on a visible row.
 *
 * Returns: 1 if something went wrong, i.e. no more visible chars. on row.
 *          0 if everything is OK.
 */
static int
movetoPosInLine (ptr *the_buffer, int hpos)
{
   int counter = 0;

   while (counter < hpos)
   {
      if (10 == getCurrentChar (the_buffer))
         return 1;
      if (movetoNextChar (the_buffer))
         return 1;
   }

   return 0;
}


/*
 * drawThisLine():
 * Assumes that the gap is located over the first visible character
 * on a visible line that is to be printed to the virtual screen.
 */
static void
drawThisLine (ptr screen, ptr *the_buffer, int screen_y, int xmax)
{
   int screen_x = 0;
   char character;

   while (screen_x < xmax)
   {
      character = getCurrentChar (the_buffer);
      if (10 == character)
      {
/* No more characters on this line, fill out with zeroes. */
         while (screen_x < xmax)
         {
            putPixchar (screen_x, screen_y, 0);
            screen_x++;
         }
      }
      else
      {
         putPixchar (screen_x, screen_y, character);
         movetoNextChar (the_buffer);
         screen_x++;
      }
   }

/* Upon exit make sure that the gap/cursor is located over
 * the first character on the next row.
 */
   while (10 != character)
   {
      movetoNextChar (the_buffer);
      character = getCurrentChar (the_buffer);
   }
   movetoNextChar (the_buffer);
}
#endif /* TOTALLY_NEW_COOL_STUFF */


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
