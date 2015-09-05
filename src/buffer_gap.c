/* buffer_gap.c - Yet Another Editor core Impl.        vim:set ts=3 sw=3 ruler:
 *
 * Yet another editor-core implementation.
 * Also known as "This ain't no disco!", courtesy of Depeche Mode.
 * First try at an implementation of the buffer gap method.
 *
 * Joachim Nilsson <crash@vmlinux.org>
 *
 ************************************************************************
 * Problems:
 ***********
 * Currently the API and the "chain of command" between the display and the
 * editor core is not as well defined as one might want. This needs looking
 * in to!
 *
 * What if the top->gap_size == 0 ???
 * Use a special move_to_next_char() and get_current_char() when moving around
 * chasing visible characters in the redraw() function???
 * Perhaps even move the "screen_gap" into the primary, stx, struct?
 * this would perhaps simplify things a bit.
 *
 * But all this relies most on the above statement!!
 *
 * Maybe support both a redraw() function AND a character generator, that
 * way bindings can be done at compile- or at run-time. Set up a bindings
 * table for each plugin to fill in. Similar to that of GWP! :)
 *
 * I've commented out all of the "move the virtual screen"-stuff. So now
 * the editor is only one (1) screensize big - That's it!
 */

#include "config.h"
#ifdef BUFFER_GAP_MODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fstat, fileno etc. */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "editor.h"
#include "buffer_gap.h"


/*** Macros *************************************************************
 */

#define GAP_AT_START(this)                      \
   ((this)->gap == (this)->buffer)

#define GAP_AT_END(this)                                                \
   ((this)->gap + (this)->gap_size - (this)->buffer == (this)->buffer_size)

/*** Prototypes *********************************************************
 */
static char *backup_buffer_pos (text_t *text);
static char *restore_buffer_pos(text_t *text, char *backup);

static int move_to_col(text_t *text, int col);
static int move_to_eol(text_t *text);
static int move_to_bol(text_t *text);
static int move_to_stx(text_t *text);

static int move_to_next_char(text_t *text);
static int move_to_prev_char(text_t *text);
static int move_to_next_line(text_t *text);
static int move_to_prev_line(text_t *text);

static char get_current_char(text_t *text);
static int  get_current_col (text_t *text);

static int scroll_text(buffer_t *buf, int offset);


/* Must be called after any movement {left,right,up,down}
 * XXX - Should perhaps be in display.c?
 */
void adjust_virtual_screen(buffer_t *buf)
{
	/* left */
	while ((buf->screen.x - buf->screen.hpos) <= 0 && buf->screen.hpos >= 5)
		buf->screen.hpos -= 5;

	/* right */
	while ((buf->screen.x - buf->screen.hpos) >= (buf->screen.max_col - 1))
		buf->screen.hpos += 5;

	/* up */
	while ((buf->screen.y - buf->screen.vpos) <= 0 && buf->screen.vpos >= 5)
		buf->screen.vpos -= scroll_text(buf, -5);

	/* down */
	while ((buf->screen.y - buf->screen.vpos) >= buf->screen.max_row)
		buf->screen.vpos += scroll_text(buf, 5);
}

/*
 * left():
 * @buf: The current buffer state.
 *
 * Moves the cursor left one character.
 */
void left(buffer_t *buf)
{
	if (move_to_prev_char(&buf->core)) {
		if (10 == get_current_char(&buf->core)) {
			buf->screen.x = get_current_col(&buf->core);
			buf->screen.y--;
		} else {
			buf->screen.x--;
		}
	}

	adjust_virtual_screen(buf);
}


/*
 * right():
 * @buf: The current buffer state.
 *
 * Moves the cursor right one character.
 */
void right(buffer_t *buf)
{
	char ch;

	ch = get_current_char(&buf->core);
	if (!move_to_next_char(&buf->core)) {
		if (10 == ch) {
			buf->screen.hpos = 0;
			buf->screen.x = 0;
			buf->screen.y++;
		} else {
			buf->screen.x++;
		}
	}

	adjust_virtual_screen(buf);
}


/*
 * up():
 * @buf: The current buffer state.
 *
 * Moves the cursor up one line.
 */
void up(buffer_t *buf)
{
	if (!move_to_prev_line(&buf->core)) {
		buf->screen.y--;
		buf->screen.x = move_to_col(&buf->core, buf->screen.x);
	}

	adjust_virtual_screen(buf);
}

/*
 * down():
 * @buf: The current buffer state.
 *
 * Moves the cursor down one line.
 */
void down(buffer_t *buf)
{
	if (!move_to_next_line(&buf->core)) {
		buf->screen.y++;
		buf->screen.x = move_to_col(&buf->core, buf->screen.x);
	}

	adjust_virtual_screen(buf);
}


void end_of_line(buffer_t *buf)
{
	move_to_eol(&buf->core);
	buf->screen.x = get_current_col(&buf->core);

	adjust_virtual_screen(buf);
}

void beginning_of_line(buffer_t *buf)
{
	move_to_bol(&buf->core);
	buf->screen.x = 0;

	adjust_virtual_screen(buf);
}

void page_up(buffer_t *buf)
{
	int i;
	int offset = buf->screen.max_row - 1;	/* Move only half of screen */

	for (i = 0; i < offset; i++) {
		if (move_to_prev_line(&buf->core))
			break;

		buf->screen.y--;
	}

	buf->screen.x = move_to_col(&buf->core, buf->screen.x);
	adjust_virtual_screen(buf);
}

void page_down(buffer_t *buf)
{
	int i;
	int offset = buf->screen.max_row - 1;	/* Move only half of screen */

	for (i = 0; i < offset; i++) {
		if (move_to_next_line(&buf->core))
			break;

		buf->screen.y++;
	}

	buf->screen.x = move_to_col(&buf->core, buf->screen.x);
	adjust_virtual_screen(buf);
}

/* try moving the screen vertically by @offset */
static int scroll_text(buffer_t *buf, int offset)
{
	int counter = 0;

	if (0 > offset) {
		while (offset) {
			if (move_to_prev_line(buf->screen.top))
				return counter;

			counter++;
			offset++;
		}
	} else {
		while (offset) {
			if (move_to_next_line(buf->screen.top))
				return counter;

			counter++;
			offset--;
		}
	}

	return counter;
}



/**
 * allocate_new_gap() - Feed cursor
 * @buf: Pointer to the current edit buffer.
 *
 * Whenever the gap is filled it needs to be reallocated. This function
 * takes care of everything - fixing all pointers and keeping the whole
 * consistent. Messy code to follow ... /Jocke
 *
 */
static void allocate_new_gap(buffer_t *buf)
{
	int   newsize   = buf->core.buffer_size + GAP_SIZE;
	char *first     = buf->core.buffer;
	int   firstlen  = buf->core.gap - buf->core.buffer;
	char *second    = &(buf->core.gap[buf->core.gap_size]);
	int   secondlen = buf->core.buffer + buf->core.buffer_size - second;
	char *gap;

	gap = calloc(newsize, sizeof(char));
	if (!gap) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	memcpy(gap, first, firstlen);
	memcpy(&(gap[firstlen + GAP_SIZE]), second, secondlen);

	free(buf->core.buffer);
	buf->core.buffer      = gap;
	buf->core.buffer_size = newsize;
	buf->core.gap         = gap + firstlen + GAP_SIZE;
	buf->core.gap_size    = GAP_SIZE;
}


/**
 * insert() - Insert a charcter/command at the current cursor position.
 * @buf: The current buffer state.
 * @ch:  The character/command to insert.
 */
void insert(buffer_t *buf, int ch)
{
	if (0 == buf->core.gap_size)
		allocate_new_gap(buf);

	buf->core.gap[0] = (char)ch;
	buf->core.gap++;
	buf->core.gap_size--;

	buf->dirty = 1;

	switch (ch) {
	case 10:		/* NL */
		buf->screen.x = 0;
		buf->screen.y++;
		break;

	case 9:		/* HT, TAB */
		buf->screen.x += 8;
		break;

	default:
		buf->screen.x++;
		break;
	}

	/* Byte offset always increased by one,
	 * we store HT/TAB as only one char.
	 */
	buf->core.position++;


	adjust_virtual_screen(buf);
}


/*
 * delete():
 * @buf: The current buffer state.
 *
 * Delete the charcter/command at the current cursor position.
 */
void delete(buffer_t *buf)
{
	/* Detect if gap at end of buffer */
	if (GAP_AT_END(&buf->core)) {
		/* Cursor at end of buffer, no delete */
	} else {
		buf->core.gap_size++;
		buf->dirty = 1;
	}

	adjust_virtual_screen(buf);
}


void set_mark(buffer_t *buf)
{
	buf->core.mark = buf->core.gap;
}

void set_point(buffer_t *buf)
{
	buf->core.point = buf->core.gap;
}

void cut(buffer_t *buf)
{
	set_point(buf);

	/* Do cutting of text here. */
}

void copy(buffer_t *buf)
{
	set_point(buf);
}

/* here is where the actual copying of the
 * previously marked text is done.
 */
void paste(buffer_t *buf)
{
	char *pos = buf->core.mark;

	while (pos < buf->core.point) {
		insert(buf, pos[0]);
		pos++;
	}
}

/**
 * editor_chargen - Generate data to display engine
 * @buf: Pointer to the current buffer.
 *
 * Generate a screenful of data from @buf->screen.top to be written to the
 * display.
 *
 * Returns:
 * A dynamically allocated buffer to be freed after usage.
 */
char *editor_chargen(buffer_t *buf)
{
	char curr;
	char *genbuf = NULL;
	int i, j, v_pos, h_pos, h_pos_max = buf->screen.hpos + buf->screen.max_col;
	int nofbytes = (buf->screen.max_col) * (buf->screen.max_row);
	text_t text;		/* traverses the current virtual screen */

	memcpy(&text, buf->screen.top, sizeof(text));

	/* We want an array of characters that is max_col * max_row large.
	 * if we cannot get that much we want at least to have
	 * an array with lots of NL and ended with a NUL.
	 */
	genbuf = calloc(nofbytes, sizeof(char));
	if (!genbuf) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	for (i = 0, v_pos = 0; v_pos < buf->screen.max_row; v_pos++) {
		/* Adjust for horizontal starting position */
		h_pos = 0;
		while (10 != get_current_char(&text) && h_pos < buf->screen.hpos) {
			move_to_next_char(&text);
			h_pos++;
		}
		for (; h_pos < h_pos_max; h_pos++) {
			/* End of buffer? */
			if (text.gap == &(buf->core.buffer[buf->core.buffer_size])) {
				/* set exit condition for outer for() */
				v_pos = buf->screen.max_row;

				/* break out of inner for() */
				break;
			}

			/* Inside the real gap? Skip the real gap! */
			if (text.gap == buf->core.gap)
				text.gap += buf->core.gap_size;

			curr = get_current_char(&text);
			switch (curr) {
			case 10:	/* NL */
				h_pos = h_pos_max;
				break;

			case 9:	/* TAB */
				for (j = 0; j < 8; j++)
					genbuf[i++] = ' ';
				break;

			default:
				genbuf[i++] = curr;
			}
			if (curr != 10)
				move_to_next_char(&text);
		}
		/* Always end every line with NL */
		genbuf[i++] = 10;	/* NL */

		move_to_next_line(&text);
	}

	/* Always terminate the buffer. */
	genbuf[i] = 0;

	return genbuf;
}


/**
 * backup_buffer_pos() - Save current cursor position.
 * @text:        Active text buffer
 *
 * Saves current setting of the gap for the below
 * function to be able to restore it later.
 *
 * Returns:
 * Current gap pos.
 */
static char *backup_buffer_pos(text_t *text)
{
	return text->gap;
}

/**
 * restore_buffer_pos() - Restore last cursor position.
 * @text:    Active text buffer
 * @bsackup: Saved position
 *
 * Recalls old setting of the gap.
 *
 * Returns:
 * Gap pos. prior to running this func.
 */
static char *restore_buffer_pos(text_t *text, char *backup)
{
	char *gap_bis = text->gap;

	/* Also fix the byte offset. */
	text->position += backup - text->gap;

	memmove(backup, text->gap, text->gap_size);
	text->gap = backup;

	return gap_bis;
}

/**
 * move_to_next_char() - Move gap to the next character.
 * @text: The current buffer state.
 *
 * Simply moves the gap/cursor to the next character. Only one possibility
 * of failure; when the gap is at the very end of the buffer.
 *
 * Returns:
 * 0 if the operation was successful;
 * 1 otherwise.
 */
static int move_to_next_char(text_t *text)
{
	if (GAP_AT_END(text))
		return 1;

	text->gap[0] = text->gap[text->gap_size];
	text->gap++;

	text->position++;

	return 0;
}


/**
 * move_to_prev_char() - Move gap to the previous character.
 * @text: The current buffer state.
 *
 * Simply moves the gap/cursor to the previous character.  Only one
 * possibility of failure; when the gap is as far as it can get to the
 * "left on the tape", i.e. when it is at the beginning of the file.
 *
 * Returns:
 * 0 if the operation was successful;
 * 1 otherwise.
 */
static int move_to_prev_char(text_t *text)
{
	if (GAP_AT_START(text))
		return 1;

	text->gap--;
	text->gap[text->gap_size] = text->gap[0];

	text->position--;

	return 0;
}


static int move_to_next_line(text_t *text)
{
	char *backup = backup_buffer_pos(text);

	move_to_eol(text);
	if (move_to_next_char(text)) {
		restore_buffer_pos(text, backup);
		return 1;
	}

	return 0;
}


static int move_to_prev_line(text_t *text)
{
	char *backup = backup_buffer_pos(text);

	/* move_to_bol() should never (!) fail ... */
	move_to_bol(text);
	if (move_to_prev_char(text)) {
		restore_buffer_pos(text, backup);
		return 1;
	}

	/* ... neither should it fail here ... I hope! */
	move_to_bol(text);

	return 0;
}

/**
 * get_current_char() - Retrieve the character which the gap is currently located "over".
 * @text: The current buffer state.
 *
 * Returns:
 * The character just outside the gap to the right.
 */
static char get_current_char(text_t *text)
{
	return text->gap[text->gap_size];
}

/**
 * get_current_col() - Get cursor X position
 * @text: Current state of buffer.
 *
 * This little "hack" calculates where the cursor is located x-wise. It
 * is actually just a function that can be used to determine the line
 * length. Which makes it extremely useful in a buffer-gap implementation.
 *
 * Returns:
 * The character position.
 */
static int get_current_col(text_t *text)
{
	char curr;
	int counter = 0;

	char *backup = backup_buffer_pos(text);

	while (!move_to_prev_char(text)) {
		curr = get_current_char(text);
		if (10 == curr) {
			move_to_next_char(text);
			break;
		}
		if (9 == curr) {
			counter += 8;
			continue;
		}
		/* else */
		counter++;
	}

	restore_buffer_pos(text, backup);

	return counter;
}


/**
 * move_to_eol() - Attempt to move the cursor to the end of the line.
 * @text: The current buffer state.
 *
 * 1) We're already at EOL -> OK
 * 2) While not at EOL move_to_next_char (if that's possible)
 *
 * Returns:
 * 0 if the operation was successful;
 * 1 otherwise.
 */
static int move_to_eol(text_t *text)
{
	while (10 != get_current_char(text))
		if (move_to_next_char(text))
			return 1;

	return 0;
}


/**
 * move_to_bol() - Move the cursor to the first column of the current line.
 * @text: The current buffer state.
 *
 * There is really only one problem and that is when you are at the
 * beginning of the file. No previous nodes or previous newlines...
 * But none of these are really no problem since you always can find the
 * beginning of a line as well as the sister function move_to_eol()!
 *
 * Returns:
 * 0 if the operation was successful;
 * 1 otherwise.
 */
static int move_to_bol(text_t *text)
{
	while (!move_to_prev_char(text)) {
		if (10 == get_current_char(text)) {
			move_to_next_char(text);
			return 0;
		}
	}

	return 0;
}


/**
 * move_to_stx() - Move cursor to the beginning/start of the text.
 * @text: The current buffer state.
 *
 * Returns:
 * 0 if the operation was successful;
 * 1 otherwise.
 */
static int move_to_stx(text_t *text)
{
	if (GAP_AT_START(text))
		return 0;

	memmove(text->buffer, text->gap, text->gap_size);

	return 0;
}


/**
 * move_to_col() - Move the gap to the specified column.
 * @text: The current state of the buffer.
 * @col:  The new column.
 *
 * Returns:
 * How far the function managed to move the gap on the current line.
 */
static int move_to_col(text_t *text, int col)
{
	int counter = 0;

	/* This should NEVER happen!! Semantically that is ... :/ */
	if (move_to_bol(text))
		return counter;

	for (counter = 0; (counter < col) && (10 != get_current_char(text)); counter++)
		move_to_next_char(text);

	return counter;
}

/**
 * goto_line() - move gap to @line in current buffer
 */
void goto_line(buffer_t *buf, int line)
{
	int offset = line - buf->screen.y;

	if (offset > 0) {
		while (offset--) {
			if (move_to_next_line(&buf->core))
				break;

			buf->screen.y++;
		}
		buf->screen.x = 0;
	} else {
		offset = 1 - offset;
		while (offset--) {
			if (move_to_prev_line(&buf->core))
				break;

			buf->screen.y--;
		}
		buf->screen.x = 0;
	}

	adjust_virtual_screen(buf);
}

/**
 * search() - i-search
 * @buf:     buffer to search in
 * @pattern: pattern to search for
 * @dir:     forward or backwards
 */
void search(buffer_t *buf, char *pattern, int dir)
{
	char *pos;

	/* XXX - ignore direction, always forward currently */
	pos = strstr(&buf->core.gap[buf->core.gap_size], pattern);
	if (!pos) {
		/* not found ... */
		/* XXX - display this error in status field */
	} else {
		int i = pos - buf->core.gap - buf->core.gap_size;

		while (i--)
			right(buf);

		adjust_virtual_screen(buf);
	}
}

int file_new(buffer_t *buf, size_t len)
{
	/* Allocate a data buffer */
	buf->core.buffer_size = len + GAP_SIZE;
	buf->core.buffer = calloc(buf->core.buffer_size, sizeof(char));
	if (!buf->core.buffer) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	/* Initial size and location of the gap */
	buf->core.gap = buf->core.buffer;
	buf->core.gap_size = GAP_SIZE;

	return 0;
}

/**
 * file_read() - Read file into buffer
 * @buf: Pointer to the new buffer core.
 * @fp:  Regular file pointer.
 *
 * Reads the current file into the array used in the buffer-gap scheme by
 * BUFFER_GAP. Very simple, indeed.
 */
int file_read(buffer_t *buf, FILE *fp)
{
	size_t len;
	struct stat filestatus;

	if (0 != fstat(fileno(fp), &filestatus)) {
		LOG("Could not fstat file before opening!\n");
		return -1;
	}

	file_new(buf, filestatus.st_size);

	/* Start reading in the file after the gap. */
	len = fread(&(buf->core.buffer[GAP_SIZE]), filestatus.st_size, 1, fp);
	if (len != filestatus.st_size)
		LOG("file_read() - Read: %zd, file size: %zd?\n", len, filestatus.st_size);

	return 0;
}

/**
 * file_save():
 * @buf: Ponter to the buffer.
 * @filename: The filename to be used for saving.
 *
 * This function can save the buffer-gap structure of BUFFER_GAP to a file.
 *
 * Returns:
 * True/False depending on the result.
 */
int file_save(buffer_t *buf, char *filename)
{
	size_t len;
	FILE *fp;
	unsigned int bs, gs;
	char *b, *g;

	fp = fopen(filename, "w");
	if (!fp) {
		LOG("Could not open %s for saving.\n", filename);
		return 0;
	}

	g = buf->core.gap;
	b = buf->core.buffer;
	gs = buf->core.gap_size;
	bs = buf->core.buffer_size;

	/* XXX: Check this ... */
	if (g == b) {
		len = fwrite(&(g[gs]), bs - gs, 1, fp);
		if (bs - gs != len)
			LOG("file_save() - Only wrote %zd bytes to %s!\n", len, filename);
	} else {
		len = fwrite(b, g - b, 1, fp);
		if (b - g != len)
			LOG("file_save() - Only wrote %zd bytes to %s!\n", len, filename);

		len = fwrite(&(g[gs]), bs - gs - (g - b), 1, fp);
		if (bs - gs - (g - b) != len)
			LOG("file_save() - Only wrote %zd bytes to %s!\n", len, filename);
	}

	fclose(fp);

	buf->dirty = 0;

	return 1;
}

void screen_new(buffer_t *buf)
{
	/* BUFFER_GAP needs this copy for the virtual screen */
	buf->screen.top = calloc(sizeof(text_t), sizeof(char));
	if (!buf->screen.top) {
		perror("Failed allocating new buffer");
		exit(1);
	}

	memcpy(buf->screen.top, &buf->core, sizeof(text_t));

	/* Invisible cursor that indicates top of screen */
	buf->screen.top->gap_size = 0;

	buf->core.position = 0;
	buf->screen.x = 0;
	buf->screen.y = 0;
}

void buffer_free(buffer_t *buf)
{
	free(buf->filename);
	free(buf->core.buffer);
}

#endif /* BUFFER_GAP_MODE */




#ifdef TOTALLY_NEW_COOL_STUFF
/*
 * move_to_linepos():
 * Moves to the first visible character on a row.
 * Assumes that the gap is located over the first character on a visible row.
 *
 * Returns: 1 if something went wrong, i.e. no more visible chars. on row.
 *          0 if everything is OK.
 */
static int move_to_linepos(ptr *the_buffer, int hpos)
{
	int counter = 0;

	while (counter < hpos) {
		if (10 == get_current_char(the_buffer))
			return 1;
		if (move_to_next_char(the_buffer))
			return 1;
	}

	return 0;
}

/*
 * draw_line():
 * Assumes that the gap is located over the first visible character
 * on a visible line that is to be printed to the virtual screen.
 */
static void draw_line(ptr screen, ptr *the_buffer, int screen_y, int xmax)
{
	int screen_x = 0;
	char character;

	while (screen_x < xmax) {
		character = get_current_char(the_buffer);
		if (10 == character) {
			/* No more characters on this line, fill out with zeroes. */
			while (screen_x < xmax) {
				screen_put_pixchar(screen_x, screen_y, 0);
				screen_x++;
			}
		} else {
			screen_put_pixchar(screen_x, screen_y, character);
			move_to_next_char(the_buffer);
			screen_x++;
		}
	}

	/* Upon exit make sure that the gap/cursor is located over
	 * the first character on the next row.
	 */
	while (10 != character) {
		move_to_next_char(the_buffer);
		character = get_current_char(the_buffer);
	}
	move_to_next_char(the_buffer);
}

/*
 * The screen sez to the core:
 * "Redraw thyself on this square!"
 *
 * The core looks upon the provided square and traverses its
 * "top" of page locator (points to the first visible screen row)
 * and writes to the screen using screen_put_pixchar().
 *
 * Actors in screenplay
 * ====================
 *   the top of screen locator:
 *     top  - recursive definition: "top of page"
 *   dimensions_and_offsets:
 *     hpos - horisontal offset.
 *     vpos - vertical offset.
 *     xmax - Maximum screen width
 *     ymax - Maximum screen height
 *     x    - cursor (insert point/gap) absolute X pos in document
 *     y    - cursor  absolute Y pos in document
 *
 *   screen:
 *     simply the virtual screen buffer with its primitives.
 *
 *   the_buffer:
 *     includes current screen cursor pos and the actual buffer.
 */
void redraw(ptr * screen, ptr * dimensions_and_offsets, ptr * the_buffer)
{
	int sy = 0;

	while (sy < ymax) {
		if (move_to_linepos(the_buffer, hpos))
			/* Failed to move, line not that long -> next line please */
			;
		else
			draw_line(screen, the_buffer, sy, xmax);

		/* Next screen line please */
		sy++;
	}

}
#endif /* TOTALLY_NEW_COOL_STUFF */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
