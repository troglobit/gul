/*
 * plugin.h
 * REadmigjoachim
 * Interface editor CORE <----> screen - - - > plugin
 * JAkov 17/4 -99
 * 12 july 1999  - Letting the keyboard return a structure dividing
 *                 keyboard commands and keyboard data.
 *
 * Translations made from my first plugin draft. /Crash
 */
#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "config.h"

#ifdef NCURSES_PLUGIN
#include "ncurses.h"
#endif

#ifdef VT100KEY_PLUGIN
#include "vt100key.h"
#endif
#ifdef VT100SCR_PLUGIN
#include "vt100scr.h"
#endif

#ifdef SLANG_PLUGIN		/* This and ncurses should be divided in 2 files too, */
#include "slang.h"		/* just to up the modularity. */
#endif

/* The virtual screen definition */
typedef struct _virtualScreen {
	/* Flaggor */
	int dirtyFlag;
	int needsRedraw;
	int maxX, maxY;
	int currentX, currentY;
	int hpos;
	char *pixcharArray;
} virtualScreen;

/* Createscreen creates a screeen. Spectravideo equivalent: initialize to
   proper screenmode, ie SCREEN 0 */
int createScreen(void);


/* Spectravideo equiv: macro that defines maxy and maxx to 24 and 40, respectively. */
void getScreenMaxYX(int *maxy, int *maxx);

/* We need it to know when to call the SCREEN provided redraw() function. */
/* The redraw() function links against our must-be-provided "text-generator"-
 * function.
 */
int isScreenChanged(void);

void screenUpdate(void);
void screenPositionCursor(int x, int y);

int  panRight (int steps);
int  panLeft  (int steps);
void panHome  (void);

int  putPixchar(int x, int y, char ch /*, int depth ?? */ );
char getPixchar(int x, int y);

void screenDebugDisplay(void);

typedef struct keyevent_str {
	gulchar event;
	gulchar keydata;
} keyevent_t;

void        keyboardInit (void);
keyevent_t  read_key     (void);
char       *read_string  (int x, int y);



/***********************************************************************
 * Rutiner som respektive skärmhanterar-plugin måste tillhandahålla
 ***********************************************************************
 */
/* Call this plugin supplied function to init both screen and keyboard.
 * important when using the NCurses plugin since the initialization
 * order IS important! /Crash
 */
void initPlugin(void);

/* This plugin supplied function is used to clean up afterwards when
 * exiting the application.
 */
void pluginFinish(void);

/* Do NOT use these old plugin init routines ... they are evil!
 * They are only left here as a warning and for "backwards incompatibility"
 * until we clear all this old stuff out. /Crash
 * XXX - Clear this shit out!
 */
/* Div. inställningar och sånt... */
void screenPluginInit(void);
void keyboardPluginInit(void);

/* Call this when quitting from editor. */
void screenPluginFinish(void);
void keyboardPluginFinish(void);

/* Positions the cursor */
void screenPluginPositionCursor(int x, int y);

/* Ska returnera skärmens storlek i just det ögonblicket. För ev. resize */
void screenPluginGetMaxYX(int *maxY, int *maxX);

/* Läser ur framebuffern/virtuella skärmen och skriver ut på skärmen */
void screenPluginUpdate(void);


/***********************************************************************
 * Rutiner som respektive tangenbords-plugin måste tillhandahålla
 ***********************************************************************
 */

/* Initierar tangentbordspluginet */

/* Returnerar GUL_* när det finns ett event att hantera. */
keyevent_t  plugin_read_key   (void);
char       *plugin_read_string(int x, int y);

#endif /* PLUGIN_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
