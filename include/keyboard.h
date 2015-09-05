#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "config.h"
#include "editor.h"

/* Another keyboard plugin (keybinding scheme)
 * must implement this interface.
 *
 * See wordstar.c for an example.
 */

int keyboard_loop(buffer_t *currentBuffer);

#endif /* KEYBOARD_H_ */
