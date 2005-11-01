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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_LOCALE_H
#include <locale.h>
#endif

#include <signal.h>
#include "config.h"
#include "editor.h"
#include "plugin.h"
#include "memory.h"
#include "fileio.h"
#include "display.h"
#include "misc.h"

/* Liten fånig text för att retas med den som läser i binären... :) */
char teaser[]="Nyfiken gul? (Which is swedish for: \"Curious yellow?\" ;-)";

/*** Prototypes *********************************************************
 */
static void finish(int sig);



int main(int argc, char *argv[])
{
   int quit = 0;
   mark_t *currentBuffer;
   char *fileName= NULL;
   keyevent thisEvent;

   if (argv[1])
      fileName= strDup(argv[1]);

   currentBuffer= loadFile(fileName);

/* openDisplay(); */
   createScreen();

/* Take care of Ctrl-C */
   signal(SIGINT, finish);

/* Debugging sent to /dev/null or stderr ... */
#if DEBUG == 0
   redirect(stderr, "/dev/null");
#endif

   LOG("main() - display(currentBuffer);\n");
   display(currentBuffer);

/* Main editor loop. */
   while (!quit)
   {
     /* Wait for an event ... */
      do {
         thisEvent= gul_event();
      }while (GUL_NO_EVENT == thisEvent.event);

      switch (thisEvent.event)
      {
         case GUL_DATA:
            if (!thisEvent.keydata) /* I.e. "if NO keydata then ..." */
               ;
            else
            {
               LOG("[%c]\n", (char)thisEvent.keydata);
               insert(currentBuffer, thisEvent.keydata);
            }
            break;

         case GUL_ENTER:
            insert (currentBuffer, 10);
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
            break;

         case GUL_F2:
            LOG("[F2]\n");
/* if (!currentBuffer->filename)
 *   {
 *    fancy_get_filename_dialog(&(currentBuffer->filename));
 *   }
 * saveFile(currentBuffer, currentBuffer->filename);
 */
            if (!currentBuffer->filename)
               saveFile(currentBuffer, "tomten_sparar");
            else
               saveFile(currentBuffer, currentBuffer->filename);
            break;

         case GUL_F3:
            LOG("[F3]\n");
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
            beginning_of_line (currentBuffer);
            break;
         case GUL_END:
            LOG("[END]\n");
            end_of_line (currentBuffer);
            break;
         case GUL_PPAGE:
            LOG("[PGUP]\n");
            page_up (currentBuffer);
            break;
         case GUL_NPAGE:
            LOG("[PGDN]\n");
            page_down (currentBuffer);
            break;
         case GUL_INSERT:
            LOG("[INS]\n");
            break;

         case GUL_NO_EVENT:
            break;
         default:
            LOG("Not a GUL_NO_EVENT ... default PANIC! :/ \n");
            break;
      }

      display (currentBuffer);
   }

   free(fileName);
   return(0);
}

static void finish(int sig)
{
   pluginFinish();

   exit(0);
}

/* Local Variables:
 * mode: C;
 * c-file-style: ellemtel;
 * indent-tabs-mode: nil;
 * End:
 */
