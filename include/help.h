/* help.h - Help routines
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
#ifndef HELP_H_
#define HELP_H_

#include "config.h"

#if   defined (WORDSTAR_KEYS)
#include "wordstar.h"
#elif defined (EMACS_KEYS)
#include "emacs.h"
#else       /* MINIMAL_KEYS */

char help_filename[] = "**HELP BUFFER**";
char help_buffer[] = "\
Nyfiken Gul?\n\
================================================================================\n\
F1 - This help                    | C-@ - Set mark (C-space)                    \n\
F2 - Save current buffer          | C-w - Cut text                              \n\
F3 - Load file to current buffer  | C-y - Paste                                 \n\
F4 -                              | C-k - Cut line(s)                           \n\
F5 -                              | C-u - Uncut line(s)                         \n\
F6 -                              |                                             \n\
F7 - Find string                  | C-f - Find string                           \n\
F8 -                              | C-s - I-search                              \n\
F9 -                              | C-r - Reversed I-search                     \n\
F0 - Quit                         | C-g - Goto line                             \n\
================================================================================\n";

text_t help_core = {
	help_buffer, help_buffer,
	NULL, NULL,
	0,
	sizeof(help_buffer), 0
};


buffer_t help = {
	help_filename,
	0,
	{0, 0, 0, 0, 0, 0,
	 &help_core,
	 "", ""}
	,

	{
	 help_buffer, help_buffer,
	 NULL, NULL,
	 0,
	 sizeof(help_buffer), 0}
	,

	NULL, NULL
};

#endif /* MINIMAL_KEYS */
#endif /* HELP_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
