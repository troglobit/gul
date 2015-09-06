/* slang.c - S-Lang plugin for the Gul Editor.
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
 * 15/7 1999   Joachim Nilsson <joachim.nilsson@mds.mdh.se>
 * 2006-12-22  Joachim Nilsson <joachim.nilsson@vmlinux.org>
 */

#ifndef GUL_SLANG_H_
#define GUL_SLANG_H_

#include <slang.h>

/* Macro to transpose S-Lang keysyms to GUL */
#define SLANG2GUL(keysym) ((keysym) - 206)

/* Event */
#define GUL_NO_EVENT  0xFF
#define GUL_DATA      0xFE
#define GUL_NO_DATA   0xFD

/* Keycodes */
#define GUL_UP        SLANG2GUL(SL_KEY_UP)
#define GUL_DOWN      SLANG2GUL(SL_KEY_DOWN)
#define GUL_LEFT      SLANG2GUL(SL_KEY_LEFT)
#define GUL_RIGHT     SLANG2GUL(SL_KEY_RIGHT)
#define GUL_BACKSPACE SLANG2GUL(SL_KEY_BACKSPACE)
#define GUL_DELETE    SLANG2GUL(SL_KEY_DELETE)
#define GUL_INSERT    SLANG2GUL(SL_KEY_IC)
#define GUL_TAB       9
#define GUL_HOME      SLANG2GUL(SL_KEY_HOME)
#define GUL_END       SLANG2GUL(SL_KEY_END)
#define GUL_PPAGE     SLANG2GUL(SL_KEY_PPAGE)
#define GUL_NPAGE     SLANG2GUL(SL_KEY_NPAGE)
#define GUL_ENTER     SLANG2GUL(SL_KEY_ENTER)
#define GUL_F1        SLANG2GUL(SL_KEY_F(1))
#define GUL_F2        SLANG2GUL(SL_KEY_F(2))
#define GUL_F3        SLANG2GUL(SL_KEY_F(3))
#define GUL_F4        SLANG2GUL(SL_KEY_F(4))
#define GUL_F5        SLANG2GUL(SL_KEY_F(5))
#define GUL_F6        SLANG2GUL(SL_KEY_F(6))
#define GUL_F7        SLANG2GUL(SL_KEY_F(7))
#define GUL_F8        SLANG2GUL(SL_KEY_F(8))
#define GUL_F9        SLANG2GUL(SL_KEY_F(9))
#define GUL_F10       SLANG2GUL(SL_KEY_F(10))
#define GUL_F11       SLANG2GUL(SL_KEY_F(11))
#define GUL_F12       SLANG2GUL(SL_KEY_F(12))

#endif /* GUL_SLANG_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
