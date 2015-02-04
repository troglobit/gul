dnl Process this file with autoconf to produce a configure script.

dnl Every other copy of the package version number gets its value from here
AC_INIT([gul],[0.1.2],[http://github.com/troglobit/gul/])

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_HEADERS([include/settings.h])
AC_CONFIG_SRCDIR(configure.ac)

dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Add _GNU_SOURCE to enable features and activate legacy API.
CFLAGS="${CFLAGS} -D_GNU_SOURCE"

dnl Here is a check for GCC which sets all necessary flags, most notably
dnl CFLAGS and CXXFLAGS. Checks for other compilers should do the same.
if test "$GCC" = "yes" ; then
    # set GCC compiler options that should work with all versions
    CFLAGS="${CFLAGS} -Wall -pedantic -fpic -Wno-long-long"
    SHAREDLIB_LDFLAGS="-shared ${SHAREDLIB_LDFLAGS}"
    DEPEND_CCFLAGS="-MM"

    # -std=c99 is only supported in later releases of GCC and causes
    # Linux headers to go into strict ANSI mode, dropping non-ANSI functions
    # so we have to fall back to -std=gnu99 if c99 doesn't work
    save_CFLAGS="${CFLAGS}"
    CFLAGS="${CFLAGS} -std=c99 -Werror-implicit-function-declaration"
    AC_MSG_CHECKING([whether GNU cc -std=c99 option works])
    AC_COMPILE_IFELSE([ AC_LANG_PROGRAM([[#include <string.h> char *a; a = strdup("foo");]])],
	AC_MSG_RESULT([yes]) ; CFLAGS="$save_CFLAGS -std=c99",
	AC_MSG_RESULT([no])
	CFLAGS="$save_CFLAGS -std=gnu99 -Werror-implicit-function-declaration"
	AC_MSG_CHECKING([whether GNU cc -std=gnu99 option works])
	AC_COMPILE_IFELSE([ AC_LANG_PROGRAM([[#include <string.h> char *a; a = strdup("foo");]]) ],
		AC_MSG_RESULT([yes]) ; CFLAGS="$save_CFLAGS -std=gnu99",
		AC_MSG_RESULT([no]) ; CFLAGS="$save_CFLAGS"))

    # Override autoconf default addition of -O2 and let the
    # --enable-optimizer-flags decide whether to set it or not
    OPTIMIZER_CFLAGS="-O2"
    OPTIMIZER_CXXFLAGS="-O2"
    CFLAGS=`echo ${CFLAGS} | sed 's/-O2//'`
    CXXFLAGS=`echo ${CXXFLAGS} | sed 's/-O2//'`
fi

dnl Enable/disable stuff
AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],[Enable debugging with GDB [default=no]]),[
CFLAGS="$CFLAGS -g -DDEBUG=1"],[
CFLAGS="$CFLAGS $OPTIMIZER_CFLAGS"
LDFLAGS="-s"])

AC_ARG_ENABLE(pedantic-compiler,
              AS_HELP_STRING([--enable-pedantic-compiler],[Enable pedantic checks in GCC [default=no]]),
              [CFLAGS="$CFLAGS -Wall -pedantic -Wformat=2 -Wunreachable-code -Winline"])


dnl Checks for documentation
dnl Required for building the documentation
AC_PATH_PROG([LATEX], [latex], no)
if test "x$LATEX" = xno; then
  missing_progs="$missing_progs latex"
fi
AC_PATH_PROG([DVIPS], [dvips], no)
if test "x$DVIPS" = xno; then
  missing_progs="$missing_progs dvips"
fi
AC_PATH_PROG([PS2PDF], [ps2pdf], no)
if test "x$PS2PDF" = xno; then
  missing_progs="$missing_progs ps2pdf"
fi
AC_PATH_PROG([FIG2DEV], [fig2dev], no)
if test "x$FIG2DEV" = xno; then
  if test "x$maintainer_mode" = xyes; then
    missing_progs="missing_progs fig2dev"
  fi
fi


dnl Checks for libraries.
AC_ARG_WITH(slang, [  --with-slang[=DIR]         Use S-Lang instead of ncurses],
        [AC_CACHE_CHECK([if this is a BSD system], gul_cv_bsdish,
                [AC_RUN_IFELSE([AC_LANG_SOURCE([[#include <sys/param.h>

main ()
{
#ifdef BSD
        exit (0);
#else
        exit (1);
#endif
}]])],[gul_cv_bsdish=yes],[gul_cv_bsdish=no],[gul_cv_bsdish=no])])

AC_MSG_CHECKING(for S-Lang)
if test $withval = yes; then
        if test -d $srcdir/../slang; then
                gul_cv_slang=$srcdir/../slang/src
                CPPFLAGS="$CPPFLAGS -I${gul_cv_slang}"
                LDFLAGS="$LDFLAGS -L${gul_cv_slang}/objs"
        else
                if test -d $gul_cv_prefix/include/slang; then
                        CPPFLAGS="$CPPFLAGS -I$gul_cv_prefix/include/slang"
                elif test -d /usr/include/slang; then
                        CPPFLAGS="$CPPFLAGS -I/usr/include/slang"
                fi
                gul_cv_slang=yes
        fi
else
        dnl ---Check to see if $withval is a source directory
        if test -f $withval/src/slang.h; then
                gul_cv_slang=$withval/src
                CPPFLAGS="$CPPFLAGS -I${gul_cv_slang}"
                LDFLAGS="$LDFLAGS -L${gul_cv_slang}/objs"
        else
                dnl ---Must be installed somewhere
                gul_cv_slang=$withval
                if test -d $withval/include/slang; then
                        CPPFLAGS="$CPPFLAGS -I${withval}/include/slang"
                elif test -d $withval/include; then
                        CPPFLAGS="$CPPFLAGS -I${withval}/include"
                fi
                LDFLAGS="$LDFLAGS -L${withval}/lib"
        fi
fi
AC_MSG_RESULT($gul_cv_slang)
if test $gul_cv_bsdish = yes; then
        AC_CHECK_LIB(termlib, main)
fi
AC_DEFINE(USE_SLANG_CURSES,1,
        [ Define if you compile with SLang instead of curses/ncurses. ])
AC_DEFINE(HAVE_COLOR,1,[ Define if your curses library supports color. ])
GUL_LIB_OBJECTS="$GUL_LIB_OBJECTS resize.o"

dnl --- now that we've found it, check the link

AC_CHECK_LIB(slang, SLtt_get_terminfo,
        [GULLIBS="$GULLIBS -lslang -lm"],
        [AC_MSG_ERROR(unable to compile.  check config.log)], -lm)

],

[gul_cv_curses=/usr
AC_ARG_WITH(curses, [  --with-curses=DIR          Where ncurses is installed ],
        [if test $withval != yes; then
                gul_cv_curses=$withval
        fi
        if test x$gul_cv_curses != x/usr; then
                LDFLAGS="-L${gul_cv_curses}/lib $LDFLAGS"
                CPPFLAGS="$CPPFLAGS -I${gul_cv_curses}/include"
        fi])

AC_CHECK_FUNC(initscr,,[
cf_ncurses="ncurses"
for lib in ncurses ncursesw
do
	AC_CHECK_LIB($lib, waddnwstr, [cf_ncurses="$lib"; break])
done
AC_CHECK_LIB($cf_ncurses, initscr,
        [GULLIBS="$GULLIBS -l$cf_ncurses"
        if test x$gul_cv_curses = x/usr -a -d /usr/include/ncurses; then
                CPPFLAGS="$CPPFLAGS -I/usr/include/ncurses"
        fi

        if test "$cf_ncurses" = ncursesw; then
		AC_CHECK_HEADERS(ncursesw/ncurses.h,[cf_cv_ncurses_header="ncursesw/ncurses.h"])
	else
		AC_CHECK_HEADERS(ncurses.h,[cf_cv_ncurses_header="ncurses.h"])
	fi],

        [CF_CURSES_LIBS])
	])

old_LIBS="$LIBS"
LIBS="$LIBS $GULLIBS"
])

AC_HEADER_STDC

dnl Check for gul specific includes
AC_CHECK_HEADERS(errno.h features.h netdb.h stdio.h syslog.h ncurses.h)
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )

AC_HEADER_MAJOR

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_CONFIG_FILES([gul.lsm gul.spec \
          Makefile         \
          doc/Makefile     \
          man/Makefile     \
          src/Makefile])
AC_OUTPUT
