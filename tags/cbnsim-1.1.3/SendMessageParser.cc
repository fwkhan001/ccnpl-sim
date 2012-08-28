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
// $Id: SendMessageParser.cc,v 1.1 2004/01/07 16:33:24 rutherfo Exp $
//
#include "cbnsim/SendMessageEvent.h"
#include "cbnsim/SendMessageParser.h"
#include "cbnsim/WorkloadParser.h"
#include "cbnsim/types.h"

using namespace cbnsim;
using namespace siena;
using namespace std;

WorkloadEvent* SendMessageParser :: parse_event( WorkloadParser& parser )
  throw ( parse_exception )
{
  parser.eat( WorkloadParser::ID ); // send_message
  unsigned int nid = parser.eat_integer();
  double ts = parser.eat_double();
  Message msg;
  while( parser.tok() != WorkloadParser::SEMICOLON )
    {
      string id = parser.eat_id();
      parser.eat_op(); // always =
      switch( parser.tok() )
	{
	case WorkloadParser::BOOL:
	  msg.add( id, parser.eat_bool() );
	  break;
	case WorkloadParser::DOUBLE:
	  msg.add( id, parser.eat_double() );
	  break;
	case WorkloadParser::INTEGER:
	  msg.add( id, parser.eat_integer() );
	  break;
	case WorkloadParser::STRING:
	  msg.add( id, parser.eat_string() );
	  break;
	default:
	  throw parse_exception( parser.val() );
	}
    }
  parser.eat( WorkloadParser::SEMICOLON );
  return new SendMessageEvent( nid, ts, msg );
}
