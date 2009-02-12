#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(ghac, 0.2.0, bjoern@bjoern-b.de)
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

#glib
PKG_CHECK_MODULES([glib], glib-2.0 >= 2.10)
AC_SUBST(glib_LIBS)
AC_SUBST(glib_CFLAGS)

#gtk
PKG_CHECK_MODULES([gtk], gtk+-2.0 >= 2.12)
AC_SUBST(gtk_LIBS)
AC_SUBST(gtk_CFLAGS)

# libglade
PKG_CHECK_MODULES([libglade], libglade-2.0)
AC_SUBST(libglade_LIBS)
AC_SUBST(libglade_CFLAGS)

#libhagraph
PKG_CHECK_MODULES([libhagraph], libhagraph >= 0.2.0)
AC_SUBST(libhagraph_LIBS)
AC_SUBST(libhagraph_CFLAGS)

#libhac
PKG_CHECK_MODULES([libhac], libhac >= 0.2.0)
AC_SUBST(libhac_LIBS)
AC_SUBST(libhac_CFLAGS)

AC_OUTPUT(Makefile src/Makefile)
