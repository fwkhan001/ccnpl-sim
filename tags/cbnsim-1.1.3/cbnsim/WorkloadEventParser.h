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
// $Id: WorkloadEventParser.h,v 1.1 2004/01/07 16:33:23 rutherfo Exp $
//
#ifndef CBNSIM_WORKLOADEVENTPARSER_H
#define CBNSIM_WORKLOADEVENTPARSER_H

#include <string>
#include "cbnsim/WorkloadEvent.h"
#include "cbnsim/parse_exception.h"

namespace cbnsim
{
  class WorkloadParser;

  /** @brief Abstract class representing a generic workload event
   * parser.
   */
  class WorkloadEventParser
    {
    private:
      std::string	m_event_name;

    protected:
      /** @brief Derived classes must provide the name of the event
       * they know how to parse.
       */
      WorkloadEventParser( const std::string& event_name )
	: m_event_name( event_name ) {}

    public:
      virtual ~WorkloadEventParser() {}

      /** @brief Returns the event name.
       */
      const std::string& event_name() const
	{ return m_event_name; }

      /** @brief Uses the methods of <code>WorkloadParser</code> to parse the next
       * event.
       */
      virtual WorkloadEvent* parse_event( WorkloadParser& parser ) throw ( parse_exception ) = 0;
    };
};

#endif
