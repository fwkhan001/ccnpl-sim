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
// $Id: NodeFailParser.cc,v 1.1 2004/01/07 16:33:24 rutherfo Exp $
//
#include "cbnsim/NodeFailEvent.h"
#include "cbnsim/NodeFailParser.h"
#include "cbnsim/WorkloadParser.h"

using namespace cbnsim;

WorkloadEvent* NodeFailParser :: parse_event( WorkloadParser& parser )
  throw ( parse_exception )
{
  parser.eat( WorkloadParser::ID ); // node_fail
  unsigned int nid = parser.eat_integer();
  unsigned int ts = parser.eat_integer();
  parser.eat( WorkloadParser::SEMICOLON );
  return new NodeFailEvent( nid, ts );
}
