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
typedef struct {
	/* Flaggor */
	int dirty;
	int refresh;
	int max_col, max_row;
	int curr_col, curr_row;
	int hpos;
	char *pixchar;
} screen_virt_t;

/* Createscreen creates a screeen. Spectravideo equivalent: initialize to
   proper screenmode, ie SCREEN 0 */
int screen_create(void);


/* Spectravideo equiv: macro that defines maxy and maxx to 24 and 40, respectively. */
void screen_get_dim(int *maxy, int *maxx);

/* We need it to know when to call the SCREEN provided redraw() function. */
/* The redraw() function links against our must-be-provided "text-generator"-
 * function.
 */
int screen_changed(void);

void screen_update(void);
void screen_set_cursor(int x, int y);

int  screen_pan_right (int steps);
int  screen_pan_left  (int steps);
void screen_pan_home  (void);

int  screen_put_pixchar(int x, int y, char ch /*, int depth ?? */ );
char screen_get_pixchar(int x, int y);

void screen_debug(void);

typedef struct keyevent_str {
	gulchar event;
	gulchar keydata;
} keyevent_t;

void        keyboard_init (void);
keyevent_t  keyboard_event(void);
char       *keyboard_gets (int x, int y);



/***********************************************************************
 * Rutiner som respektive skärmhanterar-plugin måste tillhandahålla
 ***********************************************************************
 */
/* Call this plugin supplied function to init both screen and keyboard.
 * important when using the NCurses plugin since the initialization
 * order IS important! /Crash
 */
void plugin_init(void);

/* This plugin supplied function is used to clean up afterwards when
 * exiting the application.
 */
void plugin_exit(void);

/* Do NOT use these old plugin init routines ... they are evil!
 * They are only left here as a warning and for "backwards incompatibility"
 * until we clear all this old stuff out. /Crash
 * XXX - Clear this shit out!
 */
/* Div. inställningar och sånt... */
void screen_plugin_init(void);
void keyboard_plugin_init(void);

/* Call this when quitting from editor. */
void screen_plugin_exit(void);
void keyboard_plugin_exit(void);

/* Positions the cursor */
void screen_plugin_set_cursor(int x, int y);

/* Ska returnera skärmens storlek i just det ögonblicket. För ev. resize */
void screen_plugin_get_dim(int *max_row, int *max_col);

/* Läser ur framebuffern/virtuella skärmen och skriver ut på skärmen */
void screen_plugin_update(void);


/***********************************************************************
 * Rutiner som respektive tangenbords-plugin måste tillhandahålla
 ***********************************************************************
 */

/* Initierar tangentbordspluginet */

/* Returnerar GUL_* när det finns ett event att hantera. */
keyevent_t  plugin_read_key(void);
char       *plugin_read_str(int x, int y);

#endif /* PLUGIN_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
