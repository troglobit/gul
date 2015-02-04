                    gul - The one true GUL editor!

---[ Description

A first attempt at writing an editor.  A small useful gul (Swedish for
yellow) thing that can run on Spectravideo, Atari ST, Amiga 500 as
well as regular PC's and bisarre workstations.

First steps made back in 1998 on HP-UX 10.20 from a Windows NT 4.0
Workstation using Exceed and XDM-login on one of the HP PA-RISC
servers at work (ABB).

Development continued at home in Red Hat GNU/Linux on a simple old
PentiumOverdrive wich later got upgraded into a Cyrix 6x86 166 MHz.
Which in turn now only works as server to the Laptop PII @300MHz.
Still Red Hat though ... 

Parallell testing done in Solaris at the student body, SdU, using a
wide range of Sun SPARC IPX's SparcStation 2, SPARC 10's.  Testing
have also begun on our SparcStation ELC's. 

That was way back in 1998, now it is 2004 and we sit here with AMD
Athlon's running around 2.0 GHz and finally we've picked up the
pace at developing GUL.  Currently we actually have a working release
that is around 14kB in size.


---[ General information

The documentation/specification is written in LaTeX. For a short
introductory to LaTeX, see Dr. Posts' homepage under 'Courses':
	http://www.idt.mdh.se/personal/kpt/

"LaTeX is the lingua franca among scientists."
"Based on TeX written by Donald Knuth."
"Difficult to learn but very powerful."
"When pronouncing LaTeX your screen is supposed to get really wet..."

                                          -- Dr. K. Post

It's also the GNU projects preferred typesetting/documentation tool and
it really isn't that difficult to learn if you've ever written HTML/XML/SGML.


---[ Installation

GUL use the GNU configure and build system, see the file INSTALL for
help and details concerning your OS and platform.


---[ Running

The editor runs best in a TERM=linux environment but have also been
tested in xterm-windows. The only keys working as of 1999-03-27 is
cursor-keys and delete. Everything else is pretty much untested.
Added:
	native vt100 interface.
	slang support.
	new editor core based on buffer-gap.


---[ Bugs

Plenty. Do something about it! :P
A trace is written to stderr. Redirect the trace to a file 
when running gul. (gul filename 2>logfile) see the source
for more info, as usual.


---[ Files

IDEAS    - Random thoughts
TIPS     - Some hints on what to do when the going gets tough
TODO     - A list of things that needs attention.
INSTALL  - HowTo install and compile both source and specification.
README   - This short intro.
ChangLog - A list of things that already have been done.


---[ License

Copyright (C) 2004 Joachim Nilsson
Copyright (C) 1998-2000 Joachim Nilsson, Jakob Eriksson, Anders Bornäs

gul is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
~
~
~
~
"README" 107 lines, 3436 characters written





