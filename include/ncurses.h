/*
 * ncurses.h
 *
 * Definition translations ...
 * 23/4 1999 Crash <joachim.nilsson@mds.mdh.se>
 *
 */
#ifndef NCURSES_H
#define NCURSES_H

#include <curses.h>

/* Event */
#define GUL_NO_EVENT  0
#define GUL_DATA      (KEY_MAX + 1) /* Bogus on ncurses */
#define GUL_NO_DATA   (KEY_MAX + 2) /* Bogus on ncurses */

/* Keycodes */
#define GUL_UP        KEY_UP
#define GUL_DOWN      KEY_DOWN
#define GUL_LEFT      KEY_LEFT
#define GUL_RIGHT     KEY_RIGHT
#define GUL_BACKSPACE KEY_BACKSPACE
#define GUL_DELETE    KEY_DC
#define GUL_INSERT    KEY_IC
#define GUL_TAB       KEY_STAB
#define GUL_HOME      KEY_HOME
#define GUL_END       KEY_END
#define GUL_PPAGE     KEY_PPAGE
#define GUL_NPAGE     KEY_NPAGE
#define GUL_ENTER     KEY_ENTER
#define GUL_F1        KEY_F(1)
#define GUL_F2        KEY_F(2)
#define GUL_F3        KEY_F(3)
#define GUL_F4        KEY_F(4)
#define GUL_F5        KEY_F(5)
#define GUL_F6        KEY_F(6)
#define GUL_F7        KEY_F(7)
#define GUL_F8        KEY_F(8)
#define GUL_F9        KEY_F(9)
#define GUL_F10       KEY_F(10)
#define GUL_F11       KEY_F(11)
#define GUL_F12       KEY_F(12)


#endif /* NCURSES_H */
