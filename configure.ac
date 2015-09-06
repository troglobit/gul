# Use v2.61 for backwards compat with Ubuntu 12.04 LTS
AC_PREREQ([2.61])
AC_INIT([gul],[0.2.0-pre],[https://github.com/troglobit/gul/issues])
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([include/settings.h])
AC_CONFIG_FILES([Makefile src/Makefile man/Makefile])

dnl Properly define at least some contents of CFLAGS before AC_PROG_CC
dnl is called, otherwise it'll define CFLAGS="-g -O2".  We want *no*
dnl optimizations when -g.  Also, set DEBUG=1 in debug mode, for LOG()
CPPFLAGS+=" -D_GNU_SOURCE"
AC_ARG_ENABLE([debug],
    [  --enable-debug                 include symbol table, enable gul.log debug msgs],
    [CFLAGS+=" -g -O0"; CPPFLAGS+=" -DDEBUG=1"], [CFLAGS+=" -O2"])

AC_PROG_CC
AC_PROG_INSTALL

AC_HEADER_STDC
AC_CHECK_HEADERS(errno.h features.h netdb.h stdio.h syslog.h ncurses.h)
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h string.h)

AC_ARG_WITH(slang, [  --with-slang[=DIR]        use S-Lang instead of ncurses],
        [

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
AC_ARG_WITH(curses, [  --with-curses=DIR       path to where ncurses is installed],
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

        )
	])

old_LIBS="$LIBS"
LIBS="$LIBS $GULLIBS"
])

AC_OUTPUT
