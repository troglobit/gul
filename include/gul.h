#ifndef GUL_H_
#define GUL_H_

#include "config.h"
#include "editor.h"

#if   defined NCURSES_PLUGIN
typedef int gulchar;
#elif defined SLANG_PLUGIN
typedef int gulchar;
#else
typedef unsigned char gulchar;
#endif

void      trySave(buffer_t *buf);
buffer_t *tryLoad(buffer_t *buf);

#endif /* GUL_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
