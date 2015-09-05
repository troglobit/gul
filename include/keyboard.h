#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "config.h"
#include "editor.h"

/* Another keyboard plugin (keybinding scheme)
 * must implement this interface.
 *
 * See wordstar.c for an example.
 */

int keyboard_loop(buffer_t * currentBuffer);

#endif				/* __KEYBOARD_H__ */
