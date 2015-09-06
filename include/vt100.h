/* vt100.h - vt100 plugin for the Gul Editor.
 *
 * Copyright (c) 2004-2015  Joachim Nilsson <troglobit@gmail.com>
 * Copyright (c) 1998-2000  Joachim Nilsson, Jakob Eriksson, Anders Bornäs
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *** Description ********************************************************
 * 7 july 1999  - Taken from the "ncurses.h" file. Jakov <jakob@quicknet.se>
 * 8 july 1999  - File splitted into 2 files; one for keyboard, one for screen.
 *
 */

#ifndef VT100_H_
#define VT100_H_

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

#endif /* VT100_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
