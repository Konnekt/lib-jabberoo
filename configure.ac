## $Id: configure.ac,v 1.7 2004/06/19 02:56:11 julian Exp $
##
## Copyright (c) 2004 Julian Missig <julian@jabber.org>
##

AC_INIT([jabberoo], 1.9.4.1, [http://www.jabberstudio.org/projects/gabber/bugs/])

AC_PREREQ([2.53])

JABBEROO_AGE=1
JABBEROO_REVISION=9
JABBEROO_CURRENT=4
AC_SUBST(JABBEROO_AGE)
AC_SUBST(JABBEROO_REVISION)
AC_SUBST(JABBEROO_CURRENT)

AC_REVISION([$Revision: 1.7 $])
AC_CONFIG_SRCDIR([src/jabberoo-packet.cc])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER([config.h])
## ^ AC_CONFIG_HEADERS instead? seems to be an automake-1.7 thing

## AC_SUBST([ACLOCAL_AMFLAGS], ['-I macros ${ACLOCAL_FLAGS}'])

AC_LANG_CPLUSPLUS()

AC_PROG_CC()
AC_PROG_CXX()
AC_PROG_LIBTOOL()
AM_PROG_LIBTOOL()

dnl Checks for stdc++ and typedefs, structures, etc
AC_CHECK_LIB(stdc++)
AC_TYPE_SIZE_T()

dnl Checks for library functions
AC_FUNC_MEMCMP()
AC_CHECK_FUNCS(timegm,[AC_DEFINE([HAVE_TIMEGM])])

dnl CFLAGS for release and devel versions
CFLAGS="-Wall"
CXXFLAGS="-Wall"
AC_ARG_ENABLE(debug,
        [  --enable-debug          compile with debugging (for development)],
        debug=$enable_debug
)
AC_MSG_CHECKING(for debugging)
if test "x$debug" = xyes; then
        AC_MSG_RESULT(yes)
        BUILD_TYPE="$BUILD_TYPE Debug"
        CFLAGS="$CFLAGS -g"
	CXXFLAGS="$CXXFLAGS -g"
else
	CFLAGS="$CFLAGS -O2"
	CXXFLAGS="$CXXFLAGS -O2"
        AC_MSG_RESULT(no)
fi

                                               

dnl Check for endianess
AC_C_BIGENDIAN() 

dnl ***************************
dnl Process libsigcpp if needed


PKG_CHECK_MODULES([SIGC], sigc++-2.0 >= 2.0 ,[SIGCPC="sigc++-2.0"],[PKG_CHECK_MODULES([SIGC],
                  sigc++-1.2 < 2.0, AC_DEFINE([OLD_SIGC],,"Using the old sigc++") SIGCPC="sigc++-1.2")])
AC_SUBST(SIGCPC)

AM_CONDITIONAL(DEBUG, test x$debug = xyes)
test=no
AM_CONDITIONAL(TEST, test x$test = xyes)
AC_SUBST(SIGC_CFLAGS)
AC_SUBST(SIGC_LIBS)

dnl ***************************

AC_OUTPUT([
Makefile
doxygen.cfg
jabberoo.pc
libjudo/Makefile
libjudo/src/Makefile
libjudo/src/expat/Makefile
libjudo/src/test/Makefile
include/Makefile
src/Makefile
])
