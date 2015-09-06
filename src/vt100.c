/* vt100.c - vt100 plugin for the Gul Editor.
 *
 * Copyright (c) 2004-2015  Joachim Nilsson <troglobit@gmail.com>
 * Copyright (c) 1998-2000  Joachim Nilsson, Jakob Eriksson, Anders Bornäs
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
 * This can be seen as rather simple "tutorial" on how to write other
 * plugins such as SLang and others...
 *
 * 23/4 1999 - Crash <joachim.nilsson@mds.mdh.se>
 * 30/6 1999 - Heavily modified from ncurses to native vt100 by
 *             Jakov <jakov@quicknet.se>
 *
 */


#include "config.h"

#ifdef VT100_PLUGIN

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h>


#include <unistd.h>

#include "vt100.h"
#include "plugin.h"


void mywrite(char *str)
{
	for (; *str; ++str) {
		putchar(*str);
	}
}

void command(char *str)
{
	putchar(27);
	for (; *str; ++str) {
		putchar(*str);
	}
}

char virtual[80][24];
void screen_plugin_init(void)
{
	int x, y;

	command("c");		/* Reset screen. */
	command("[?3l");	/* 80 columns. */
	for (y = 0; y < 24; y++) {
		for (x = 0; x < 80; x++) {
			virtual[x][y] = '\0';
		}
	}
}

/* Call this when quitting from editor. */
void screen_plugin_exit(void)
{
	return;
}

/* Returns screen size at a given moment. */
void screen_plugin_get_dim(int *max_row, int *max_col)
{
	*max_row = 24;
	*max_col = 80;
}


int vt100x, vt100y;
/* Position the cursor */
void screen_plugin_set_cursor(int x, int y)
{
	char str[10];

	sprintf(str, "[%d;%dH", y, x + 1);
	command(str);
}

void gotoxy(int x, int y)
{
	char str[10];

	sprintf(str, "[%d;%dH", y, x + 1);
	command(str);
}


/* Reads from framebuffer/virtual screen and prints to CRT. */
void screen_plugin_update(void)
{
	int x, y;
	char ch;

	command("[1;1H");
	for (y = 0; y < 24; y++) {
		for (x = 0; x < 80; x++) {
			ch = screen_get_pixchar(x, y);
			if (virtual[x][y] != ch) {
				virtual[x][y] = ch;
				gotoxy(x, y);
				if (0 == ch) {
					putchar(' ');
				} else {
					putchar(ch);
				}
			}
		}
	}
}


static struct termios oldmode;
void keyboardNoEcho(void)
{
	struct termios newmode;

	tcgetattr(0, &oldmode);

	memcpy(&newmode, &oldmode, sizeof(struct termios));

	/* Disable canonical mode, and set buffer size to 1 byte */
	newmode.c_lflag &= (~ICANON);
	newmode.c_cc[VTIME] = 0;
	newmode.c_cc[VMIN] = 1;
	newmode.c_lflag &= (~ECHO);

	tcsetattr(0, TCSANOW, &newmode);

	return;
}

void keyboard_plugin_init(void)
{
	keyboardNoEcho();
}

void keyboard_plugin_exit(void)
{
	tcsetattr(0, TCSANOW, &oldmode);

	return;
}

static int isready(int fd)
{
	int rc;
	fd_set fds;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	tv.tv_sec = tv.tv_usec = 0;

	rc = select(fd + 1, &fds, NULL, NULL, &tv);
	if (rc < 0) {
		return -1;
	}

	return FD_ISSET(fd, &fds) ? 1 : 0;
}

/* Nonblocking check, 0 if there is no character. */
keyevent_t processRawKeys(void)
{
	keyevent_t keyreturn;
	char c;

	c = getchar();
	keyreturn.event = GUL_DATA;

	if (27 == c) {
		c = getchar();
		if (91 != c) {	/* ASCII    91  --> '[' */
			keyreturn.event = GUL_NO_EVENT;

			return keyreturn;
		}
		c = getchar();
		switch (c) {
		case VT100_INKEY_UP:
			keyreturn.event = GUL_UP;
		case VT100_INKEY_DOWN:
			keyreturn.event = GUL_DOWN;
		case VT100_INKEY_RIGHT:
			keyreturn.event = GUL_RIGHT;
		case VT100_INKEY_LEFT:
			keyreturn.event = GUL_LEFT;
		default:
			keyreturn.event = GUL_NO_EVENT;
		}
	}
	if (10 == c) {
		/* Translate from terminal sending 10 and meaning CRLF. */
		keyreturn.keydata = 13;

		return keyreturn;
	}
	keyreturn.keydata = c;

	return keyreturn;
}

keyevent_t plugin_read_key(void)
{
	keyevent_t keyreturn;

	if (isready(0)) {
		keyreturn = processRawKeys();
	}

	return keyreturn;
}

#endif				/* VT100_PLUGIN */


/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
