#include "config.h"

#include <stdio.h>
#include <stdlib.h>


void *allocate(size_t memSize, char *callingFunc)
{
   void *ptr;

   if (memSize < 1){
      if (NULL != callingFunc)
         LOG("\nError, couldn't allocate %d bytes for %s\n",
                 memSize, callingFunc);
      else
         LOG("\nError, couldn't allocate %d bytes.\n",
                 memSize);

      exit(EXIT_FAILURE);
   }

   if (NULL == (ptr= calloc(1, memSize))){
      if (NULL != callingFunc)
         LOG("\nError, couldn't allocate %d bytes for %s\n",
                 memSize, callingFunc);
      else
         LOG("\nError, couldn't allocate %d bytes.\n",
                 memSize);

      exit(EXIT_FAILURE);
   }

   return (ptr);
}


/**
 * Local Variables:
 *  c-file-style: "ellemtel"
 *  indent-tabs-mode: nil
 * End:
 */
