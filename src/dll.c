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
static text *insertNode(buffer_t * currentp);
int calculateXposition(text * thisp, int i);
text *splitNode(buffer_t * currentp);
text *deleteRightNode(buffer_t * currentp);
void insertCommand(buffer_t * currentp, int command);
buffer_t *backupBufferInfo(buffer_t * currentp);
void restoreBufferInfo(buffer_t * backup_info, buffer_t * currentp);
int getNextNode(buffer_t * currentp);
int getPrevNode(buffer_t * currentp);
int movetoNextChar(buffer_t * currentp);
int movetoPrevChar(buffer_t * currentp);
int atSTX(buffer_t * currentp);
char getCurrentChar(buffer_t * currentp);
int getCurrentColumn(buffer_t * currentp);
int movetoEOL(buffer_t * currentp);
int movetoBOL(buffer_t * currentp);
int movetoSTX(buffer_t * currentp);
int movetoCol(buffer_t * currentp, int col);


void left(buffer_t * currentp)
{
	if (movetoPrevChar(currentp)) {
		if ('\n' == getCurrentChar(currentp)) {
			currentp->scr.y--;
			currentp->scr.x = getCurrentColumn(currentp);
		} else {
			currentp->scr.x--;
		}
	} else {
//      beep(); /* Finns bara i curses!! */
	}
}

void right(buffer_t * currentp)
{
	char previousChar;

	previousChar = getCurrentChar(currentp);

	if (movetoNextChar(currentp)) {
		if ('\n' == previousChar) {
			currentp->scr.y++;
			currentp->scr.x = 0;
		} else {
			currentp->scr.x++;
		}
	} else {
//      beep(); /* Finns bara i curses!! */
	}
}

void up(buffer_t * currentp)
{
	buffer_t *safe_state;

	safe_state = backupBufferInfo(currentp);

	movetoBOL(currentp);	/* Det _ska_ inte kunna bli fel! */
	if (!movetoPrevChar(currentp)) {
//      beep(); /* Finns bara i curses!! */
		restoreBufferInfo(safe_state, currentp);
	} else {
		free(safe_state);
		movetoBOL(currentp);	/* Precis samma h�r... */
		currentp->scr.y--;
		currentp->scr.x = movetoCol(currentp, currentp->scr.x);
	}
}

void down(buffer_t * currentp)
{
	buffer_t *safe_state;

	safe_state = backupBufferInfo(currentp);

	movetoEOL(currentp);
	if (!movetoNextChar(currentp)) {
//      beep(); /* Finns bara i curses!! */
		restoreBufferInfo(safe_state, currentp);
	} else {
		free(safe_state);
		currentp->scr.y++;
		currentp->scr.x = movetoCol(currentp, currentp->scr.x);
	}
}

void insert(buffer_t * currentp, int thisCommand)
{
	/* Skriv in tecknet i strukturen ... mycket jox... */
	if (13 == (char)thisCommand) {
		insertCommand(currentp, (int)10);
		currentp->scr.y++;
		currentp->scr.x = 0;
	} else {
		if (isprint(thisCommand)) {
			insertCommand(currentp, thisCommand);
			currentp->scr.x++;
		} else {
//         beep(); /* Finns bara i curses!! */
		}
	}
}


void delete(buffer_t * currentp)
{
	if (currentp->pos + 1 == currentp->core.node->end) {
		currentp->core.node->end--;
		getNextNode(currentp);
	} else {
		if (currentp->pos > currentp->core.node->begin) {
			splitNode(currentp);

			LOG("[node splitted]");
		}
		if (currentp->core.node->begin < currentp->core.node->end)
			(currentp->core.node->begin)++;

		currentp->pos = currentp->core.node->begin;
	}

	if (currentp->core.node->begin == currentp->core.node->end)
		if (!deleteRightNode(currentp))
			currentp->pos = currentp->core.node->begin;
}



int calculateXposition(text * thisp, int i)
{
	int counter = 0;

	if (thisp) {
		for (counter = 0; '\n' == thisp->line[i]; counter++, i--) {
			if (i == thisp->begin) {
				if (thisp->previous) {
					thisp = thisp->previous;
					i = thisp->end;
				} else {
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
char *editorCharacterGenerator(buffer_t * currentp)
{
	int i, j;
	text *thisText;
	char *genbuf;

	/* Lite kinkit det h�r att allokera minne, raderna kan ju vara
	 * v�ldigt olika l�nga.
	 */
	int nofbytes = (currentp->scr.maxX) * (currentp->scr.maxY);

	genbuf = allocate(nofbytes, "editorCharacterGenerator");

	j = 0;
	thisText = currentp->screen.top;
	while (thisText != NULL && j < nofbytes) {
		i = thisText->begin;
		while (i < thisText->end && j < nofbytes) {
			genbuf[j++] = thisText->line[i++];
		}
		thisText = thisText->next;
	}

	genbuf[j] = '\0';
	return genbuf;
}



/*** splitNode() ********************************************************
 * Tar en nod och splittar den i tv� vid 'i'. L�nkar in de tv� nya i
 * "bakl�nges" ordning.
 * Returnerar pekaren till den h�gra av de tv�.
 ************************************************************************
 */
text *splitNode(buffer_t * currentp)
{
	text *left, *right;
	text *thisp = currentp->core.node;

	/* Snabbkoll om vi st�r p� antingen begin eller end */
	if (currentp->pos == currentp->core.node->begin)
		return currentp->core.node;	/* Delat och klart!! :-) */
	else if (currentp->pos == currentp->core.node->end)
		if (getNextNode(currentp))
			return currentp->core.node;	/* Ox� f�rdig... ;) */

	/* Kan man optimera det h�r tro? Tidsm�ssigt/kodm�ssigt? */
	right = allocate(sizeof(text), "splitNode()");
	left = allocate(sizeof(text), "splitNode()");
	memcpy(right, thisp, sizeof(text));
	memcpy(left, thisp, sizeof(text));

	/* Vi pekar p� varandra */
	left->next = right;
	right->previous = left;

	/* Efterf�ljande nods previous-pekarare ska peka p� den h�gra. */
	if (thisp->next)
		(thisp->next)->previous = right;

	/* Finns det n�n f�reg�ende ska den peka p� den v�nstra... */
	if (thisp->previous)
		(thisp->previous)->next = left;
	/* Annars s� var det f�rsta noden ... */
	else
		currentp->core.stx = left;

	/* �r det h�r samma nod som displayText() vill l�sa? */
	if (currentp->screen.top == thisp)
		currentp->screen.top = left;

	/* Nuschkavischeh��r.... *fullogo* */
	/* V�nster nod ska ha allt som var till v�nster om i utom i sj�lvt.
	   H�ger ska ha allt till h�ger inkl. i
	   eller �r jag ute och rave:ar nu?
	   Borde ju bli:
	   left->begin= samma;
	   left->end= i;
	   right->begin= i;
	   right->end= samma.

	   Fr�gan �r vad det �r jag gjort nedan... *fullst�ndigt nollst�lld!*
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
	left->end = currentp->pos;
	right->begin = currentp->pos;

	/* Vi sj�lva st�ller oss i den h�gra noden p� begin. */
	currentp->core.node = right;
	currentp->pos = right->begin;

	/* Gl�mde n�stan att ta bort den gamla... */
	free(thisp);

	return (right);
}


/*** deleteRightNode() **************************************************
 * Tar bort nuvarande nod, men inte om det �r den enda kvar. Givetvis
 * tas noden bara bort omm den �r tom!
 * Funktionen kan bara "fallera" n�r den nuvarande noden �r helt ensam.
 * OBS!! pos kan aldrig garanteras av den h�r funktionen. Iaf. inte
 * om NULL returneras... F�r d� har ingenting gjorts.
 ************************************************************************
 */
text *deleteRightNode(buffer_t * currentp)
{
	text *thisp = currentp->core.node;

	if (thisp->begin < thisp->end)
		return (NULL);

	if (!thisp->next) {
		if (thisp->previous) {
			(thisp->previous)->next = thisp->next;

			currentp->core.node = thisp->previous;
			currentp->pos = thisp->previous->end;

			if (currentp->screen.top == thisp)
				currentp->screen.top = thisp->previous;

			free(thisp);
			return (currentp->core.node);
		}
	} else {
		if (thisp->previous)
			(thisp->previous)->next = thisp->next;
		else
			currentp->core.stx = thisp->next;

		if (currentp->screen.top == thisp)
			currentp->screen.top = thisp->next;

		currentp->core.node = thisp->next;
		currentp->pos = thisp->next->begin;

		free(thisp);

		return (currentp->core.node);
	}

	return (NULL);
}



/*** insertNode(text *thisp, int i) *************************************
 * Infogar en ny nod vid 'pos'.
 *
 * Returnerar pekaren till den nya fr�sha noden.
 ************************************************************************
 */
static text *insertNode(buffer_t * currentp)
{
	text *newNode;

	newNode = splitNode(currentp);
	if (currentp->core.node->begin == currentp->core.node->end) {
		currentp->core.node->begin = 0;
		currentp->core.node->end = 0;
		currentp->pos = 0;
	} else {
		newNode = allocate(sizeof(text), "insertNode()");
		newNode->next = currentp->core.node;
		newNode->previous = currentp->core.node->previous;
		if (currentp->core.node->previous)
			(currentp->core.node->previous)->next = newNode;
		else
			currentp->core.stx = newNode;
		currentp->core.node->previous = newNode;

		if (currentp->screen.top == currentp->core.node)
			currentp->screen.top = newNode;

		currentp->core.node = newNode;
		currentp->pos = newNode->begin;
	}

	return (newNode);
}


/*** insertCommand() ****************************************************
 * OBS!! Den h�r rackaren tar iofs. emot thisp & i men den �r inte s�
 * oberoende f�r det. P� ett par st�llen tilldelas n�mligen current.pos
 * istf. tex (*i) s� den g�r f�r tillf�llet bara att anv�nda till att
 * infoga tecken vid mark�ren ...
 ************************************************************************
 */
void insertCommand(buffer_t * currentp, int command)
{
	/* �r detta en tom nod? Mindre troligt faktiskt eftersom alla s�na
	 * brukar tas bort direkt av delete...
	 */
	if (currentp->core.node->begin == currentp->core.node->end) {
		currentp->core.node->begin = 0;
		currentp->core.node->line[0] = (char)command;
		currentp->core.node->end = 1;
		currentp->pos = 1;
	} else {
		/* St�r vi i slutet p� en icketom nod? */
		if ((currentp->pos == currentp->core.node->end) && (currentp->pos < (MAX_LINE_LENGTH - 1))) {
			currentp->core.node->line[currentp->pos] = (char)command;
			currentp->pos++;
			currentp->core.node->end++;
		} else {
			insertNode(currentp);
			currentp->core.node->line[0] = (char)command;
			currentp->core.node->end = 1;
			currentp->pos = 1;
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
buffer_t *backupBufferInfo(buffer_t * currentp)
{
	buffer_t *backup_info;

	backup_info = allocate(sizeof(buffer_t), "backupBufferInfo()");
	memcpy(backup_info, currentp, sizeof(buffer_t));

	return backup_info;
}

/*** restoreBufferInfo() ************************************************
 * Restore
 */
void restoreBufferInfo(buffer_t * backup_info, buffer_t * currentp)
{
	memcpy(currentp, backup_info, sizeof(buffer_t));

	free(backup_info);
}


/*** getNextNode() ******************************************************
 * B�r nog, precis som sin syster, heta n�got annat. movetoNextNode()
 * kanske?
 * Mark�ren flyttas till b�rjan p� n�sta nod, om det finns n�gon.
 * Funktionen misslyckas vid slutet p� filen.
 ************************************************************************
 */
int getNextNode(buffer_t * currentp)
{
	if (currentp->core.node->next) {
		currentp->core.node = currentp->core.node->next;
		currentp->pos = currentp->core.node->begin;
		return 1;
	}

	return 0;
}

/*** getPrevNode() ******************************************************
 * Borde kanske eg. heta n�t i stil med movetoPrevNode() ...
 * S� l�nge det finns en f�reg�ende nod flyttas kontext/mark�ren till
 * slutet p� denna.
 * Stod man redan innan p� f�rsta noden s� kommer funktionen att
 * misslyckas.
 ************************************************************************
 */
int getPrevNode(buffer_t * currentp)
{
	if (!((currentp->core.node)->previous)) {
		return 0;
	}
	currentp->core.node = currentp->core.node->previous;
	currentp->pos = currentp->core.node->end;

	if (currentp->pos == currentp->core.node->begin)
		if (!getPrevNode(currentp))
			return 0;

	return 1;
}


/*** movetoNextChar() ***************************************************
 * Flytta mark�ren till n�sta tecken, oberoende om det finns i n�sta nod
 * eller p� n�sta rad.
 *
 * Ett enda problem f�rutom att h�lla sig inom nodernas b�rjan och slut:
 * ... slutet p� filen. Det �r enda tillf�llet d� funktionen �ht kan
 * misslyckas.
 ************************************************************************
 */
int movetoNextChar(buffer_t * currentp)
{
	while (currentp->pos == currentp->core.node->end) {
		if (!getNextNode(currentp)) {
			return 0;
		}
	}
	currentp->pos++;

	return 1;
}


/*** movetoPrevChar() ***************************************************
 * Flyttar mark�ren till f�reg�ende tecken, oberoende av om detta ligger
 * i en annan (f�reg�ende) nof eller p� raden innan...
 *
 * Precis som sin systerfunktion kan den bara fallera p� ett enda st�lle
 * n�mligen vid b�rjan p� filen. Ingen f�reg�ende nod finns och pos. i
 * nuvarande nod �r ekvivalent med b�rjan p� densamma.
 ************************************************************************
 */
int movetoPrevChar(buffer_t * currentp)
{
	currentp->pos--;
	if (currentp->pos < currentp->core.node->begin) {
		if (!getPrevNode(currentp)) {
			(currentp->pos)++;
			return 0;
		}
		currentp->pos--;
	}

	return 1;
}


/*** atSTX() ************************************************************
 * St�r vi i b�rjan p� filen?
 ************************************************************************
 */
int atSTX(buffer_t * currentp)
{
	if (!currentp->core.node->previous && currentp->pos == currentp->core.node->begin)
		return 1;

	return 0;
}


char getCurrentChar(buffer_t * currentp)
{
	return currentp->core.node->line[currentp->pos];
}


int getCurrentColumn(buffer_t * currentp)
{
	int counter = 0;
	buffer_t *prev_state;

	prev_state = backupBufferInfo(currentp);

	if (movetoBOL(currentp)) {
		while (movetoNextChar(currentp)) {
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
int movetoEOL(buffer_t * currentp)
{
	do {
//      if ('\n' == getCurrentChar(currentp))
		if (10 == getCurrentChar(currentp))
			return 1;
	} while (movetoNextChar(currentp));

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
int movetoBOL(buffer_t * currentp)
{
	while (movetoPrevChar(currentp)) {
//      if ('\n' == getCurrentChar(currentp)){
		if (10 == getCurrentChar(currentp)) {
			movetoNextChar(currentp);	/* This should NEVER fail!! */
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
int movetoSTX(buffer_t * currentp)
{
	currentp->core.node = currentp->core.stx;
	currentp->pos = currentp->core.stx->begin;

	return 1;
}


/*** movetoCol() ********************************************************
 *
 ************************************************************************
 */
int movetoCol(buffer_t * currentp, int col)
{
	int counter = 0;

	if (!movetoBOL(currentp))
		return counter;	/* This should NEVER happen!! Semantically that is ... :/ */

//   for (counter= 0; counter < col && '\n' != getCurrentChar(currentp);counter++)
	for (counter = 0; (counter < col) && (10 != getCurrentChar(currentp)); counter++)
		movetoNextChar(currentp);

	return counter;
}


text_t newFile(text_t * core)
{
}


text_t *readFile(FILE *filep, text_t * core)
{
	text *headp = NULL, *currentp = NULL, *previousp = NULL;
	size_t bytesRead;

	do {
		/* XXX - not refactored yet! */
		//    currentp->line = allocate(MAX_LINE_LENGTH, "readFile()");
		bytesRead = fread(currentp->line, sizeof(char), MAX_LINE_LENGTH, filep);
		currentp->begin = 0;
		currentp->end = bytesRead;

		if (headp == NULL)
			headp = currentp;
		else
			previousp->next = currentp;

		currentp->previous = previousp;
		previousp = currentp;
	} while (MAX_LINE_LENGTH == bytesRead);

	return (headp);
}


/*** saveFile(text *start, char *filename) ******************************
 * Sparar p� ett v�ldigt simpelt s�tt hela klabbet i en fil.
 * B�rjar spara fr�n start till slutet... borde kanske g�ras om s.a. den
 * klarar att spara markerad text fr�n valfria pos. och annat jox.
 ************************************************************************
 */
int saveFile(buffer_t * currentp, char *filename)
{
	text *thisp;
	size_t bytesWritten;
	FILE *filep;

	if (NULL == (filep = fopen(filename, "w"))) {
		LOG("Could not open %s for saving.\n", filename);
		return 0;
	}
	for (thisp = currentp->stx; thisp != NULL; thisp = thisp->next) {
		bytesWritten = fwrite(&(thisp->line[thisp->begin]), sizeof(char), thisp->end - thisp->begin, filep);
		if (bytesWritten != (thisp->end - thisp->begin)) {
			LOG("Oups, difference between bytes in node and bytes written to %s...\n", filename);
		}
	}

	fclose(filep);

	currentp->dirty = 0;

	return 1;
}


#endif				/* DLL_MODE */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
