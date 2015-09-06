/* wordstar.h - The WordStar compatible keybinding mode.
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
 */
#ifndef WORDSTAR_H_
#define WORDSTAR_H_

char help_filename[] = "**HELP BUFFER**";
char help_buffer[] = "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -     \n\
------------ The WordStar Cursor Diamond and Scrolling Square ------------          \n\
            .-------. .--------------.--------------.                               \n\
            | Quick*| | Line Up  _.-' `-._  Scrn Up |                               \n\
            |  ^Q   | |   ^W _.-'   ^E    `-._ ^R   |                               \n\
            `-------' |  _.-'     Line up     `-._  |                               \n\
                     _.-'                         `-._                              \n\
.----------.     _.-' Word    Char       Char    Word `-._                          \n\
| Home-row |  .-'      ^A      ^S   _|_   ^D      ^F      `-.                       \n\
| Ctrl key | `-._     Left    Left   |   Right   Right    _.-'                      \n\
`----------'     `-._                                 _.-'                          \n\
                   / `-._                         _.-'      ^ = Ctrl                \n\
     Cursor Control   |  `-._    Line Down    _.-'  |      ^X = Ctrl+x              \n\
                      |   ^Z `-._   ^X    _.-' ^C   |     ^Qx = Ctrl+q, then x      \n\
  Viewframe Control --| Line Dn  `-._ _.-'  Scrn Dn |     ^Qx = ^QX = ^Q^X          \n\
                      `--------------\"--------------'                              \n\
*\"Quick\" Functions:                                                               \n\
 ------ Cursor -------   ------- Viewframe ------   --- Miscellaneous ---           \n\
 ^Qe: top of screen    | ^Qw: auto-scroll up      | ^Qa: search & replace           \n\
 ^Qs: left edge        | ^Qz: auto-scroll down    | ^Qf: search                     \n\
 ^Qd: right edge       | ^Qr: jump to top of file | ^Qq: repeat next char           \n\
 ^Qx: bottom of screen | ^Qc: jump to end of file |      or command                 \n\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -     \n\
Left hand: All movement.    Middle: Risky functions.    Right hand: All others.     \n\
      .-----------------------.---------------.-------------------------------.     \n\
Quick | ========= Up ======== | == Delete === | ======= Menus & Misc ======== |     \n\
Funcs | Scr    Cursor    Scr  |               | Undo/          On-scrn  Print |     \n\
===== | 1 Ln    1 Ln    1 Scr |  Word   Line  | Cancel   Tab   Format   Ctrls |     \n\
.---. | .---.   .---.   .---. | .---.   .---. | .---.   .---.   .---.   .---. |     \n\
|^Q*| | |^W |   |^E |   |^R | | |^T |   |^Y | | |^U |   |^I**   |^O*|   |^P*| |     \n\
`---' | `---'   `---'   `---' | `---'   `---' | `---'   `---'   `---'   `---' |     \n\
 .----'                       `--.            `--.                            |     \n\
 | ======== Left/Right ========= | Char  Char Lf | Help    Block  Last Search |     \n\
 | .---Left----.   .---Right---. | .---.   .---. | .---.   .---.   .---. (or  |     \n\
 | |^A |   |^S |   |^D |   |^F | | |^G |   |^H** | |^J*|   |^K*|   |^L | srch |     \n\
 | `---'   `---'   `---'   `---' | `---'   `---' | `---'   `---'   `---' and  |     \n\
 | Word    Char     Char   Word  |        (Bksp) |        (\"File\")       repl)|   \n\
 `--.                       .----' - - - - - .---'            .---------------'     \n\
    | .---.   .---.   .---. | .---.   .---.  |.---.   .---.   | === Legend ===      \n\
    | |^Z |   |^X |   |^C | | |^V |   |^B |  ||^N |   |^M**   |    ^ = Ctrl         \n\
    | `---'   `---'   `---' | `---'   `---'  |`---'   `---'   | ==== Note ====      \n\
    |  Scr    Cursor   Scr  | Ins/  \"Box Up\" | Add   Add New  |  All commands     \n\
    |  1 Ln    1 Ln   1 Scr | Ovr-  (Reform) | New   Line and |  are case-          \n\
    |  ======= Down ======= | type  P'graph  | Line  Go Down  |  insensitive.       \n\
    `-----------------------`----------------'----------------'                     \n\
 *^Q ^O ^P ^J ^K ...: Prefix keys. Display menus showing available functions.       \n\
**^H ^I ^M .........: Universal editing keys. Defined in ASCII and Unicode.         \n\
  \"Quick\" Funcs ....: Long cursor jumps, search, search and replace, math....     \n\
  \"Block\" Funcs ....: Select, move, copy, save, quit, print, open....             \n\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -";

text_t help_core = {
	help_buffer, help_buffer,
	NULL, NULL, sizeof(help_buffer), 0
};

buffer_t help = {
	help_filename,
	0, 0,
	{0, 0, 0, 0, 0, 0,
	 &help_core,
	 "", ""}
	,

	{
	 help_buffer, help_buffer,
	 NULL, NULL, sizeof(help_buffer), 0}
	,

	NULL, NULL
};

#endif /* WORDSTAR_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
