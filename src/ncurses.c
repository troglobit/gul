/* ncurses.c                                     vim:set ts=3 sw=3 ruler:
 *
 * NCurses-plugin for the Gul Editor.
 * This can be seen as rather simple "tutorial" on how to write other
 * plugins such as SLang & vt100 and others...
 *
 * 23/4 1999 - Crash <joachim.nilsson@mds.mdh.se>
 *
 */
#include "config.h"
#ifdef NCURSES_PLUGIN

#include <ncurses.h>
#include "plugin.h"

/* Div. inställningar och sånt... */
void screenPluginInit(void)
{
   initscr();
   nonl();
   scrollok(stdscr, FALSE);

//   attrset(WA_NORMAL);
//   attroff(A_REVERSE);
//   attroff(A_UNDERLINE);
//   attroff(A_BLINK);
}

/* Call this when quitting from editor. */
void screenPluginFinish(void)
{
   endwin();
}

/* Returnera skärmens storlek i just det ögonblicket. */
void screenPluginGetMaxYX(int *maxY, int *maxX)
{
   /* Sjuka getmaxyx är et MACRO! sjukt sjukt sjukt sjukt sjukt sjukt sjukt sjukt sjukt sjukt !!! */
   getmaxyx(stdscr, (*maxY), (*maxX));
}


/* Positions the cursor */
void screenPluginPositionCursor(int x, int y)
{
   move(y, x);
}


/* Läser ur framebuffern/virtuella skärmen och skriver ut på skärmen */
void screenPluginUpdate(void)
{
   int virtualXmax, virtualYmax;
   int stdscrXmax, stdscrYmax;
   int x, y;
   char ch;

   LOG("screenPluginUpdate()\n");

   /* Bara för "säkerhets" skull så kollar jag att fönstret inte resize:ats. */
   getScreenMaxYX(&virtualYmax, &virtualXmax);
//   getmaxyx(stdscr, stdscrYmax, stdscrXmax);
//   LOG("screenPluginUpdate() - virtualXmax= %d, virtualYmax= %d\n", virtualXmax, virtualYmax);

   /* Kanske är det onödigt, men vaddå ... :P */
//   for (y= 0; y < (virtualYmax > stdscrYmax ? stdscrYmax : virtualYmax); y++){
//      for (x= 0; y < (virtualXmax >  stdscrXmax ? stdscrXmax : virtualXmax); x++){
   move(0, 0);
   for (y= 0; y < virtualYmax; y++){
      for (x= 0; x < virtualXmax; x++){
         ch= getPixchar(x, y);

         if (0 == ch)
            addch(' ');
         else
            addch(ch);
      }
   }

   touchwin(stdscr);
   refresh();
}



/* Initierar tangentbordspluginet */
void keyboardPluginInit(void)
{
   keypad(stdscr, TRUE); /* Enable keypad/movement-keys */
   meta(stdscr, TRUE);   /* eight bit input */
   noecho();             /* No echo to screen. */
//   raw();                /* Unformatted input */
   cbreak();             /* Somewhat formatted, break/resize/others catched */
   //   nodelay(stdscr, TRUE);/* Non-blocking input! */
   nodelay(stdscr, FALSE);/* Blocking input! */
}

/*
 * keyboardPluginFinish ()
 * Reset all special keyboard handling back to that previous.
 */
void
keyboardPluginFinish (void)
{
/* Dummy */
}

/**
 * gul_plugin_event - Check for a key command.
 *
 * Checks for a key in the inbuffer, if
 * mo key available at that time returns
 * %GUL_NO_EVENT.
 *
 * Non-blocking call.
 */

keyevent gul_plugin_event(void)
{
   int ch = getch();
   keyevent input = {GUL_NO_EVENT, GUL_NO_DATA};

   ch = (ch == ERR ? GUL_NO_EVENT : ch);

   if (ch >= KEY_CODE_YES)
   {
      input.event   = ch;
      input.keydata = GUL_NO_DATA;
   }
   else
   {
      input.event   = GUL_DATA;
      input.keydata = (13 == ch ? 10 : ch);
   }

   return input;
}


/* Ganska viktig ty görs inte den ena eller den andra så kajkar det ur
 * rätt fint ...
 */
void initPlugin(void)
{
   screenPluginInit();
   keyboardPluginInit();
}

/**
 * pluginFinish:
 *
 * Releases all memory and resets the screen and keyboard back to normal.
 */
void
pluginFinish (void)
{
   keyboardPluginFinish ();
   screenPluginFinish ();
}

#endif /* NCURSES_PLUGIN */

/* Local Variables:
 * mode: C;
 * c-file-style: ellemtel;
 * indent-tabs-mode: nil;
 * End:
 */
