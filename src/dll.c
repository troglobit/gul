/* -*-C-*- vim:set ts=3 sw=3 ruler: */

/*** dll.c *********************************************************
 * The dll GUL editor core.
 *
 *** Description ********************************************************
 * The first implemented ideas for an editor model.
 * Dynamically Linked List of lines.
 *
 *** Authors ************************************************************
 * General:
 * Joachim "Crash Override" Nilsson <joachim.nilsson@mds.mdh.se>
 *
 * Movement & semantics:
 * Jakob "JAkov af Wallby" Eriksson <jakob.eriksson@quicknet.se>
 *
 ************************************************************************
 */
#include "config.h"

#ifdef DLL_MODE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "editor.h"
#include "dll.h"
#include "memory.h"


/*** Global variables **************************************************/
/* int posX, posY; */

/* These two following lines should be included in the buffer info
   instead, but I don't know what you think ...
 */
/*int cursorY, cursorX;
 *text *top;
 */

/*** Prototypes *********************************************************
 */
static text *insertNode(mark_t *currentp);
int calculateXposition(text *thisp, int i);
text *splitNode(mark_t *currentp);
text *deleteRightNode(mark_t *currentp);
void insertCommand(mark_t *currentp, int command);
mark_t *backupBufferInfo(mark_t *currentp);
void restoreBufferInfo(mark_t *backup_info, mark_t *currentp);
int getNextNode(mark_t *currentp);
int getPrevNode(mark_t *currentp);
int movetoNextChar(mark_t *currentp);
int movetoPrevChar(mark_t *currentp);
int atSTX(mark_t *currentp);
char getCurrentChar(mark_t *currentp);
int getCurrentColumn(mark_t *currentp);
int movetoEOL(mark_t *currentp);
int movetoBOL(mark_t *currentp);
int movetoSTX(mark_t *currentp);
int movetoCol(mark_t *currentp, int col);


void left(mark_t *currentp)
{
   if (movetoPrevChar(currentp)){
      if ('\n' == getCurrentChar(currentp)){
         currentp->scr.y--;
         currentp->scr.x= getCurrentColumn(currentp);
      }else{
         currentp->scr.x--;
      }
   }else{
//      beep(); /* Finns bara i curses!! */
   }
}

void right(mark_t *currentp)
{
   char previousChar;

   previousChar= getCurrentChar(currentp);

   if (movetoNextChar(currentp)){
      if ('\n' == previousChar){
         currentp->scr.y++;
         currentp->scr.x= 0;
      }else{
         currentp->scr.x++;
      }
   }else{
//      beep(); /* Finns bara i curses!! */
   }
}

void up(mark_t *currentp)
{
   mark_t *safe_state;

   safe_state= backupBufferInfo(currentp);

   movetoBOL(currentp); /* Det _ska_ inte kunna bli fel! */
   if(!movetoPrevChar(currentp)){
//      beep(); /* Finns bara i curses!! */
      restoreBufferInfo(safe_state, currentp);
   }else{
      free(safe_state);
      movetoBOL(currentp); /* Precis samma här... */
      currentp->scr.y--;
      currentp->scr.x= movetoCol(currentp, currentp->scr.x);
   }
}

void down(mark_t *currentp)
{
   mark_t *safe_state;

   safe_state= backupBufferInfo(currentp);

   movetoEOL(currentp);
   if (!movetoNextChar(currentp)){
//      beep(); /* Finns bara i curses!! */
      restoreBufferInfo(safe_state, currentp);
   }
   else{
      free(safe_state);
      currentp->scr.y++;
      currentp->scr.x= movetoCol(currentp, currentp->scr.x);
   }
}

void insert(mark_t *currentp, int thisCommand)
{
   /* Skriv in tecknet i strukturen ... mycket jox... */
   if (13 == (char)thisCommand){
      insertCommand(currentp, (int)10);
      currentp->scr.y++;
      currentp->scr.x= 0;
   }
   else{
      if (isprint(thisCommand)){
         insertCommand(currentp, thisCommand);
         currentp->scr.x++;
      }
      else{
//         beep(); /* Finns bara i curses!! */
      }
   }
}


void delete(mark_t *currentp)
{
   if (currentp->pos + 1 == currentp->node->end){
      currentp->node->end--;
      getNextNode(currentp);
   }
   else{
      if (currentp->pos > currentp->node->begin){
         splitNode(currentp);

         LOG("[node splitted]");
      }
      if (currentp->node->begin < currentp->node->end)
         (currentp->node->begin)++;

      currentp->pos= currentp->node->begin;
   }

   if (currentp->node->begin == currentp->node->end)
      if (!deleteRightNode(currentp))
         currentp->pos= currentp->node->begin;
}



int calculateXposition(text *thisp, int i)
{
   int counter= 0;

   if (thisp){
      for (counter= 0;
           '\n' == thisp->line[i];
           counter++, i--){
         if (i == thisp->begin){
            if (thisp->previous){
               thisp= thisp->previous;
               i= thisp->end;
            }else{
               break;
            }
         }
      }
   }

   return (counter);
}

/* Simple prototype for a generator/fread-function alike thingy.
 *
 * This needs another implementation and interface. Something more
 * like the fread()-function...
 */
char *editorCharacterGenerator(mark_t *currentp)
{
   int i, j;
   text *thisText;
   char *genbuf;

   /* Lite kinkit det här att allokera minne, raderna kan ju vara
    * väldigt olika långa.
    */
   int nofbytes= (currentp->scr.maxX) * (currentp->scr.maxY);

   genbuf= allocate(nofbytes, "editorCharacterGenerator");

   j= 0;
   thisText= currentp->top;
   while(thisText != NULL && j < nofbytes){
      i= thisText->begin;
      while(i < thisText->end && j < nofbytes){
         genbuf[j++]= thisText->line[i++];
      }
      thisText= thisText->next;
   }

   genbuf[j]= '\0';
   return genbuf;
}



/*** splitNode() ********************************************************
 * Tar en nod och splittar den i två vid 'i'. Länkar in de två nya i
 * "baklänges" ordning.
 * Returnerar pekaren till den högra av de två.
 ************************************************************************
 */
text *splitNode(mark_t *currentp)
{
   text *left, *right;
   text *thisp= currentp->node;

   /* Snabbkoll om vi står på antingen begin eller end */
   if (currentp->pos == currentp->node->begin)
      return currentp->node; /* Delat och klart!! :-) */
   else if (currentp->pos == currentp->node->end)
      if (getNextNode(currentp))
         return currentp->node; /* Oxå färdig... ;) */

   /* Kan man optimera det här tro? Tidsmässigt/kodmässigt? */
   right= allocate(sizeof(text), "splitNode()");
   left= allocate(sizeof(text), "splitNode()");
   memcpy(right, thisp, sizeof(text));
   memcpy(left, thisp, sizeof(text));

   /* Vi pekar på varandra */
   left->next= right;
   right->previous= left;

   /* Efterföljande nods previous-pekarare ska peka på den högra. */
   if (thisp->next)
      (thisp->next)->previous= right;

   /* Finns det nån föregående ska den peka på den vänstra... */
   if (thisp->previous)
      (thisp->previous)->next= left;
   /* Annars så var det första noden ... */
   else
      currentp->stx= left;

   /* är det här samma nod som displayText() vill läsa? */
   if (currentp->top == thisp)
      currentp->top= left;

   /* Nuschkavischehäär.... *fullogo* */
   /* Vänster nod ska ha allt som var till vänster om i utom i självt.
      Höger ska ha allt till höger inkl. i
      eller är jag ute och rave:ar nu?
      Borde ju bli:
         left->begin= samma;
         left->end= i;
         right->begin= i;
         right->end= samma.

      Frågan är vad det är jag gjort nedan... *fullständigt nollställd!*
   */
/*
   if (currentp->pos > thisp->begin){
      left->end= currentp->pos;
      right->begin= currentp->pos;
   }else{
      left->end= thisp->begin;
      right->begin= currentp->pos;
   }
*/
/* kliipper in det som verkar vettigast... */
   left->end= currentp->pos;
   right->begin= currentp->pos;

   /* Vi själva ställer oss i den högra noden på begin. */
   currentp->node= right;
   currentp->pos= right->begin;

   /* Glömde nästan att ta bort den gamla... */
   free(thisp);

   return (right);
}


/*** deleteRightNode() **************************************************
 * Tar bort nuvarande nod, men inte om det är den enda kvar. Givetvis
 * tas noden bara bort omm den är tom!
 * Funktionen kan bara "fallera" när den nuvarande noden är helt ensam.
 * OBS!! pos kan aldrig garanteras av den här funktionen. Iaf. inte
 * om NULL returneras... För då har ingenting gjorts.
 ************************************************************************
 */
text *deleteRightNode(mark_t *currentp)
{
   text *thisp= currentp->node;

   if (thisp->begin < thisp->end)
      return (NULL);

   if (!thisp->next){
      if (thisp->previous){
         (thisp->previous)->next= thisp->next;

         currentp->node= thisp->previous;
         currentp->pos= thisp->previous->end;

         if (currentp->top == thisp)
            currentp->top= thisp->previous;

         free(thisp);
         return (currentp->node);
      }
   }else{
      if (thisp->previous)
         (thisp->previous)->next= thisp->next;
      else
         currentp->stx= thisp->next;

      if (currentp->top == thisp)
         currentp->top= thisp->next;

      currentp->node= thisp->next;
      currentp->pos=thisp->next->begin;

      free(thisp);

      return (currentp->node);
   }

   return (NULL);
}



/*** insertNode(text *thisp, int i) *************************************
 * Infogar en ny nod vid 'pos'.
 *
 * Returnerar pekaren till den nya fräsha noden.
 ************************************************************************
 */
static text *insertNode(mark_t *currentp)
{
   text *newNode;

   newNode= splitNode(currentp);
   if (currentp->node->begin == currentp->node->end){
      currentp->node->begin= 0;
      currentp->node->end= 0;
      currentp->pos= 0;
   }
   else{
      newNode= allocate(sizeof(text), "insertNode()");
      newNode->next= currentp->node;
      newNode->previous= currentp->node->previous;
      if (currentp->node->previous)
         (currentp->node->previous)->next= newNode;
      else
         currentp->stx= newNode;
      currentp->node->previous= newNode;

      if (currentp->top == currentp->node)
         currentp->top= newNode;

      currentp->node= newNode;
      currentp->pos= newNode->begin;
   }

   return (newNode);
}


/*** insertCommand() ****************************************************
 * OBS!! Den här rackaren tar iofs. emot thisp & i men den är inte så
 * oberoende för det. På ett par ställen tilldelas nämligen current.pos
 * istf. tex (*i) så den går för tillfället bara att använda till att
 * infoga tecken vid markören ...
 ************************************************************************
 */
void insertCommand(mark_t *currentp, int command)
{
   /* Är detta en tom nod? Mindre troligt faktiskt eftersom alla såna
    * brukar tas bort direkt av delete...
    */
   if (currentp->node->begin == currentp->node->end){
      currentp->node->begin= 0;
      currentp->node->line[0]= (char)command;
      currentp->node->end= 1;
      currentp->pos= 1;
   }else{
      /* Står vi i slutet på en icketom nod? */
      if ((currentp->pos == currentp->node->end) &&
          (currentp->pos < (MAX_LINE_LENGTH - 1))){
         currentp->node->line[currentp->pos]= (char)command;
         currentp->pos++;
         currentp->node->end++;
      }else{
         insertNode(currentp);
         currentp->node->line[0]= (char)command;
         currentp->node->end= 1;
         currentp->pos= 1;
      }
   }
}

/************************************************************************
 * Revolutionary new ideas!! :-D
 ************************************************************************
 */

/*** backupBufferInfo() *************************************************
 * Backup
 */
mark_t *backupBufferInfo(mark_t *currentp)
{
   mark_t *backup_info;

   backup_info= allocate(sizeof(mark_t), "backupBufferInfo()");
   memcpy(backup_info, currentp, sizeof(mark_t));

   return backup_info;
}

/*** restoreBufferInfo() ************************************************
 * Restore
 */
void restoreBufferInfo(mark_t *backup_info, mark_t *currentp)
{
   memcpy(currentp, backup_info, sizeof(mark_t));

   free(backup_info);
}


/*** getNextNode() ******************************************************
 * Bör nog, precis som sin syster, heta något annat. movetoNextNode()
 * kanske?
 * Markören flyttas till början på nästa nod, om det finns någon.
 * Funktionen misslyckas vid slutet på filen.
 ************************************************************************
 */
int getNextNode(mark_t *currentp)
{
        if (currentp->node->next){
                currentp->node= currentp->node->next;
                currentp->pos= currentp->node->begin;
                return 1;
        }

        return 0;
}

/*** getPrevNode() ******************************************************
 * Borde kanske eg. heta nåt i stil med movetoPrevNode() ...
 * Så länge det finns en föregående nod flyttas kontext/markören till
 * slutet på denna.
 * Stod man redan innan på första noden så kommer funktionen att
 * misslyckas.
 ************************************************************************
 */
int getPrevNode(mark_t *currentp)
{
        if (!((currentp->node)->previous)){
      return 0;
   }
   currentp->node= currentp->node->previous;
   currentp->pos= currentp->node->end;

   if (currentp->pos == currentp->node->begin)
      if(!getPrevNode(currentp))
         return 0;

   return 1;
}


/*** movetoNextChar() ***************************************************
 * Flytta markören till nästa tecken, oberoende om det finns i nästa nod
 * eller på nästa rad.
 *
 * Ett enda problem förutom att hålla sig inom nodernas början och slut:
 * ... slutet på filen. Det är enda tillfället då funktionen öht kan
 * misslyckas.
 ************************************************************************
 */
int movetoNextChar(mark_t *currentp)
{
        while (currentp->pos == currentp->node->end){
                if (!getNextNode(currentp)){
         return 0;
      }
   }
   currentp->pos++;

        return 1;
}


/*** movetoPrevChar() ***************************************************
 * Flyttar markören till föregående tecken, oberoende av om detta ligger
 * i en annan (föregående) nof eller på raden innan...
 *
 * Precis som sin systerfunktion kan den bara fallera på ett enda ställe
 * nämligen vid början på filen. Ingen föregående nod finns och pos. i
 * nuvarande nod är ekvivalent med början på densamma.
 ************************************************************************
 */
int movetoPrevChar(mark_t *currentp)
{
   currentp->pos--;
   if (currentp->pos < currentp->node->begin){
      if (!getPrevNode(currentp)){
         (currentp->pos)++;
         return 0;
      }
      currentp->pos--;
   }

        return 1;
}


/*** atSTX() ************************************************************
 * Står vi i början på filen?
 ************************************************************************
 */
int atSTX(mark_t *currentp)
{
   if (!currentp->node->previous && currentp->pos == currentp->node->begin)
      return 1;

   return 0;
}


char getCurrentChar(mark_t *currentp)
{
        return currentp->node->line[currentp->pos];
}


int getCurrentColumn(mark_t *currentp)
{
   int counter= 0;
   mark_t *prev_state;

   prev_state= backupBufferInfo(currentp);

   if (movetoBOL(currentp)){
      while (movetoNextChar(currentp)){
         counter++;
         if (prev_state->pos == currentp->pos)
            break;
      }
   }
   restoreBufferInfo(prev_state, currentp);

   return counter;
}


/*** movetoEOL() ********************************************************
 * Tries to move the insertpoint to EOL.
 *
 * 1) We're already at EOL -> OK
 * 2) While not at EOL movetoNextChar (if that's possible)
 *
 * This function is ALWAYS successful...
 *
 ************************************************************************
 */
int movetoEOL(mark_t *currentp)
{
   do{
//      if ('\n' == getCurrentChar(currentp))
                if (10 == getCurrentChar(currentp))
         return 1;
   }while (movetoNextChar(currentp));

   return 1;
}


/*** movetoBOL() ********************************************************
 * Puts the cursor at the first column of the current line.
 *
 * There is really only one problem and that is when you are at the
 * beginning of the file. No previous nodes or previous newlines...
 * But none of these are really no problem since you always can find the
 * beginning of a line as well as the sister function movetoEOL()!
 ************************************************************************
 */
int movetoBOL(mark_t *currentp)
{
   while (movetoPrevChar(currentp)){
//      if ('\n' == getCurrentChar(currentp)){
      if (10 == getCurrentChar(currentp)){
         movetoNextChar(currentp); /* This should NEVER fail!! */
         return 1;
      }
   }
   if (atSTX(currentp))
      return 1;

   /* We would really never get here... */
   LOG("movetoBOL() - FAILED!!\n");
   return 0;
}

/*** movetoSTX() ********************************************************
 *
 ************************************************************************
 */
int movetoSTX(mark_t *currentp)
{
   currentp->node= currentp->stx;
   currentp->pos= currentp->stx->begin;

   return 1;
}


/*** movetoCol() ********************************************************
 *
 ************************************************************************
 */
int movetoCol(mark_t *currentp, int col)
{
   int counter= 0;

   if (!movetoBOL(currentp))
      return counter; /* This should NEVER happen!! Semantically that is ... :/ */

//   for (counter= 0; counter < col && '\n' != getCurrentChar(currentp);counter++)
   for (counter= 0; (counter < col) && (10 != getCurrentChar(currentp));counter++)
      movetoNextChar(currentp);

   return counter;
}

#endif /* DLL_MODE */
