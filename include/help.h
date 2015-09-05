#ifndef __HELP_H__
#define __HELP_H__

#include "config.h"

#if defined (WORDSTAR_KEYS)
#include "wordstar.h"
#elif defined (EMACS_KEYS)
#include "emacs.h"
#else				/* MINIMAL_KEYS */

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

#endif				/* Key binding alternatives */
#endif				/* __HELP_H__ */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
