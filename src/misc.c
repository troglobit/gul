/* misc.c
 * Miscellaneous routines that make life a whole lot easier!
 * Most of these are stringhandling routines that have no
 * counterpart in string.h or somewhere else.
 *
 * Joachim Nilsson 980805
 */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *strnDup(char *src, int n)
{
   char oldchar = src[n];
   char *dest;

   src[n] = '\0';
   if(NULL == (dest = strdup(src))){
      fprintf(stderr, "ERROR: strndup(), out of memory...\n");
      exit(EXIT_FAILURE);
   }
   src[n] = oldchar;

   return(dest);
}

/* Str�ngduplicerare, precis som strdup() men *med* felhantering s.a. den
 * returnerade pekaren verkligen inte �r NULL!
 */
char *strDup(char *src)
{
   char *dest = NULL;

   if(src != NULL){
      if(NULL ==(dest = strdup(src))){
         LOG("ERROR: strDup() out of memory, aborting.\n");
         exit(EXIT_FAILURE);
      }
   }else{
      LOG("ERROR: strDup() Couldn't find a string to duplicate. Aborting.\n");
      exit(EXIT_FAILURE);
   }

   return(dest);
}


/* char *strCat(s1, s2) - Str�ngkonkatenator.
 * Sl�r ihop s1 och s2 till en str�ng och returnerar denna.
 * Allokerar minne och kontrollerar pekare och allt s�nt ...
 */
char *strCat(char *s1, char *s2)
{
   int  totalLength;
   char *s;

   if (NULL == s1)
      return(strDup(s2));
   else if (NULL == s2)
      return(strDup(s1));
   else{
      totalLength= strlen(s1) + strlen(s2) + 2;
/*      if (NULL == (s= realloc(s1, totalLength))){
 *       LOG("ERROR: strCat() out of memory!\n       Aborting.\n");
 *       exit(EXIT_FAILURE);
 *    }
 *    return(strcat(s, s2));
 */
      if (NULL == (s= malloc(totalLength))){
         LOG("ERROR: strCat() out of memory!\n       Aborting.\n");
         exit(EXIT_FAILURE);
      }
      sprintf(s, "%s%s", s1, s2);
      return(s);
   }
}


/*
 * strLen()
 * Letar reda p� "slutet" p� str�ngen 'string'. S�ker maximalt igenom
 * 'maxLen' tecken i jakten p� 'endChar'.
 *
 * Om man anger 'maxLen' till noll s� s�ker strlen() tills den hittar
 * 'endChar' eller '\0'. Vilket den nu hittar f�rst.
 *** OBS detta �r ingen s�ker metod eftersom den KAN g� utanf�r!!! ***
 *
 * Returnerar 'maxLen' om 'endChar' inte �terfinns i 'string'.
 *
 */
int strLen(char *string, int maxLen, char endChar)
{
   int counter;

   if (maxLen){
      for (counter= 0; counter < maxLen; counter++){
         if (endChar == string[counter])
            return counter;
      }
   }else{
      for (counter= 0; 1; counter++){
         if (endChar == string[counter] ||
             '\0'    == string[counter])
            return counter;
      }
   }



   return maxLen;
}
