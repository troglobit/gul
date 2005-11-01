/*
 * vt100.h
 *
 * Definition translations ...
 * 7 july 1999  - Taken from the "ncurses.h" file. Jakov <jakob@quicknet.se>
 * 8 july 1999  - File splitted into 2 files; one for keyboard, one for screen.
 *
 */

#ifndef __VT100_H__
#define __VT100_H__

/*
                Vt52    ANSI and Cursor Key modes
Arrow Key       Mode    Reset           Set
---------------------------------------------------
Up              Esc A   Esc [ A         Esc O A
Down            Esc B   Esc [ B         Esc O B
Right           Esc C   Esc [ C         Esc O C
Left            Esc D   Esc [ D         Esc O D

We will care for "Esc 0 a"
                  where a can be any of A, B, C or D.
*/

#define VT100_INKEY_UP    'A'
#define VT100_INKEY_DOWN  'B'
#define VT100_INKEY_RIGHT 'C'
#define VT100_INKEY_LEFT  'D'

#define GUL_NO_EVENT        0
#define GUL_DATA            1
#define GUL_UP              2
#define GUL_DOWN            3
#define GUL_LEFT            4
#define GUL_RIGHT           5
#define GUL_BACKSPACE       6
#define GUL_DELETE          7
#define GUL_INSERT          8
#define GUL_HOME            9
#define GUL_END            10
#define GUL_PPAGE          11
#define GUL_NPAGE          12
#define GUL_F1             13
#define GUL_F2             14
#define GUL_F3             15
#define GUL_F4             16
#define GUL_F5             17
#define GUL_F6             18
#define GUL_F7             19
#define GUL_F8             20
#define GUL_F9             21
#define GUL_F10            22
#define GUL_F11            23
#define GUL_F12            24

#endif  /* __VT100_H__ */


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
