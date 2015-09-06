/* buffer_gap.h - Yet Another Editor core Impl.
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
#include "config.h"

#ifndef BUFFER_GAP_H_
#define BUFFER_GAP_H_

#define GAP_SIZE 1024

/* @buffer holds the file data
 * @gap is an empty void (cursor) that can be moved inside @buffer
 * @point end pointer when marking
 * @mark start pointer when marking, set with C-@ or C-<space>
 * @buffer_size == file size
 * @gap_size == how much more can be fed to @buffer before realloc.
 */
typedef struct {
	char *buffer;
	char *gap;
	char *point, *mark;
	unsigned int position;	/* byte position in file/buffer */
	unsigned int buffer_size, gap_size;
} text_t;

#endif /* BUFFER_GAP_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
