#ifndef __DLL_H__
#define __DLL_H__

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

typedef struct _text {
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

#endif				/* __DLL_H__ */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
