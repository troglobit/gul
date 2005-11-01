#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "editor.h"

void display        (buffer_t *currentp);
int  display_status (buffer_t *currentp, char *message);
void popUp_OK (char *message);

#endif /* __DISPLAY_H__ */


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
