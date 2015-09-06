/* dll.c - The dll GUL editor core.
 *
 * Copyright (c) 2004-2015  Joachim Nilsson <troglobit@gmail.com>
 * Copyright (c) 1998-2000  Joachim Nilsson, Jakob Eriksson, Anders Born�s
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
static text_t *split_node(buffer_t *buf);
static text_t *delete_node(buffer_t *buf);
static text_t *insert_node(buffer_t *buf);

static int  get_next_node(buffer_t *buf);
static int  get_prev_node(buffer_t *buf);

static buffer_t *backup_buffer_pos (buffer_t *buf);
static void      restore_buffer_pos(buffer_t *buf, buffer_t *backup);

static int  move_to_eol      (buffer_t *buf);
static int  move_to_bol      (buffer_t *buf);
static int  move_to_stx      (buffer_t *buf);
static int  move_to_col      (buffer_t *buf, int col);

static void insert_char      (buffer_t *buf, int command);
static int  move_to_next_char(buffer_t *buf);
static int  move_to_prev_char(buffer_t *buf);

static int  at_stx           (buffer_t *buf);

static char get_current_char (buffer_t *buf);
static int  get_current_col  (buffer_t *buf);


void left(buffer_t *buf)
{
	if (move_to_prev_char(buf)) {
		if ('\n' == get_current_char(buf)) {
			buf->scr.y--;
			buf->scr.x = get_current_col(buf);
		} else {
			buf->scr.x--;
		}
	} else {
//      beep(); /* Finns bara i curses!! */
	}
}

void right(buffer_t *buf)
{
	char previousChar;

	previousChar = get_current_char(buf);

	if (move_to_next_char(buf)) {
		if ('\n' == previousChar) {
			buf->scr.y++;
			buf->scr.x = 0;
		} else {
			buf->scr.x++;
		}
	} else {
//      beep(); /* Finns bara i curses!! */
	}
}

void up(buffer_t *buf)
{
	buffer_t *backup = backup_buffer_pos(buf);

	move_to_bol(buf);	/* Det _ska_ inte kunna bli fel! */
	if (!move_to_prev_char(buf)) {
//      beep(); /* Finns bara i curses!! */
		restore_buffer_pos(buf, backup);
	} else {
		free(safe_state);
		move_to_bol(buf);	/* Precis samma h�r... */
		buf->scr.y--;
		buf->scr.x = move_to_col(buf, buf->scr.x);
	}
}

void down(buffer_t *buf)
{
	buffer_t *backup = backup_buffer_pos(buf);

	move_to_eol(buf);
	if (!move_to_next_char(buf)) {
//      beep(); /* Finns bara i curses!! */
		restore_buffer_pos(buf, backup);
	} else {
		free(safe_state);
		buf->scr.y++;
		buf->scr.x = move_to_col(buf, buf->scr.x);
	}
}

void insert(buffer_t *buf, int ch)
{
	/* Skriv in tecknet i strukturen ... mycket jox... */
	if (13 == (char)ch) {
		insert_char(buf, (int)10);
		buf->scr.y++;
		buf->scr.x = 0;
	} else {
		if (isprint(ch)) {
			insert_char(buf, ch);
			buf->scr.x++;
		} else {
//         beep(); /* Finns bara i curses!! */
		}
	}
}


void delete(buffer_t *buf)
{
	if (buf->pos + 1 == buf->core.node->end) {
		buf->core.node->end--;
		get_next_node(buf);
	} else {
		if (buf->pos > buf->core.node->begin) {
			split_node(buf);

			LOG("[node splitted]");
		}
		if (buf->core.node->begin < buf->core.node->end)
			(buf->core.node->begin)++;

		buf->pos = buf->core.node->begin;
	}

	if (buf->core.node->begin == buf->core.node->end)
		if (!delete_node(buf))
			buf->pos = buf->core.node->begin;
}

/* Simple prototype for a generator/fread-function alike thingy.
 *
 * This needs another implementation and interface. Something more
 * like the fread()-function...
 */
char *editor_chargen(buffer_t *buf)
{
	int i, j;
	text *thisText;
	char *genbuf;

	/* Lite kinkit det h�r att allokera minne, raderna kan ju vara
	 * v�ldigt olika l�nga.
	 */
	int nofbytes = (buf->scr.max_col) * (buf->scr.max_row);

	genbuf = allocate(nofbytes, "editor_chargen");

	j = 0;
	thisText = buf->screen.top;
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



/*** split_node() ********************************************************
 * Tar en nod och splittar den i tv� vid 'i'. L�nkar in de tv� nya i
 * "bakl�nges" ordning.
 * Returnerar pekaren till den h�gra av de tv�.
 ************************************************************************
 */
text_t *split_node(buffer_t *buf)
{
	text_t *left, *right;
	text_t *text = buf->core.node;

	/* Snabbkoll om vi st�r p� antingen begin eller end */
	if (buf->pos == buf->core.node->begin)
		return buf->core.node;	/* Delat och klart!! :-) */
	else if (buf->pos == buf->core.node->end)
		if (get_next_node(buf))
			return buf->core.node;	/* Ox� f�rdig... ;) */

	/* Kan man optimera det h�r tro? Tidsm�ssigt/kodm�ssigt? */
	right = allocate(sizeof(text), "split_node()");
	left = allocate(sizeof(text), "split_node()");
	memcpy(right, text, sizeof(text));
	memcpy(left, text, sizeof(text));

	/* Vi pekar p� varandra */
	left->next = right;
	right->previous = left;

	/* Efterf�ljande nods previous-pekarare ska peka p� den h�gra. */
	if (text->next)
		(text->next)->previous = right;

	/* Finns det n�n f�reg�ende ska den peka p� den v�nstra... */
	if (text->previous)
		(text->previous)->next = left;
	/* Annars s� var det f�rsta noden ... */
	else
		buf->core.stx = left;

	/* �r det h�r samma nod som displayText() vill l�sa? */
	if (buf->screen.top == text)
		buf->screen.top = left;

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
   if (buf->pos > text->begin){
      left->end= buf->pos;
      right->begin= buf->pos;
   }else{
      left->end= text->begin;
      right->begin= buf->pos;
   }
*/
/* kliipper in det som verkar vettigast... */
	left->end = buf->pos;
	right->begin = buf->pos;

	/* Vi sj�lva st�ller oss i den h�gra noden p� begin. */
	buf->core.node = right;
	buf->pos = right->begin;

	/* Gl�mde n�stan att ta bort den gamla... */
	free(text);

	return right;
}


/*** delete_node() **************************************************
 * Tar bort nuvarande nod, men inte om det �r den enda kvar. Givetvis
 * tas noden bara bort omm den �r tom!
 * Funktionen kan bara "fallera" n�r den nuvarande noden �r helt ensam.
 * OBS!! pos kan aldrig garanteras av den h�r funktionen. Iaf. inte
 * om NULL returneras... F�r d� har ingenting gjorts.
 ************************************************************************
 */
text *delete_node(buffer_t *buf)
{
	text *text = buf->core.node;

	if (text->begin < text->end)
		return NULL;

	if (!text->next) {
		if (text->previous) {
			(text->previous)->next = text->next;

			buf->core.node = text->previous;
			buf->pos = text->previous->end;

			if (buf->screen.top == text)
				buf->screen.top = text->previous;

			free(text);
			return buf->core.node;
		}
	} else {
		if (text->previous)
			(text->previous)->next = text->next;
		else
			buf->core.stx = text->next;

		if (buf->screen.top == text)
			buf->screen.top = text->next;

		buf->core.node = text->next;
		buf->pos = text->next->begin;

		free(text);

		return buf->core.node;
	}

	return NULL;
}



/*** insert_node(text *text, int i) *************************************
 * Infogar en ny nod vid 'pos'.
 *
 * Returnerar pekaren till den nya fr�sha noden.
 ************************************************************************
 */
static text_t *insert_node(buffer_t *buf)
{
	text_t *newNode;

	newNode = split_node(buf);
	if (buf->core.node->begin == buf->core.node->end) {
		buf->core.node->begin = 0;
		buf->core.node->end = 0;
		buf->pos = 0;
	} else {
		newNode = allocate(sizeof(text), "insert_node()");
		newNode->next = buf->core.node;
		newNode->previous = buf->core.node->previous;
		if (buf->core.node->previous)
			(buf->core.node->previous)->next = newNode;
		else
			buf->core.stx = newNode;
		buf->core.node->previous = newNode;

		if (buf->screen.top == buf->core.node)
			buf->screen.top = newNode;

		buf->core.node = newNode;
		buf->pos = newNode->begin;
	}

	return newNode;
}

/*** get_next_node() ******************************************************
 * Mark�ren flyttas till b�rjan p� n�sta nod, om det finns n�gon.
 * Funktionen misslyckas vid slutet p� filen.
 ************************************************************************
 */
int get_next_node(buffer_t *buf)
{
	if (buf->core.node->next) {
		buf->core.node = buf->core.node->next;
		buf->pos = buf->core.node->begin;
		return 1;
	}

	return 0;
}

/*** get_prev_node() ******************************************************
 * S� l�nge det finns en f�reg�ende nod flyttas kontext/mark�ren till
 * slutet p� denna.
 * Stod man redan innan p� f�rsta noden s� kommer funktionen att
 * misslyckas.
 ************************************************************************
 */
int get_prev_node(buffer_t *buf)
{
	if (!(buf->core.node)->previous)
		return 0;

	buf->core.node = buf->core.node->previous;
	buf->pos = buf->core.node->end;

	if (buf->pos == buf->core.node->begin)
		if (!get_prev_node(buf))
			return 0;

	return 1;
}

/*** insert_char() ****************************************************
 * OBS!! Den h�r rackaren tar iofs. emot text & i men den �r inte s�
 * oberoende f�r det. P� ett par st�llen tilldelas n�mligen current.pos
 * istf. tex (*i) s� den g�r f�r tillf�llet bara att anv�nda till att
 * infoga tecken vid mark�ren ...
 ************************************************************************
 */
static void insert_char(buffer_t *buf, int command)
{
	/* �r detta en tom nod? Mindre troligt faktiskt eftersom alla s�na
	 * brukar tas bort direkt av delete...
	 */
	if (buf->core.node->begin == buf->core.node->end) {
		buf->core.node->begin = 0;
		buf->core.node->line[0] = (char)command;
		buf->core.node->end = 1;
		buf->pos = 1;
	} else {
		/* St�r vi i slutet p� en icketom nod? */
		if ((buf->pos == buf->core.node->end) && (buf->pos < (MAX_LINE_LENGTH - 1))) {
			buf->core.node->line[buf->pos] = (char)command;
			buf->pos++;
			buf->core.node->end++;
		} else {
			insert_node(buf);
			buf->core.node->line[0] = (char)command;
			buf->core.node->end = 1;
			buf->pos = 1;
		}
	}
}

/*** backup_buffer_pos() *************************************************
 * Backup
 */
buffer_t *backup_buffer_pos(buffer_t *buf)
{
	buffer_t *backup;

	backup = allocate(sizeof(buffer_t), "backup_buffer_pos()");
	memcpy(backup, buf, sizeof(buffer_t));

	return backup;
}

/*** restore_buffer_pos() ************************************************
 * Restore
 */
static void restore_buffer_pos(buffer_t *buf, buffer_t *backup)
{
	memcpy(buf, backup, sizeof(buffer_t));
	free(backup_info);
}

/*** move_to_next_char() ***************************************************
 * Flytta mark�ren till n�sta tecken, oberoende om det finns i n�sta nod
 * eller p� n�sta rad.
 *
 * Ett enda problem f�rutom att h�lla sig inom nodernas b�rjan och slut:
 * ... slutet p� filen. Det �r enda tillf�llet d� funktionen �ht kan
 * misslyckas.
 ************************************************************************
 */
int move_to_next_char(buffer_t *buf)
{
	while (buf->pos == buf->core.node->end) {
		if (!get_next_node(buf)) {
			return 0;
		}
	}
	buf->pos++;

	return 1;
}

/*** move_to_prev_char() ***************************************************
 * Flyttar mark�ren till f�reg�ende tecken, oberoende av om detta ligger
 * i en annan (f�reg�ende) nof eller p� raden innan...
 *
 * Precis som sin systerfunktion kan den bara fallera p� ett enda st�lle
 * n�mligen vid b�rjan p� filen. Ingen f�reg�ende nod finns och pos. i
 * nuvarande nod �r ekvivalent med b�rjan p� densamma.
 ************************************************************************
 */
int move_to_prev_char(buffer_t *buf)
{
	buf->pos--;
	if (buf->pos < buf->core.node->begin) {
		if (!get_prev_node(buf)) {
			(buf->pos)++;
			return 0;
		}
		buf->pos--;
	}

	return 1;
}


/*** at_stx() ************************************************************
 * St�r vi i b�rjan p� filen?
 ************************************************************************
 */
static int at_stx(buffer_t *buf)
{
	if (!buf->core.node->previous && buf->pos == buf->core.node->begin)
		return 1;

	return 0;
}


char get_current_char(buffer_t *buf)
{
	return buf->core.node->line[buf->pos];
}


int get_current_col(buffer_t *buf)
{
	int counter = 0;
	buffer_t *prev_state;

	prev_state = backup_buffer_pos(buf);

	if (move_to_bol(buf)) {
		while (move_to_next_char(buf)) {
			counter++;
			if (prev_state->pos == buf->pos)
				break;
		}
	}
	restore_buffer_pos(prev_state, buf);

	return counter;
}


/*** move_to_eol() ********************************************************
 * Tries to move the insertpoint to EOL.
 *
 * 1) We're already at EOL -> OK
 * 2) While not at EOL move_to_next_char (if that's possible)
 *
 * This function is ALWAYS successful...
 *
 ************************************************************************
 */
int move_to_eol(buffer_t *buf)
{
	do {
//      if ('\n' == get_current_char(buf))
		if (10 == get_current_char(buf))
			return 1;
	} while (move_to_next_char(buf));

	return 1;
}


/*** move_to_bol() ********************************************************
 * Puts the cursor at the first column of the current line.
 *
 * There is really only one problem and that is when you are at the
 * beginning of the file. No previous nodes or previous newlines...
 * But none of these are really no problem since you always can find the
 * beginning of a line as well as the sister function move_to_eol()!
 ************************************************************************
 */
int move_to_bol(buffer_t *buf)
{
	while (move_to_prev_char(buf)) {
//      if ('\n' == get_current_char(buf)){
		if (10 == get_current_char(buf)) {
			move_to_next_char(buf);	/* This should NEVER fail!! */
			return 1;
		}
	}

	if (at_stx(buf))
		return 1;

	/* We would really never get here... */
	LOG("move_to_bol() - FAILED!!\n");
	return 0;
}

/*** move_to_stx() ********************************************************
 *
 ************************************************************************
 */
int move_to_stx(buffer_t *buf)
{
	buf->core.node = buf->core.stx;
	buf->pos = buf->core.stx->begin;

	return 1;
}


/*** move_to_col() ********************************************************
 *
 ************************************************************************
 */
int move_to_col(buffer_t *buf, int col)
{
	int counter = 0;

	if (!move_to_bol(buf))
		return counter;	/* This should NEVER happen!! Semantically that is ... :/ */

//   for (counter= 0; counter < col && '\n' != get_current_char(buf);counter++)
	for (counter = 0; (counter < col) && (10 != get_current_char(buf)); counter++)
		move_to_next_char(buf);

	return counter;
}


text_t file_new(buffer_t *buf, size_t len)
{
}


text_t *file_read(buffer_t *buf, FILE *fp)
{
	text_t *headp = NULL, *buf = NULL, *previousp = NULL;
	size_t bytesRead;

	do {
		/* XXX - not refactored yet! */
		//    buf->line = allocate(MAX_LINE_LENGTH, "file_read()");
		bytesRead = fread(buf->line, sizeof(char), MAX_LINE_LENGTH, fp);
		buf->begin = 0;
		buf->end = bytesRead;

		if (headp == NULL)
			headp = buf;
		else
			previousp->next = buf;

		buf->previous = previousp;
		previousp = buf;
	} while (MAX_LINE_LENGTH == bytesRead);

	return headp;
}


/*** file_save(text *start, char *filename) ******************************
 * Sparar p� ett v�ldigt simpelt s�tt hela klabbet i en fil.
 * B�rjar spara fr�n start till slutet... borde kanske g�ras om s.a. den
 * klarar att spara markerad text fr�n valfria pos. och annat jox.
 ************************************************************************
 */
int file_save(buffer_t *buf, char *filename)
{
	text *text;
	size_t bytesWritten;
	FILE *fp;

	fp = fopen(filename, "w");
	if (!fp) {
		LOG("Could not open %s for saving.\n", filename);
		return 0;
	}

	for (text = buf->stx; text != NULL; text = text->next) {
		bytesWritten = fwrite(&(text->line[text->begin]), sizeof(char), text->end - text->begin, fp);
		if (bytesWritten != (text->end - text->begin)) {
			LOG("Oups, difference between bytes in node and bytes written to %s...\n", filename);
		}
	}

	fclose(fp);

	buf->dirty = 0;

	return 1;
}

#endif /* DLL_MODE */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
