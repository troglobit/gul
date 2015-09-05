#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "editor.h"

void display        (buffer_t *currentp);
int  display_status (buffer_t *currentp, char *message);
void popUp_OK       (char *message);

#endif /* DISPLAY_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
