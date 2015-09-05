/*
 * ncurses.h
 *
 * Definition translations ...
 * 23/4 1999 Crash <joachim.nilsson@mds.mdh.se>
 *
 */
#ifndef GUL_NCURSES_H_
#define GUL_NCURSES_H_

#include <curses.h>

/* Event */
#define GUL_NO_EVENT  0
#define GUL_DATA      (KEY_MAX + 1)	/* Bogus on ncurses */
#define GUL_NO_DATA   (KEY_MAX + 2)	/* Bogus on ncurses */

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

/* Control + key */
#define GUL_C_SPACE   0		/* NUL */
#define GUL_C_a       1		/* SOH -  */
#define GUL_C_b       2		/* STX -  */
#define GUL_C_c       3		/* ETX - Break (default) or copy? */
#define GUL_C_d       4		/* EOT */
#define GUL_C_e       5		/* ENQ -  */
#define GUL_C_f       6		/* ACK - Find  */
#define GUL_C_g       7		/* BEL - Abort or Find next? */
#define GUL_C_h       8		/* BS  - Backspace or Help? */
#define GUL_C_i       9		/* HT */
#define GUL_C_j       10	/* LF - Newline or Enter key ? UNIX Linefeed */
#define GUL_C_k       11	/* VT */
#define GUL_C_l       12	/* FF - Refresh */
#define GUL_C_m       13	/* CR - Newline or Enter key? */
#define GUL_C_n       14	/* SO - Next line */
#define GUL_C_o       15	/* SI - Open file */
#define GUL_C_p       16	/* DLE - Prev. line */
#define GUL_C_q       17	/* DC1 -  */
#define GUL_C_r       18	/* DC2 - Reversed i-search */
#define GUL_C_s       19	/* DC3 - Forward i-search */
#define GUL_C_t       20	/* DC4 -  */
#define GUL_C_u       21	/* NAK - undo? */
#define GUL_C_v       22	/* SYN - paste? */
#define GUL_C_w       23	/* ETB - cut? */
#define GUL_C_x       24	/* CAN - cut? */
#define GUL_C_y       25	/* EM - cut to eol? */
#define GUL_C_z       26	/* SUB - suspend (default) or undo? */
#define GUL_C_3       27	/* ESC */
#define GUL_C_4       28	/* FS - NCurses quits immediately! */
#define GUL_C_5       29	/* GS - */
#define GUL_C_6       30	/* RS - */
#define GUL_C_7       31	/* US - Undo? */
#define GUL_C__       31	/* US - Undo? */
#define GUL_C_8       KEY_BACKSPACE

#endif /* GUL_NCURSES_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
