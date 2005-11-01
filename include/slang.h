/*
 * ncurses.h
 *
 * Definition translations ...
 * 23/4 1999 Crash <joachim.nilsson@mds.mdh.se>
 *
 */
#ifndef __SLANG_H__
#define __SLANG_H__

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


#endif /* __SLANG_H__ */


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
