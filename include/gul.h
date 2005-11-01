#ifndef GUL_H
#define GUL_H


#if   defined NCURSES_PLUGIN
typedef int gulchar;
#elif defined SLANG_PLUGIN
typedef int gulchar;
#else
typedef unsigned char gulchar;
#endif /* PLUGINS */

#endif /* GUL_H */
