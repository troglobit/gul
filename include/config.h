/* config.h - Handles all program setup */
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Include configure settings */
#include "settings.h"

#ifdef HAVE_FEATURES_H
#include <features.h>
#endif

/* Size of editor ruler */
#define RULER_WIDTH 80

/* Size of status/io window */
#define STATUS_WIDTH RULER_WIDTH

/* Tabs are displayed as %TAB_WIDTH spaces */
#define TAB_WIDTH   8


/*** Debug messages to stderr? *****************************************/
#ifdef DEBUG
#define LOG(args...) \
  fprintf (stderr, args)
#else
#define LOG(args...)
#endif

/*** Frontend **********************************************************/
/* NCurses for screen handling and keyboard input. */
#define NCURSES_PLUGIN

/*
 *#define VT100KEY_PLUGIN
 *#define VT100SCR_PLUGIN
 */

/* SLang plugin for screen and keyboard */
/* #define SLANG_PLUGIN */

/*** Backend ***********************************************************
 * Comment out the ones that are not to be used.
 * Only one backend is allowed, this is important since the result
 * otherwise is quite undefined. :P
 ***********************************************************************/

/* The first lame editor core. */
/* #define DLL_MODE */

/* Yet Another Editor-core Model /Crash */
#define BUFFER_GAP_MODE

#include "gul.h"


#endif  /* __CONFIG_H__ */


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */