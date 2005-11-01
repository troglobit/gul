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
static void backupBufferInfo    (text *thisp);
static void restoreBufferInfo   (text *thisp);
static int  movetoCol           (text *thisp, int col);
static int  movetoEOL           (text *thisp);
static int  movetoBOL           (text *thisp);
static int  movetoSTX           (text *thisp);
static int  movetoNextChar      (text *thisp);
static int  movetoPrevChar      (text *thisp);
static int  movetoNextLine      (text *thisp);
static int  movetoPrevLine      (text *thisp);
static char getCurrentChar      (text *thisp);
static int  getCurrentColumn    (text *thisp);
static int  atSTX               (text *thisp);
static int  move_virtual_screen (mark_t *currentp, int offset);


/* Must be called after any movement {left,right,up,down}
 * XXX - Should perhaps be in display.c?
 */
void adjust_virtual_screen (mark_t *currentp)
{
/* left */
   while ((currentp->scr.x - currentp->scr.hpos) <= 0 && currentp->scr.hpos >= 5)
   {
      currentp->scr.hpos -= 5;
   }
/* right */
   while ((currentp->scr.x - currentp->scr.hpos) >= (currentp->scr.maxX - 1))
   {
      currentp->scr.hpos += 5;
   }

/* up */
   while ((currentp->scr.y - currentp->scr.vpos) <= 0 && currentp->scr.vpos >= 5)
   {
      currentp->scr.vpos -= move_virtual_screen (currentp, -5);
   }
/* down */
   while ((currentp->scr.y - currentp->scr.vpos) >= currentp->scr.maxY)
   {
      currentp->scr.vpos += move_virtual_screen (currentp, 5);
   }
}

/*
 * left():
 * @currentp: The current buffer state.
 *
 * Moves the cursor left one character.
 */
void
left (mark_t *currentp)
{
   if (movetoPrevChar (currentp->stx))
   {
/* Failed to move */
   }
   else
   {
      if (10 == getCurrentChar (currentp->stx))
      {
         currentp->scr.x= getCurrentColumn (currentp->stx);
         currentp->scr.y--;
      }
      else
      {
         currentp->scr.x--;
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
right (mark_t *currentp)
{
   char prevChar;

   prevChar= getCurrentChar (currentp->stx);
   if (movetoNextChar (currentp->stx))
   {
/* Failed to move */
   }
   else
   {
      if (10 == prevChar)
      {
         currentp->scr.hpos = 0;
         currentp->scr.x= 0;
         currentp->scr.y++;
      }
      else
      {
         currentp->scr.x++;
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
up (mark_t *currentp)
{
   if (movetoPrevLine (currentp->stx))
      ;
   else
   {
      currentp->scr.y--;
      currentp->scr.x= movetoCol(currentp->stx, currentp->scr.x);
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
down (mark_t *currentp)
{
   if (movetoNextLine (currentp->stx))
      ;
   else
   {
      currentp->scr.y++;
      currentp->scr.x= movetoCol(currentp->stx, currentp->scr.x);
   }

   adjust_virtual_screen (currentp);
}


void
end_of_line (mark_t *currentp)
{
   movetoEOL (currentp->stx);
   currentp->scr.x = getCurrentColumn (currentp->stx);

   adjust_virtual_screen (currentp);
}

void
beginning_of_line (mark_t *currentp)
{
   movetoBOL (currentp->stx);
   currentp->scr.x = 0;

   adjust_virtual_screen (currentp);
}

void
page_up (mark_t *currentp)
{
   int i;
   int offset   = currentp->scr.maxY / 2; /* Move only half of screen */

   for (i = 0; i < offset; i++)
   {
      if (movetoPrevLine (currentp->stx))
      {
         break;
      }
      else
      {
         currentp->scr.y--;
      }
   }

   currentp->scr.x= movetoCol(currentp->stx, currentp->scr.x);
   adjust_virtual_screen (currentp);
}

void
page_down (mark_t *currentp)
{
   int i;
   int offset   = currentp->scr.maxY / 2; /* Move only half of screen */

   for (i = 0; i < offset; i++)
   {
      if (movetoNextLine (currentp->stx))
      {
         break;
      }
      else
      {
         currentp->scr.y++;
      }
   }

   currentp->scr.x= movetoCol(currentp->stx, currentp->scr.x);
   adjust_virtual_screen (currentp);
}

/* try moving the screen vertically by @offset */
static int
move_virtual_screen (mark_t *currentp, int offset)
{
   int counter = 0;

   if (0 > offset)
   {
      while (0 != offset)
      {
         if (movetoPrevLine (currentp->top))
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
         if (movetoNextLine (currentp->top))
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
allocate_new_gap (mark_t *currentp)
{
   int newsize    = currentp->stx->buffer_size + GAP_SIZE;
   char *newbuffer= allocate (newsize, "allocate_new_gap()");

   char *first = currentp->stx->buffer;
   int firstlen= currentp->stx->gap - currentp->stx->buffer;
   char *second = &(currentp->stx->gap[currentp->stx->gap_size]);
   int secondlen= currentp->stx->buffer + currentp->stx->buffer_size - second;

   memcpy (newbuffer, first, firstlen);
   memcpy (&(newbuffer[firstlen + GAP_SIZE]), second, secondlen);

   free (currentp->stx->buffer);
   currentp->stx->buffer= newbuffer;
   currentp->stx->buffer_size= newsize;
   currentp->stx->gap= newbuffer + firstlen + GAP_SIZE;
   currentp->stx->gap_size= GAP_SIZE;
}


/*
 * insert():
 * @currentp:    The current buffer state.
 * @thisCommand: The character/command to insert.
 *
 * Insert a charcter/command at the current cursor position.
 */
void
insert (mark_t *currentp, int thisCommand)
{
   if (0 == currentp->stx->gap_size)
      allocate_new_gap (currentp);

   currentp->stx->gap[0]= (char)thisCommand;
   currentp->stx->gap++;
   currentp->stx->gap_size--;

   currentp->dirty = 1;

   switch (thisCommand)
   {
      case 10:                  /* NL */
         currentp->scr.x = 0;
         currentp->scr.y++;
         break;

      case 9:                   /* HT, TAB */
         currentp->scr.x +=8;
         break;

      default:
         currentp->scr.x++;
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
delete (mark_t *currentp)
{
/* Detect if gap at end of buffer */
   if (GAP_AT_END (currentp->stx))
   {
/* Cursor at end of buffer, no delete */
   }
   else
   {
      currentp->stx->gap_size++;
      currentp->dirty = 1;
   }

   adjust_virtual_screen (currentp);
}


/**
 * editorCharacterGenerator - Generate data to display engine
 * @currentp: Pointer to the current buffer.
 *
 * Generate a screenful of data from @currentp->top to be written to the
 * display.
 *
 * Returns: A dynamically allocated character array of data to be freed
 *          after usage.
 */
char *
editorCharacterGenerator (mark_t *currentp)
{
   char curr;
   char *genbuf= NULL;
   int  i, j, v_pos, h_pos, h_pos_max = currentp->scr.hpos + currentp->scr.maxX;
   int nofbytes=  (currentp->scr.maxX) * (currentp->scr.maxY);
   text runner;    /* traverses the current virtual screen */

   memcpy (&runner, currentp->top, sizeof (text));

/* We want an array of characters that is maxX * maxY large.
 * if we cannot get that much we want at least to have
 * an array with lots of NL and ended with a NUL.
 */
   genbuf= allocate(nofbytes, "editorCharacterGenerator");

   for (i = 0, v_pos = 0; v_pos < currentp->scr.maxY; v_pos ++)
   {
/* Adjust for horizontal starting position */
      h_pos = 0;
      while (10 != getCurrentChar (&runner) && h_pos < currentp->scr.hpos)
      {
         movetoNextChar (&runner);
         h_pos ++;
      }
      for (; h_pos < h_pos_max; h_pos++)
      {
/* End of buffer? */
         if (runner.gap ==
             &(currentp->stx->buffer[currentp->stx->buffer_size]))
         {
/* set exit condition for outer for() */
            v_pos = currentp->scr.maxY;

/* break out of inner for() */
            break;
         }

/* Inside the real gap? Skip the real gap! */
         if (runner.gap == currentp->stx->gap)
         {
            runner.gap += currentp->stx->gap_size;
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

#ifdef BIZARRE_OLD_STUFF
/* horiz offset? */
   if (currentp->scr.hpos)
   {
      h_pos = 0;
      while (10 != getCurrentChar (&runner) && h_pos < currentp->scr.hpos)
      {
         movetoNextChar (&runner);
         h_pos ++;
      }
   }

   i= 0;
   while (i < nofbytes)
   {
/* End of buffer? */
      if (runner.gap ==
          &(currentp->stx->buffer[currentp->stx->buffer_size]))
         break;

/* Inside the real gap? Skip the real gap! */
      if (runner.gap == currentp->stx->gap)
      {
         runner.gap += currentp->stx->gap_size;
      }

      curr = getCurrentChar (&runner);
/* Not in correct h_pos and not an EOL */
      if (10 != curr && h_pos < currentp->scr.hpos)
      {
         h_pos ++;
         movetoNextChar (&runner);
      }
      if (10 == curr && h_pos <= currentp->scr.hpos)
      {
         h_pos = 0;
         genbuf[i++]= curr;

         movetoNextChar (&runner);
      }
      else
      {
         if (h_pos < h_pos_max)
         {
            h_pos ++;
            genbuf[i++]= curr;
            movetoNextChar (&runner);
         }
         else
         {
            h_pos = 0;
            genbuf[i++] = 10;   /* NL */
            movetoNextLine (&runner);
         }

      }
   }

   genbuf[i]= '\0';
#endif /* BIZARRE_OLD_STUFF */

   return genbuf;
}

/* XXX - This is the definition of *not* being reentrant. */
static char *old_gap;

/*
 * backupBufferInfo():
 *
 * Saves current setting of the gap for the below
 * function to be able to restore it later.
 */
static void
backupBufferInfo (text *thisp)
{
   old_gap = thisp->gap;
}


/*
 * restoreBufferInfo():
 *
 * Recalls old setting of the gap.
 */
static void
restoreBufferInfo (text *thisp)
{
   memmove (old_gap, thisp->gap, thisp->gap_size);
   thisp->gap= old_gap;
}


/*
 * freeBufferInfo():
 *
 * Deadweight in this core, but still here to maintain
 * ... eh ... symmetry? *Dylan*
 */
static void
freeBufferInfo (mark_t *backup_info)
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
movetoNextChar (text *thisp)
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
movetoPrevChar (text *thisp)
{
   if (GAP_AT_START (thisp))
      return 1;

   thisp->gap--;
   thisp->gap[thisp->gap_size] = thisp->gap[0];

   return 0;
}


static int
movetoNextLine (text *thisp)
{
   backupBufferInfo (thisp);

   movetoEOL (thisp);
   if (movetoNextChar (thisp))
   {
      restoreBufferInfo (thisp);
      return 1;
   }

   return 0;
}


static int
movetoPrevLine (text *thisp)
{
   backupBufferInfo (thisp);

/* movetoBOL() should never (!) fail ... */
   movetoBOL (thisp);
   if(movetoPrevChar (thisp))
   {
      restoreBufferInfo (thisp);
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
atSTX (text *thisp)
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
getCurrentChar (text *thisp)
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
getCurrentColumn (text *thisp)
{
   char curr;
   int counter = 0;

   backupBufferInfo(thisp);

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

   restoreBufferInfo(thisp);

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
movetoEOL (text *thisp)
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
movetoBOL (text *thisp)
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
movetoSTX (text *thisp)
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
movetoCol (text *thisp, int col)
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

#endif /* BUFFER_GAP_MODE */



#ifdef COMPATIBLE

char *
editorCharacterGenerator (mark_t *currentp)
{
   char *genbuf= NULL;
   int  i, j;

   int nofbytes=  (currentp->scr.maxX) * (currentp->scr.maxY);
   genbuf= allocate(nofbytes, "editorCharacterGenerator");

   i= 0;
   j= 0;
   while (i < nofbytes)
   {
/* End of buffer? */
      if (&(currentp->top->buffer[j]) ==
          &(currentp->stx->buffer[currentp->stx->buffer_size]))
         break;

/* Inside gap? Skip gap! */
      if (&(currentp->top->buffer[j]) == currentp->stx->gap)
      {
         j+= currentp->stx->gap_size;
      }
      genbuf[i++]= currentp->top->buffer[j++];
   }

   genbuf[i]= '\0';

   return genbuf;
}

#endif /* COMPATIBLE */



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

/* Local Variables:
 * mode: C;
 * c-file-style: ellemtel;
 * indent-tabs-mode: nil;
 * End:
 */
