#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(ghac, 0.2.0, bjoern@bjoern-b.de)
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(ghac, 0.2.0)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_INT16_T
AC_TYPE_INT8_T
AC_STRUCT_TM
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([floor memset strchr strstr])

AC_SUBST(EXTRA_CFLAGS)
AC_SUBST(EXTRA_LDFLAGS)
#glib
PKG_CHECK_MODULES([glib], glib-2.0 >= 2.10)
AC_SUBST(glib_LIBS)
AC_SUBST(glib_CFLAGS)

#gtk
PKG_CHECK_MODULES([gtk], gtk+-2.0 >= 2.12)
AC_SUBST(gtk_LIBS)
AC_SUBST(gtk_CFLAGS)

PKG_CHECK_MODULES([libssl], libssl >= 0.9.8g)
AC_SUBST(libssl_LIBS)
AC_SUBST(libssl_CFLAGS)

win32=no
case $host in
  *-*-mingw32* | *-*-windows)
    win32=yes
    EXTRA_CFLAGS="$EXTRA_CFLAGS -mno-cygwin -mwindows -mms-bitfields"
    EXTRA_LDFLAGS="-mwindows -mms-bitfields" ;;
esac

AM_CONDITIONAL(WIN32, test x$win32 = xyes)
AC_ARG_ENABLE(libhagraph,
              [  --disable-libhagraph			Graph drawing support.],
              [ case "${enableval}" in
                    yes)    enable_libhagraph=yes;;
                     no)    enable_libhagraph=no;;
                      *)    AC_MSG_ERROR([bad value ${enableval} for --disable-libhagraph]);;
                esac],
              [enable_libhagraph=yes])

if test x${enable_libhagraph} = xyes; then
	PKG_CHECK_MODULES([libhagraph], libhagraph)
	AC_SUBST(libhagraph_LIBS)
	AC_SUBST(libhagraph_CFLAGS)
	AC_DEFINE(ENABLE_LIBHAGRAPH, 1, [Enable Graph Drawing Support])

fi
AM_CONDITIONAL(ENABLE_LIBHAGRAPH, test x$enable_libhagraph = xyes)

AC_ARG_ENABLE(libhac,
              [  --disable-libhac			Home automation client support.],
              [ case "${enableval}" in
                    yes)    enable_libhac=yes;;
                     no)    enable_libhac=no;;
                      *)    AC_MSG_ERROR([bad value ${enableval} for --disable-libhac]);;
                esac],
              [enable_libhac=yes])

if test x${enable_libhac} = xyes; then
	PKG_CHECK_MODULES([libhac], libhac)
	AC_SUBST(libhac_LIBS)
	AC_SUBST(libhac_CFLAGS)
	AC_DEFINE(ENABLE_LIBHAC, 1, [Enable Home Automation Client Support])

fi
AM_CONDITIONAL(ENABLE_LIBHAC, test x$enable_libhac = xyes)


AC_OUTPUT(Makefile src/Makefile)

if test x${enable_libhagraph} = xyes; then
	echo "Use libhagraph:  enabled"
else
	echo "Use libhagraph:  disabled"
fi

if test x${enable_libhac} = xyes; then
	echo "Use libhac:      enabled"
else
	echo "Use libhac:      disabled"
fi
