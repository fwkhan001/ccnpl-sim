// -*- C++ -*-
//
//  This file is part of CBNSIM, the Content-Based Networking
//  Simulation Library.
//
//  Authors: See the file AUTHORS for full details.
//
//  Copyright (C) 2002-2004 University of Colorado
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
//  USA, or send email to serl@cs.colorado.edu.
//
// $Id: logging.h,v 1.3 2004/01/20 05:39:27 rutherfo Exp $
//
#include <cassert>
#include <iostream>

#ifndef CBNSIM_LOGGING_H
#define CBNSIM_LOGGING_H
/** \file cbnsim/logging.h This file contains preprocessor macros and
 * function prototypes for printing generic debugging and error messages
 * and also for printing predicates and messages in the format that
 * the sff program can read in.
 **/

#include <siena/forwarding.h>
#include <siena/ssimp_types.h>

/** @brief Namespace containing classes and procedures provided by the <code>cbnsim</code>
 * library.
 */
namespace cbnsim
{
  extern bool g_debug_on;
  extern bool g_info_on;
  extern bool g_error_on;

  /** @brief Function to print a predicate in the format expected by
   * the <code>sff</code> program.
   */
  void sff_print( const ssimp::SimplifyingPredicate& pred, siena::if_t id = 0 );

  /** @brief Function to print a message in the format expected by the
   * <code>sff</code> program.
   */
  void sff_print( const ssimp::Message& msg );
};

#ifdef CBNDEBUG_ON
#define CBNDEBUG(u) if( cbnsim::g_debug_on ) { std::cout << "DEBUG: " << u << endl; }
#else
#define CBNDEBUG(u)
#endif // CBNDEBUG_ON

#define CBNINFO(u) if( cbnsim::g_info_on ) { std::cout << "INFO: " << u << endl; }
#define CBNERROR(u) if( cbnsim::g_error_on ) { std::cerr << "ERROR: " << u << endl; }

#ifndef CBNASSERT_OFF
#define CBNASSERT(exp,msg) if(!(exp)){CBNERROR(msg);assert(exp);}
#else
#define CBNASSERT(exp,msg)
#endif // CBNASSERT_OFF

#endif // CBNSIM_LOGGING_H
