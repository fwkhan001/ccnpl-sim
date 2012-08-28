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
// $Id: SetPredicateParser.cc,v 1.2 2004/02/03 06:16:21 hallcp Exp $
//
#include "cbnsim/SetPredicateEvent.h"
#include "cbnsim/SetPredicateParser.h"
#include "cbnsim/WorkloadParser.h"
#include "cbnsim/types.h"

using namespace cbnsim;
using namespace siena;
using namespace ssimp;
using namespace std;

WorkloadEvent* SetPredicateParser :: parse_event( WorkloadParser& parser )
  throw ( parse_exception )
{
  parser.eat( WorkloadParser::ID ); // set_predicate
  unsigned int nid = parser.eat_integer();
  double ts = parser.eat_double();
  unsigned int rate = parser.eat_integer();
  Predicate p;
  while( parser.tok() != WorkloadParser::SEMICOLON )
    {
      if( parser.tok() == WorkloadParser::PIPE )
	{
	  parser.eat( WorkloadParser::PIPE );
	}
      Filter f;
      constraint_list( parser, f );
      p.add( f );
    }
  parser.eat( WorkloadParser::SEMICOLON );
  return new SetPredicateEvent( nid, ts, p, rate );
}

void SetPredicateParser :: constraint_list( WorkloadParser& parser, Filter& filt )
  throw ( parse_exception )
{
  string name = parser.eat_id();
  siena::operator_id op = parser.eat_op();
  switch( parser.tok() )
    {
    case WorkloadParser::BOOL:
      filt.add( Constraint( name, op, parser.eat_bool() ) );
      break;
    case WorkloadParser::DOUBLE:
      filt.add( Constraint( name, op, parser.eat_double() ) );
      break;
    case WorkloadParser::INTEGER:
      filt.add( Constraint( name, op, (int_t)parser.eat_integer() ) );
      break;
    case WorkloadParser::STRING:
      filt.add( Constraint( name, op, parser.eat_string() ) );
      break;
    default:
      throw parse_exception( parser.val() );
    }
  // now decide to recurse or not
  if( parser.tok() == WorkloadParser::AMP )
    {
      parser.eat( WorkloadParser::AMP );
      constraint_list( parser, filt );
    }
}

