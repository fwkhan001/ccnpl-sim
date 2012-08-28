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
// $Id: io_exception.h,v 1.3 2004/01/08 16:02:17 rutherfo Exp $
//
#ifndef CBNSIM_IO_EXCEPTION_H
#define CBNSIM_IO_EXCEPTION_H

#include <exception>
#include <string>

namespace cbnsim
{
  /** @brief Generic IO exception.
   */
  class io_exception : public std::exception
  {
    std::string m_arg;

  public:
    io_exception( const std::string& arg ) throw ()
      : m_arg( arg ) {}

    virtual ~io_exception() throw() {}

    virtual const char* what() const throw ()
    { return m_arg.c_str(); }
  };
};
#endif
