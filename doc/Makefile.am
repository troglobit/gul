# Makefile.am - Makefile template for the manual.
# Copyright (C) 2003 Free Software Foundation, Inc.
# Copyright (C) 2004 Joachim Nilsson
# Written by Neal H. Walfield and modified for the
# GUL project by Joachim Nilsson.
#
# This file is part of GUL.
#
# GUL is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published
# by the Free Software Foundation; either version 2 of the License,
# or (at your option) any later version.
#
# GUL is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to:
#   The Free Software Foundation, Inc.
#   59 Temple Place - Suite 330
#   Boston, MA 02111-1307
#   USA

SUFFIXES       = .ps .dvi .tex .eps .fig .pdf

tex_files      = spec.tex

pkgdata_DATA   = spec.dvi spec.ps spec.pdf

fig_files      = overview.fig
eps_files      = overview.eps
eps_cleanfiles = ${eps_files}

EXTRA_DIST     = ${fig_files} ${eps_files} ${tex_files} spec.ps spec.pdf \
		 buffer-gap.txt \
                 dec_vt100_codes.txt \
		 virtual_screen.txt \
		 vt100-easy.txt \
		 vt100-esc-codes-easy2.txt \
		 vt100_codes_news.txt \
		 vt220-graphics.txt \
		 undo.txt

# _DEPENDENCIES does not work with _DATA.
spec.dvi: ${eps_files} ${tex_files}

.tex.dvi:
	$(LATEX) $< -o $@
	while egrep "^LaTeX Warning:.*Rerun to" \
	    `echo $< | sed -e 's/.tex$$/.log/'`; do \
	  rm -f $(subst $<,.tex,.log); \
	  $(LATEX) $< -o $@; \
	done

.dvi.ps:
	$(DVIPS) $<

.ps.pdf:
	$(PS2PDF) $<

.fig.eps:
	$(FIG2DEV) -L eps $< $@

clean:
	rm -f *.aux *.ps *.dvi *.pdf *.log *.toc

MAINTAINERCLEANFILES=${eps_cleanfiles}
