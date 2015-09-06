/* dll.h - The dll GUL editor core.
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
#ifndef DLL_H_
#define DLL_H_

/* Well, a "line" in this essence isn't really a line in the sense
 * that we're all perhaps used to, but the maximum size of a node ... :)
 */
#define MAX_LINE_LENGTH 3 * (2 * sizeof(struct dllSt *) + 2 * sizeof(int))

typedef struct dllSt {
	struct dllSt *previous;
	struct dllSt *next;
	int begin;
	int end;
	char line[MAX_LINE_LENGTH];
} dll_t;

typedef struct {
	dll_t *stx;		/* Start of TeXt and list. */
	dll_t *node;		/* Current, where the cursor is */
} text_t;

/* Proposed new text-type.
 * typdef struct dllSt{
 *    struct dllSt  *previous;
 *    struct dllSt  *next;
 *    int           begin;
 *    int           end;
 *    char          *line;      En fysisk rad == nod.
 * }text;
 */

#endif /* DLL_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
