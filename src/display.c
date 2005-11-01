/* display.c - Display engine                    vim:set ts=3 sw=3 ruler:
 * Borde visa lite mer �n bara text & mark�r. Tex linjal och annat blaj
 ************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include "editor.h"
#include "plugin.h"
#include "display.h"
#include "memory.h"

static void displayStatus(mark_t *currentp);
static void displayCursor(mark_t *currentp);
static void displayText(mark_t *currentp);


void display(mark_t *currentp)
{
   getScreenMaxYX(&(currentp->scr.maxY), &(currentp->scr.maxX));
/* Reservera sista raden som statusrad... */
   currentp->scr.maxY -= 2; /* Kolla f�rst om noll!! */
   currentp->scr.maxX -= 0;

   displayText(currentp);
   displayStatus(currentp);

   /* Kanske g�ra om det h�r lite granna?
    * F�rslag:
    * screenRefresh (currentp);
    * Vet om var mark�ren �r s� den kan ritas dit sist av allt!
    */
   /* Nuvarande kladdiga r�ra */
   screenUpdate();
   displayCursor(currentp);
}


static void displayCursor(mark_t *currentp)
{
   screenPositionCursor(currentp->scr.x - currentp->scr.hpos, currentp->scr.y - currentp->scr.vpos);

/*  screenPositionCursor(currentp->scr.x, currentp->scr.y);
 */
}

static char *noname = "<noname>";

/* OBS m�ste ta h�nsyn till maxX && klara resize! */
static void displayStatus(mark_t *currentp)
{
   int i;
   char *ruler, *filename = currentp->filename;

   /* Rita direkt i framebuffern/virtuella sk�rmen ?! :) */

   /* Varning f�r livsfarlig kod!!! */
   ruler= allocate (currentp->scr.maxX, "displayStatus()");

   /* Tr�tt, skiter i s�kerhetsb�ltet :P */
   if (!filename)
     filename = noname;

   sprintf(ruler, "-GUL--%s-- %s --L%d-C%d",
           (currentp->dirty ? "**" : "--"),
           filename,
           currentp->scr.y, currentp->scr.x);

   for (i= strlen(ruler); i < currentp->scr.maxX; i++)
      ruler[i]= '-';

   for (i= 0; i < currentp->scr.maxX; i++)
      putPixchar(i, currentp->scr.maxY + 1, ruler[i]);

   free(ruler);
}

static void displayText(mark_t *currentp)
{
   int x, y, xmax, ymax;
   int i= 0;
   char ch;
   char *genbuf;

   genbuf= editorCharacterGenerator(currentp);

//   LOG("currentp->scr.maxY= %d, currentp->scr.maxX= %d\n", currentp->scr.maxY, currentp->scr.maxX);
//   LOG("%s\displayText() - nSlut p� genbuf.\n", genbuf);

   ymax= currentp->scr.maxY;
   xmax= currentp->scr.maxX + currentp->scr.hpos;

   for (y= 0; y < ymax; y++)
   {
      for (x= 0; x < xmax; x++)
      {
         ch= genbuf[i];
         if (0 != ch)
            i++;

/* EOL? Fill out with zeros. */
         if ('\n' == ch)
         {
            while (x < xmax)
               putPixchar(x++, y, 0);
         }
         else
         {
            putPixchar(x, y, ch);
         }
      }
// HAr vi redan detekterad newline? Eller finns det "skr�p" kvar till nyrad?
      if ('\n' != ch)
         while (0 != ch && '\n' != (ch= genbuf[i++]));
   }

   free(genbuf);
/*
  for (y= 0; y < currentp->scr.maxY; y++)
  {
  for (x= 0; x < currentp->scr.maxX; x++)
  {
  LOG("%c", getPixchar(x, y));
  }
  LOG("\n");
  }
*/
#if DEBUG == 1
   screenDebugDisplay();
#endif
}


void popUp_OK(char *message)
{
   LOG("%s", message);
}





#ifdef COMPATIBLE


/*** displayText() ******************************************************
 * M�ste g�ras om att ta pekare till f�rsta tecknet som skall visas samt
 * klara av resize "on the fly" s.a.s. dvs l�sa in nya maxX och maxY-
 * v�rden.
 * Fr�gan �r: Vem har hand om kontrollen �ver sj�lva vyn? Och hur skall
 * detta d� sk�tas f�r tex en resize kan vara lite sm�kinkig att g�ra.
 ************************************************************************
 */
static void displayText(mark_t *currentp)
{
   int x, y, xmax, ymax;
   int i= 0;
   char ch;
   char *genbuf;

   genbuf= editorCharacterGenerator(currentp);

//   LOG("currentp->scr.maxY= %d, currentp->scr.maxX= %d\n", currentp->scr.maxY, currentp->scr.maxX);
//   LOG("%s\displayText() - nSlut p� genbuf.\n", genbuf);

   ymax= currentp->scr.maxY;
   xmax= currentp->scr.maxX + currentp->scr.hpos;

   for (y= 0; y < ymax; y++)
   {
      for (x= 0; x < xmax; x++)
      {
         ch= genbuf[i];
         if (0 != ch)
            i++;

/* EOL? Fill out with zeros. */
         if ('\n' == ch)
         {
            while(x < xmax)
               putPixchar(x++, y, 0);
         }
         else
         {
            putPixchar(x, y, ch);
         }
      }
// HAr vi redan detekterad newline? Eller finns det "skr�p" kvar till nyrad?
      if ('\n' != ch)
         while (0 != ch && '\n' != (ch= genbuf[i++]));
   }

   free(genbuf);
/*
  for (y= 0; y < currentp->scr.maxY; y++){
  for (x= 0; x < currentp->scr.maxX; x++){
  LOG("%c", getPixchar(x, y));
  }
  LOG("\n");
  }
*/
#if DEBUG == 1
   screenDebugDisplay();
#endif
}


#endif

/* Local Variables:
 * mode: C;
 * c-file-style: ellemtel;
 * indent-tabs-mode: nil;
 * End:
 */
