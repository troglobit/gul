/* plugin.c - Toolkit plugin interface           vim:set ts=3 sw=3 ruler:
 *
 * Joachim Nilsson <crash@vmlinux.org>
 *
 */

#include <stdio.h>
#include "plugin.h"
#include "memory.h"

/*
 * The virtual screen
 * ==================
 *
 * The virtual screen is a sort of a middleware between a real screen and
 * the editor core. The core uses the primitives set up to write so called
 * "pixchars" (short for "character pixels") to the virtual screen.
 *
 * The current screen plugin uses the defined primitives to read from this
 * screen and put the contents on the physical screen.
 *
 * The idea is to be plugin independant and to be able to apply in more cool
 * features on the virtual screen before smackin' it all on to the real screen.
 * Try to imagine a syntax highlighting type thingy for instance... :-)
 */
static virtualScreen currentScreen;

/*** createScreen() *****************************************************
 * Your basic contructor type thingy... :)
 ************************************************************************
 */
int createScreen(void)
{
  // Endast när det verkligen *är* en konstruktor... annars så segfault! :)
  //  if (currentScreen.pixcharArray)
  //    free (currentScreen.pixcharArray);

  /* Initialize the keyboard and screen plugin */
  initPlugin ();

  /* OLD (?) init of plugin...
   * screenPluginInit();
   * keyboardPluginInit();
   */

  /* Check if the physical screen has been altered/resized. Some
   * plugins need this to reallocate their own buffers and
   * reinitialize them. Therefore we send pointers to the old
   * size and hope the plugin does its own bit.
   */
  screenPluginGetMaxYX(&(currentScreen.maxY), &(currentScreen.maxX));
  currentScreen.pixcharArray=
    allocate((currentScreen.maxX) * (currentScreen.maxY), "screen::createScreen()");

  currentScreen.dirtyFlag= 0;
  currentScreen.hpos= 0;

  return 0;
}

void getScreenMaxYX(int *maxy, int *maxx)
{
  if (maxy == NULL || maxx == NULL)
    return;

  if (!currentScreen.pixcharArray)
    {
      *maxx= 0;
      *maxy= 0;
    }
  else
    {
      *maxx= currentScreen.maxX;
      *maxy= currentScreen.maxY;
    }
}


/*** resizeScreen() *****************************************************
 * Skapar ny virtuell skärm med samma storlek som den fysiska skärmen och
 * kopierar över all synlig text till den nya.
 ************************************************************************
 */
static int resizeScreen(int newMaxX, int newMaxY)
{
  char *newPixcharArray;
  int  x, y;

  newPixcharArray= (char *)allocate(newMaxX * newMaxY, "resizeScreen()");

  /* Kopiera så mycket som möjligt till den nya arrayen */
  for (y= 0;
       y < (newMaxY > currentScreen.maxY ? currentScreen.maxY : newMaxY);
       y++)
    {
      for (x= 0;
           y < (newMaxX > currentScreen.maxX ? currentScreen.maxX : newMaxX);
           x++)
        {
          newPixcharArray[y * currentScreen.maxX + x]=
            currentScreen.pixcharArray[y * currentScreen.maxX + x];
        }
    }
  /* Kritiskt område... :( */
  free(currentScreen.pixcharArray);
  currentScreen.pixcharArray= newPixcharArray;
  currentScreen.maxX= newMaxX;
  currentScreen.maxY= newMaxY;

  /* Nu ser ju förmodligen inte skärmen klok ut så man skulle behöva
   * rita om den...
   */
  currentScreen.dirtyFlag= 1;

  return 0;
}

int screenIsDirty(void)
{
  return currentScreen.dirtyFlag;
}


/*** isChanged() ********************************************************
 * Den här är tom jag emot upplägget i, men jag gjorde så här som ett
 * första test. resizeScreen() kanske inte eg. borde ligga här eftersom
 * funktionen nog bara borde svara på frågan och inte GÖRA något...
 * ... men men. :)
 ************************************************************************
 */
int screenIsChanged(void)
{
  int newMaxX, newMaxY;

  /* Check if the physical screen has been altered/resized. Some
   * plugins need this to reallocate their own buffers and
   * reinitialize them. Therefore we send pointers to the old
   * size and hope the plugin does its own bit.
   */
  screenPluginGetMaxYX(&newMaxY, &newMaxX);

  if(newMaxX != currentScreen.maxX || newMaxY != currentScreen.maxY)
    {
      resizeScreen(newMaxX, newMaxY);

      return 1;
    }

  return 0;
}


void screenUpdate(void)
{
  screenPluginUpdate();
}

void screenPositionCursor(int x, int y)
{
  screenPluginPositionCursor(x, y);
}

/* Scroll the view right ... */
int panRight(int steps)
{
  currentScreen.hpos+= steps;

  return currentScreen.hpos;
}

/* Scroll the view left ... */
int panLeft(int steps)
{
  currentScreen.hpos-= steps;
  if (currentScreen.hpos < 0)
    {
      currentScreen.hpos= 0;
    }

  return currentScreen.hpos;
}

/* Scroll the view to the leftmost position ... */
void panHome(void)
{
  currentScreen.hpos= 0;
}


int putPixchar(int x, int y, char ch)
{
  if (!currentScreen.pixcharArray)
    {
      return 1;
    }

  if (y < currentScreen.maxY)
    {
      if (x < currentScreen.maxX)
        {
          currentScreen.pixcharArray[y*currentScreen.maxX + x]= ch;
          currentScreen.dirtyFlag= 1;
        }
    }

  return 0;
}

char getPixchar(int x, int y)
{
  if (!currentScreen.pixcharArray)
    {
      return 0;
    }

  if (x >= currentScreen.maxX || y >= currentScreen.maxY)
    {
      return 0;
    }

  return currentScreen.pixcharArray[y * currentScreen.maxX + x];
}

void screenDebugDisplay(void)
{
  int x, y;

  LOG("Debug display... [%d, %d]\n",
          currentScreen.maxX,
          currentScreen.maxY);
  for (y= 0; y < currentScreen.maxY; y++)
    {
      for (x= 0; x < currentScreen.maxX; x++)
        {
          if ('\0' != getPixchar(x, y))
            {
              LOG("%c", getPixchar(x, y));
            }
        }
      LOG("\n");
    }
}

keyevent_t read_key(void)
{
  return plugin_read_key();
}

char *read_string (int x, int y)
{
  return plugin_read_string (x, y);
}





/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
