/* config.h - Handles all program setup
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
#ifndef CONFIG_H_
#define CONFIG_H_

/* Include configure settings */
#include "settings.h"

#ifdef HAVE_FEATURES_H
#include <features.h>
#endif

/* Size of editor ruler */
#define RULER_WIDTH 80

/* Size of status/io window */
#define STATUS_WIDTH RULER_WIDTH

/* Tabs are displayed as %TAB_WIDTH spaces */
#define TAB_WIDTH   8


/*** Debug messages to stderr? *****************************************/
#ifdef DEBUG
#define LOG(args...)				\
 {						\
	FILE *fp = fopen("gul.log", "a");	\
	if (fp) {				\
		fprintf(fp, args);		\
		fclose(fp);			\
	}					\
 }
#else
#define LOG(args...)
#endif

/*** Frontend **********************************************************/
/* NCurses for screen handling and keyboard input. */
#define NCURSES_PLUGIN

/*
 *#define VT100KEY_PLUGIN
 *#define VT100SCR_PLUGIN
 */

/* SLang plugin for screen and keyboard */
//#define SLANG_PLUGIN

/*** Backend ***********************************************************
 * Comment out the ones that are not to be used.
 * Only one backend is allowed, this is important since the result
 * otherwise is quite undefined. :P
 ***********************************************************************/

/* The first lame editor core. */
/* #define DLL_MODE */

/* Yet Another Editor-core Model /Crash */
#define BUFFER_GAP_MODE

#include "gul.h"

#endif /* CONFIG_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
