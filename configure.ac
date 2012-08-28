dnl
dnl  This file is part of CBCBSIM, the Combined Broadcast Content-Based
dnl  (CBCB) routing simulation.
dnl
dnl  Authors: See the file AUTHORS for full details. 
dnl
dnl  Copyright (C) 2002-2004 University of Colorado
dnl
dnl  This program is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU General Public License
dnl  as published by the Free Software Foundation; either version 2
dnl  of the License, or (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
dnl  USA, or send email to serl@cs.colorado.edu.
dnl
dnl $Id: configure.ac,v 1.3 2004/04/30 19:42:41 rutherfo Exp $
dnl
AC_INIT(cbcbsim, 1.0.2, rutherfo@cs.colorado.edu)
AC_PREFIX_DEFAULT(/opt)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE
dnl AC_CONFIG_TESTDIR(tests)

: ${CXXFLAGS="-Wall -g"}

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX( g++ c++ )
AC_PROG_CXXCPP

dnl Checks for libraries.
AC_LANG(C++)

dnl Checks for header files.

dnl Checks for library locations
AC_MSG_CHECKING(custom location of ssim)

AC_ARG_WITH(ssim,
	[ --with-ssim[=PATH] Path to ssim installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	  AC_SUBST(SSIM_DIR,"${withval}")
	],
	[ AC_MSG_RESULT(no)
	])

AC_MSG_CHECKING(custom location of sff)

AC_ARG_WITH(sff,
	[ --with-sff[=PATH] Path to sff installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	  AC_SUBST(SFF_DIR,"${withval}")
	],
	[ AC_MSG_RESULT(no)
	])

AC_MSG_CHECKING(custom location of ssimp)

AC_ARG_WITH(ssimp,
	[ --with-ssimp[=PATH] Path to ssimp installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	  AC_SUBST(SSIMP_DIR,"${withval}")
	],
	[ AC_MSG_RESULT(no)
	])

AC_MSG_CHECKING(custom location of ssbg)

AC_ARG_WITH(ssbg,
	[ --with-ssbg[=PATH] Path to ssbg installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	  AC_SUBST(SSBG_DIR,"${withval}")
	],
	[ AC_MSG_RESULT(no)
	])

AC_MSG_CHECKING(custom location of cbnsim)

AC_ARG_WITH(cbnsim,
	[ --with-cbnsim[=PATH] Path to cbnsim installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	  AC_SUBST(CBNSIM_DIR,"${withval}")
	],
	[ AC_MSG_RESULT(no)
	])


dnl Checks for library functions.
AC_CHECK_LIB(ssim,main,
	[ LIBS="-lssim $LIBS"
	],
	[ AC_MSG_ERROR( ["ssim was not found. use --with-ssim if it is located in a non-standard directory."] )
	])

AC_CHECK_LIB(sff,main,
	[ LIBS="-lsff $LIBS"
	],
	[ AC_MSG_ERROR( ["sff was not found. use --with-sff if it is located in a non-standard directory."] )
	])

AC_CHECK_LIB(ssimp,main,
	[ LIBS="-lssimp $LIBS"
	],
	[ AC_MSG_ERROR( ["ssimp was not found. use --with-ssimp if it is located in a non-standard directory."] )
	])

AC_CHECK_LIB(ssbg,main,
	[ LIBS="-lssbg $LIBS"
	],
	[ AC_MSG_ERROR( ["ssbg was not found. use --with-ssbg if it is located in a non-standard directory."] )
	])

AC_CHECK_LIB(cbnsim,main,
	[ LIBS="-lcbnsim $LIBS"
	],
	[ AC_MSG_ERROR( ["cbnsim was not found. use --with-cbnsim if it is located in a non-standard directory."] )
	])

AC_MSG_CHECKING(inclusion of debug messages)
ac_debugs=no
AC_ARG_ENABLE( debugs,
  AC_HELP_STRING([--enable-debugs], [enable evaluation of debug messages (default is NO)]),
  [ ac_debugs=${enableval}
  ])
AC_MSG_RESULT($ac_debugs)
if test "$ac_debugs" = yes; then
  AC_DEFINE(CBNDEBUG_ON, 1, [Enables debug message code])
fi

AC_MSG_CHECKING(inclusion of assertions)
ac_asserts=yes
AC_ARG_ENABLE( assertions,
  AC_HELP_STRING([--enable-assertions], [enable evaluation of assertions (default is YES)]),
  [ ac_asserts=${enableval}
  ])
AC_MSG_RESULT($ac_asserts)
if test "$ac_asserts" = no; then
  AC_DEFINE(CBNASSERT_OFF, 1, [Disables assertion code])
fi

AC_PATH_PROG(SH, sh, /bin/sh)
AC_PATH_PROG(FALSE, false, /usr/bin/false)
AC_PATH_PROG(TRUE, true, /usr/bin/true)
AC_PATH_PROG(GNUPLOT, gnuplot, $FALSE)
AC_PATH_PROG(MV, mv, $FALSE)
AC_PATH_PROG(ECHO, echo, $FALSE)
AC_PATH_PROG(GREP, grep, $FALSE)
AC_PATH_PROG(TEST, test, $FALSE)
AC_PATH_PROG(CP, cp, $FALSE)
AC_PATH_PROG(CAT, cat, $FALSE)
AC_PATH_PROG(CUT, cut, $FALSE)
AC_PATH_PROG(AUTOM4TE, autom4te, $FALSE)
AC_PATH_PROG(DIFF, diff, $FALSE)

dnl output
AC_OUTPUT([
	Makefile 
	exp/Makefile
	include/Makefile
	include/cbcbsim/Makefile
	src/Makefile
	])
dnl	tests/Makefile
dnl	tests/atlocal
