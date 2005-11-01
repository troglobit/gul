#ifndef ORIGINAL_H
#define ORIGINAL_H

/* Well, a "line" in this essence isn't really a line in the sense
 * that we're all perhaps used to, but the maximum size of a node ... :)
 */
#define MAX_LINE_LENGTH 3 * (2 * sizeof(struct dllSt *) + 2 * sizeof(int))

typedef struct dllSt{
   struct dllSt  *previous;
   struct dllSt  *next;
   int           begin;
   int           end;
   char          line[MAX_LINE_LENGTH];
}text;

/* Proposed new text-type.
 * typdef struct dllSt{
 *    struct dllSt  *previous;
 *    struct dllSt  *next;
 *    int           begin;
 *    int           end;
 *    char          *line;      En fysisk rad == nod.
 * }text;
 */

#endif /* ORIGINAL_H */
