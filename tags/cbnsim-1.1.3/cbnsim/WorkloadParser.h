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
// $Id: WorkloadParser.h,v 1.2 2004/01/08 07:01:50 rutherfo Exp $
//
#ifndef CBNSIM_WORKLOADPARSER_H
#define CBNSIM_WORKLOADPARSER_H

#include <istream>
#include <map>
#include <set>
#include <siena/types.h>
#include <vector>
#include "cbnsim/WorkloadEvent.h"
#include "cbnsim/WorkloadEventParser.h"
#include "cbnsim/parse_exception.h"

namespace cbnsim
{
  /** @brief Class to parse a workload created by <code>cbnwlgen</code>.
   *
   * This is implemented as a simple recursive-descent parser which makes it
   * amenable to doing an inline parse.
   */
  class WorkloadParser
  {
  public:
    enum TOK { AMP,
	       BOOL,
	       COMMA,
	       DOUBLE,
	       END,
	       ID,
	       INTEGER,
	       LINK,
	       NODE,
	       OP,
	       PIPE,
	       SEMICOLON,
	       SIM_LENGTH,
	       STRING,
	       TIME_UNIT,
	       TOK_COUNT };
  
  private: 
    WorkloadEvent		*m_event_ptr;
    std::map<std::string,WorkloadEventParser*> m_event_parsers;
    std::istream*		m_in;
    double			m_time_unit;
    TOK				m_tok;
    double			m_sim_length;
    std::string			m_val;

    void			link_list() throw ( parse_exception );

    void			node_list() throw ( parse_exception );

    void			sim_length() throw ( parse_exception );

    void			time_unit() throw ( parse_exception );

  public:
    WorkloadParser();
  
    virtual ~WorkloadParser();
  
    void			add( WorkloadEventParser* ep )
    { m_event_parsers.insert( std::make_pair( ep->event_name(), ep ) ); }

    void			advance() throw ( parse_exception );

    void			eat( TOK tok ) throw ( parse_exception );

    bool			eat_bool() throw ( parse_exception );

    std::string			eat_id() throw ( parse_exception );

    int				eat_integer() throw ( parse_exception );

    double			eat_double() throw ( parse_exception );

    siena::operator_id		eat_op() throw ( parse_exception );

    std::string			eat_string() throw ( parse_exception );

    double			get_sim_length() const;

    double			get_time_unit() const;

    bool			isbool() const;

    bool			isdouble() const;
  
    bool			isid() const;

    bool			isinteger() const;

    bool			isop() const;

    bool			isstring() const;

    const WorkloadEvent*	next_event() throw ( parse_exception );

    void			parse_times() throw ( parse_exception );

    void			set_istream( std::istream& in ) { m_in = &in; }

    TOK				tok() { return m_tok; }

    const std::string&		val() { return m_val; }
  };
};
#endif
