/* plugin.c - Toolkit plugin interface           vim:set ts=3 sw=3 ruler:
 *
 * Joachim Nilsson <crash@vmlinux.org>
 *
 */

#include <stdio.h>
#include <stdlib.h>

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
static screen_virt_t screen;

/*** screen_create() *****************************************************
 * Your basic contructor type thingy... :)
 ************************************************************************
 */
int screen_create(void)
{
	// Endast när det verkligen *är* en konstruktor... annars så segfault! :)
	//  if (screen.pixchar)
	//    free (screen.pixchar);

	/* Initialize the keyboard and screen plugin */
	plugin_init();

	/* OLD (?) init of plugin...
	 * screen_plugin_init();
	 * keyboard_plugin_init();
	 */

	/* Check if the physical screen has been altered/resized. Some
	 * plugins need this to reallocate their own buffers and
	 * reinitialize them. Therefore we send pointers to the old
	 * size and hope the plugin does its own bit.
	 */
	screen_plugin_get_dim(&screen.max_row, &screen.max_col);
	screen.pixchar = calloc(screen.max_col * screen.max_row, sizeof(char));
	if (!screen.pixchar) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	screen.dirty = 0;
	screen.hpos = 0;

	return 0;
}

void screen_get_dim(int *maxy, int *maxx)
{
	if (maxy == NULL || maxx == NULL)
		return;

	if (!screen.pixchar) {
		*maxx = 0;
		*maxy = 0;
	} else {
		*maxx = screen.max_col;
		*maxy = screen.max_row;
	}
}


/*** screen_resize() *****************************************************
 * Skapar ny virtuell skärm med samma storlek som den fysiska skärmen och
 * kopierar över all synlig text till den nya.
 ************************************************************************
 */
static int screen_resize(int col, int row)
{
	char *newPixcharArray;
	int x, y;

	newPixcharArray = calloc(col * row, sizeof(char));
	if (!newPixcharArray) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	/* Kopiera så mycket som möjligt till den nya arrayen */
	for (y = 0; y < (row > screen.max_row ? screen.max_row : row); y++) {
		for (x = 0; y < (col > screen.max_col ? screen.max_col : col); x++)
			newPixcharArray[y * screen.max_col + x] = screen.pixchar[y * screen.max_col + x];
	}

	/* Kritiskt område... :( */
	free(screen.pixchar);
	screen.pixchar = newPixcharArray;
	screen.max_col = col;
	screen.max_row = row;

	/*
	 * Nu ser ju förmodligen inte skärmen klok ut så man skulle behöva
	 * rita om den...
	 */
	screen.dirty = 1;

	return 0;
}

int screen_is_dirty(void)
{
	return screen.dirty;
}


int screen_changed(void)
{
	int col, row;

	/* Check if the physical screen has been altered/resized. Some
	 * plugins need this to reallocate their own buffers and
	 * reinitialize them. Therefore we send pointers to the old
	 * size and hope the plugin does its own bit.
	 */
	screen_plugin_get_dim(&row, &col);

	if (col != screen.max_col || row != screen.max_row) {
		screen_resize(col, row);

		return 1;
	}

	return 0;
}


void screen_update(void)
{
	screen_plugin_update();
}

void screen_set_cursor(int x, int y)
{
	screen_plugin_set_cursor(x, y);
}

/* Scroll the view right ... */
int screen_pan_right(int steps)
{
	screen.hpos += steps;

	return screen.hpos;
}

/* Scroll the view left ... */
int screen_pan_left(int steps)
{
	screen.hpos -= steps;
	if (screen.hpos < 0) {
		screen.hpos = 0;
	}

	return screen.hpos;
}

/* Scroll the view to the leftmost position ... */
void screen_pan_home(void)
{
	screen.hpos = 0;
}


int screen_put_pixchar(int x, int y, char ch)
{
	if (!screen.pixchar) {
		return 1;
	}

	if (y < screen.max_row) {
		if (x < screen.max_col) {
			screen.pixchar[y * screen.max_col + x] = ch;
			screen.dirty = 1;
		}
	}

	return 0;
}

char screen_get_pixchar(int x, int y)
{
	if (!screen.pixchar) {
		return 0;
	}

	if (x >= screen.max_col || y >= screen.max_row) {
		return 0;
	}

	return screen.pixchar[y * screen.max_col + x];
}

void screen_debug(void)
{
	int x, y;

	LOG("Debug display... [%d, %d]\n", screen.max_col, screen.max_row);
	for (y = 0; y < screen.max_row; y++) {
		for (x = 0; x < screen.max_col; x++) {
			if ('\0' != screen_get_pixchar(x, y)) {
				LOG("%c", screen_get_pixchar(x, y));
			}
		}
		LOG("\n");
	}
}

keyevent_t keyboard_event(void)
{
	return plugin_read_key();
}

char *keyboard_gets(int x, int y)
{
	return plugin_read_str(x, y);
}





/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
