SUFFIXES       = .ps .dvi .tex .eps .fig .pdf
tex_files      = spec.tex
pkgdata_DATA   = spec.dvi spec.ps spec.pdf
fig_files      = overview.fig
eps_files      = overview.eps
eps_cleanfiles = ${eps_files}
EXTRA_DIST     = ${fig_files} ${eps_files} ${tex_files} spec.ps spec.pdf	\
		 buffer-gap.txt							\
                 dec_vt100_codes.txt						\
		 virtual_screen.txt						\
		 vt100-easy.txt							\
		 vt100-esc-codes-easy2.txt					\
		 vt100_codes_news.txt						\
		 vt220-graphics.txt						\
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
