/* slang.c                                       vim:set ts=3 sw=3 ruler:
 *
 * S-Lang plugin for the Gul Editor.
 * plugin_init() is the central starting point of any control plugin.
 *
 * 6/9 1999 - Joachim Nilsson <crash@vmlinux.org>
 *
 */
#include "config.h"
#ifdef SLANG_PLUGIN

#include <slang.h>
#include "plugin.h"

#define SCREEN_RESIZED(max_row, max_col)                                      \
   (((max_row) != SLtt_Screen_Rows || (max_col) != SLtt_Screen_Cols) ? 1 : 0)

/**
 * screen_plugin_init():
 *
 * Initiates the screen and the various SLang variables.
 */
void screen_plugin_init(void)
{
	if (SLsmg_init_smg() == -1) {
		LOG("S-Lang could not properly allocate the screen.\
Sorry, exiting!\n");
		exit(-1);
	} else {
		LOG("screen_plugin_init() - S-Lang succeeded.\n");
	}
}

/**
 * screen_plugin_exit():
 *
 * Call this when quitting from the editor.
 */
void screen_plugin_exit(void)
{
	SLsmg_reset_smg();
}

/**
 * screen_plugin_get_dim():
 * @max_row: Pointer to return maximum no. of columns in.
 * @max_col: Pointer to return maximum no. or rows in.
 *
 * Returns the physical screen size.
 */
void screen_plugin_get_dim(int *max_row, int *max_col)
{
	static int init = 1;

	if (init) {
		*max_row = SLtt_Screen_Rows;
		*max_col = SLtt_Screen_Cols;
		init = 0;
	} else {
		*max_row = SLtt_Screen_Rows;
		*max_col = SLtt_Screen_Cols;

		if (SCREEN_RESIZED(*max_row, *max_col)) {
			/* Need to readjust the SLang screen */
			SLsmg_reset_smg();
			if (SLsmg_init_smg() == -1) {
				LOG("S-Lang could not properly reallocate the screen. Sorry!\n");
				exit(-1);
			}
		}
	}
}


/**
 * screen_plugin_set_cursor():
 * @x: Column
 * @y: Row
 *
 * Positions the cursor
 */
void screen_plugin_set_cursor(int x, int y)
{
	/* Move to [row, col] */
	SLsmg_gotorc(y, x);
	LOG("screen_plugin_set_cursor (%d,%d)\n", y, x);
	SLsmg_refresh();
}


/**
 * screen_plugin_update() - 
 *
 * Reads from the frame-buffer/virtual-screen and updates the physical
 * screen accordingly.
 */
void screen_plugin_update(void)
{
	int virtualXmax, virtualYmax;
//  int stdscrXmax, stdscrYmax;
	int x, y;
	char ch;

	LOG("screen_plugin_update()");

	/* Just in case, check that there have not been any resize events. */
	screen_get_dim(&virtualYmax, &virtualXmax);

	LOG(" - [%d, %d]\n", virtualXmax, virtualYmax);
	LOG("Done row: ");

	for (y = 0; y < virtualYmax; y++) {
		SLsmg_gotorc(y, 0);
		for (x = 0; x < virtualXmax; x++) {
			ch = screen_get_pixchar(x, y);

			if (0 == ch)
				SLsmg_write_char(' ');
			else
				SLsmg_write_char(ch);
		}
		LOG("%d, ", y);
	}

	SLsmg_touch_lines(0, virtualYmax);
	SLsmg_refresh();
}


/**
 * keyboard_plugin_init():
 *
 * Initiates the keyboard.
 */
void keyboard_plugin_init(void)
{
	SLtt_get_terminfo();

	if (-1 == SLkp_init()) {
		LOG("Unable to initialize the keyboard properly!\n");
		exit(-1);
	}

	/* ABORT_CHAR        -- -1 is to keep system predefined...
	 * use SLang_set_abort_signal to add a hook function.
	 * FLOW_CONTROL      -- TRUE/FALSE, enable/disable ^S & ^Q.
	 * OUTPUT_PROCESSING -- TRUE/FALSE, special processing?
	 */
	if (-1 == SLang_init_tty(-1, 0, 0)) {
		LOG("Unable to initialize the tty properly!\n");
		exit(-1);
	}

	/*  Slang_set_abort_signal (signal_sigint_handler); */
}

/**
 * keyboard_plugin_exit ()
 * Reset all special keyboard handling back to that previous.
 */
void keyboard_plugin_exit(void)
{
	SLang_reset_tty();
}

#define TIMEOUT 2
static int gul_emulate_ncurses_getch(void)
{
	int ch;

	while (0 == SLang_input_pending(1000))
		continue;

	ch = SLang_getkey();

	if (ch == 033) {	/* escape */
		if (0 == SLang_input_pending(TIMEOUT))
			return ch;
	}

	SLang_ungetkey(ch);

	return SLkp_getkey();
}


/**
 * plugin_read_key():
 *
 * Checks for waiting input, does NOT wait for input!
 *
 * Returns: The char waiting or zero (0) if none exists.
 */
keyevent_t plugin_read_key(void)
{
	int ch;

	keyevent_t input = { GUL_NO_EVENT, GUL_NO_DATA };

	ch = gul_emulate_ncurses_getch();
	ch = (SL_KEY_ERR == ch ? 0 : ch);

	if (ch > 255) {
		input.event = SLANG2GUL(ch);
		input.keydata = GUL_NO_DATA;
	} else {
		input.event = GUL_DATA;
		input.keydata = (13 == ch ? 10 : ch);
	}

	return input;
}

/**
 * plugin_init():
 *
 * Initates the control.
 */
void plugin_init(void)
{
	keyboard_plugin_init();
	screen_plugin_init();
}


/**
 * plugin_exit ():
 *
 * Releases all memory and resets the screen and keyboard back to normal.
 */
void plugin_exit(void)
{
	keyboard_plugin_exit();
	screen_plugin_exit();
}


#endif				/* SLANG_PLUGIN */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
