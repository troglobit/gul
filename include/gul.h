#ifndef __GUL_H__
#define __GUL_H__

#include "config.h"
#include "editor.h"


#if   defined NCURSES_PLUGIN
typedef int gulchar;
#elif defined SLANG_PLUGIN
typedef int gulchar;
#else
typedef unsigned char gulchar;
#endif				/* PLUGINS */

void trySave(buffer_t * buf);
buffer_t *tryLoad(buffer_t * buf);


#endif				/* __GUL_H__ */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
