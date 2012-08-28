dnl
dnl  This file is part of CBNSIM, the Content-Based Networking
dnl  Simulation Library.
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
dnl $Id: configure.ac,v 1.15 2008-04-01 16:34:08 carzanig Exp $
dnl
AC_INIT(cbnsim, 1.1.3, rutherfo@cs.colorado.edu)
AC_CONFIG_HEADERS([config.h])
AC_PREFIX_DEFAULT(/opt)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE

#
# libtool versioning.  Follow this rule:
# 
# if (interface changed) {
#    LT_CURRENT := LT_CURRENT + 1
#    LT_REVISION := 0
#    if (backward compatible) {
#       LT_AGE := LT_AGE + 1
#    } else {
#       LT_AGE := 0
#    }
# } else {  /* no interface changes */
#    LT_REVISION := LT_REVISION + 1
# }
#
LT_RELEASE=1
LT_CURRENT=1
LT_REVISION=3
LT_AGE=0
#
# Caveat: don't try to maintain the package version in sync with the
# ======  library version at all costs.  This is a common mistake.
#
# _LDFLAGS =  -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
#             -release $(LT_RELEASE)
#
# LT_CURRENT is the "major" library version.  
#	Change LT_CURRENT whenever you brake interface compatibility
#
# LT_REVISION is the revision number (i.e., "minor" number)
#	Change LT_CURRENT whenever you change the implementation, but
#	reset LT_VERSION=0 whenever you change LT_CURRENT
#
#
# LT_AGE indicates a compatibility range, referring to LT_CURRENT
#	e.g., 5:34:2 means also compatible with 3:*:* and 4:*:*
#
# LT_RELEASE is the label associated with the file name.  It makes sense 
#	to assign the "major" package version number to LT_RELEASE: 
#
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
#
# configuration options
#
AC_ARG_ENABLE(profiling, 
  AC_HELP_STRING([--enable-profiling],
	[include profiling information (default is NO)]),
dnl this is to optionally compile with profiling
dnl I don't know too much about this, but it looks like
dnl -pg only works with static libraries, so I'm going to 
dnl disable shared libraries here.
  [ case "$enableval" in
        coverage )
	    CFLAGS_prof='-pg -fprofile-arcs -ftest-coverage'
	    CXXFLAGS_prof='-pg -fprofile-arcs -ftest-coverage'
	    LDFLAGS_prof='-pg'
	    AC_MSG_RESULT([Enabling profiling and coverage information])
	    ;;
        * )
	    CFLAGS_prof='-pg'
	    CXXFLAGS_prof='-pg'
	    LDFLAGS_prof='-pg'
	    AC_MSG_RESULT([Enabling profiling information])
	    ;;
    esac
    AC_DISABLE_SHARED ], 
  [ CFLAGS_prof=''
    CXXFLAGS_prof=''
    LDFLAGS_prof=''
    AC_ENABLE_SHARED ])
AC_SUBST(CFLAGS_prof)
AC_SUBST(CXXFLAGS_prof)
AC_SUBST(LDFLAGS_prof)
#
# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_INSTALL
#
# The libtool macro AC_PROG_LIBTOOL checks for the C preprocessor.
# Configure gets confused if we try to check for a C preprocessor
# without first checking for the C compiler
# (see http://sources.redhat.com/ml/autoconf/2001-07/msg00036.html),
# so we invoke AC_PROG_CC explicitly.
AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)
#
AC_PROG_LIBTOOL
#
AC_MSG_CHECKING(custom location of sff)
AC_ARG_WITH(sff,
	[ --with-sff[=PATH] Path to sff installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	],
	[ AC_MSG_RESULT(no)
	])

AC_CHECK_LIB(sff,main,
	[ LIBS="-lsff $LIBS"
	],
	[ AC_MSG_ERROR( ["sff was not found. use --with-sff if it is located in a non-standard directory."] )
	])

AC_MSG_CHECKING(custom location of ssimp)
AC_ARG_WITH(ssimp,
	[ --with-ssimp[=PATH] Path to ssimp installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	],
	[ AC_MSG_RESULT(no)
	])

AC_CHECK_LIB(ssimp,main,
	[ LIBS="-lssimp $LIBS"
	],
	[ AC_MSG_ERROR( ["ssimp was not found. use --with-ssimp if it is located in a non-standard directory."] )
	])

AC_MSG_CHECKING(custom location of ssim)
AC_ARG_WITH(ssim,
	[ --with-ssim[=PATH] Path to ssim installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	],
	[ AC_MSG_RESULT(no)
	])

AC_CHECK_LIB(ssim,main,
	[ LIBS="-lssim $LIBS"
	],
	[ AC_MSG_ERROR( ["ssim was not found. use --with-ssim if it is located in a non-standard directory."] )
	])

AC_MSG_CHECKING(custom location of ssbg)
AC_ARG_WITH(ssbg,
	[ --with-ssbg[=PATH] Path to ssbg installation directory.],
	[ AC_MSG_RESULT($withval)
	  LIBS="-L$withval/lib $LIBS"
	  CXXFLAGS="-I$withval/include $CXXFLAGS"
	],
	[ AC_MSG_RESULT(no)
	])

AC_CHECK_LIB(ssbg,main,
	[ LIBS="-lssbg $LIBS"
	],
	[ AC_MSG_ERROR( ["ssbg was not found. use --with-ssbg if it is located in a non-standard directory."] )
	])

dnl AC_MSG_CHECKING(custom location of collisionfield)
dnl AC_ARG_WITH(collisionfield,
dnl 	[ --with-collisionfield=PATH  Direct path to collisionfield library/includes.],
dnl 	[ LIBS="-L$withval $LIBS"
dnl 	  CXXFLAGS="-I$withval $CXXFLAGS"
dnl 	],
dnl 	[ AC_MSG_RESULT(no)
dnl 	])

dnl AC_CHECK_LIB(collisionfield, _init,
dnl 	  [ LIBS="-lcollisionfield $LIBS"
dnl 	    AC_MSG_RESULT(yes)
dnl 	    AC_DEFINE(COLLISION_DETECTION_ON, 1, [Enables radio collision detection])
dnl 	  ],
dnl 	  [ AC_MSG_RESULT( no )
dnl 	   ])

AC_MSG_CHECKING(inclusion of GMP)
AC_SUBST(power_type)
AC_SUBST(power_include)
power_type="long long"
power_include="stdint.h"
ac_gmp=no
AC_ARG_ENABLE( gmp,
  AC_HELP_STRING([--enable-gmp], [enable use of GnuMP for power management (default is NO)]),
  [ ac_gmp=${enableval}
  ])
AC_MSG_RESULT($ac_gmp)
if test "$ac_gmp" = yes; then
  AC_CHECK_LIB(gmp, __gmpz_add, [
    AC_SUBST(LDFLAGS_gmp, "-lgmp -lgmpxx")
    power_include="gmpxx.h"
    power_type="mpz_class"
dnl     AC_CHECK_LIB(gmpxx, __gmpz_add, [
dnl     ], [
dnl       AC_ERROR([To enable GnuMP, libgmp and libgmpxx must be present.])
dnl     ])
  ], [
    AC_ERROR([To enable GnuMP, libgmp and libgmpxx must be present.])
  ])
fi


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

AC_DEFINE_UNQUOTED(PREFIX, "${prefix}", [path prefix])

#
# programs
#
AM_PROG_LEX
AC_PROG_YACC

AC_PATH_PROG(SH, sh, /bin/sh)
AC_PATH_PROG(FALSE, false, /usr/bin/false)
AC_PATH_PROG(TRUE, true, /usr/bin/true)
AC_PATH_PROG(RM, rm, $FALSE)
AC_PATH_PROG(MV, mv, $FALSE)
AC_PATH_PROG(CP, cp, $FALSE)
AC_PATH_PROG(TAR, tar, $FALSE)
AC_PATH_PROG(GZIP, gzip, $FALSE)
AC_PATH_PROG(ECHO, echo, $FALSE)
#
# I should probably create a better test for rpmbuild
#
AC_PATH_PROG(RPMBUILD, rpmbuild rpm, $FALSE)
#
# library, header and othere system-specific stuff
#
AC_HEADER_TIME
#
# check for doxygen
#
AC_PATH_PROG(DOXYGEN, doxygen, false, $PATH)
AM_CONDITIONAL(HAVE_DOXYGEN, test "x$DOXYGEN" != xfalse)                     
#
#
AC_OUTPUT([Makefile
	   cbnsim-version
           cbnsim/types.h
	   doc/Makefile
	   doc/doxygen.conf 
	   doc/header.html
	   test/Makefile])
