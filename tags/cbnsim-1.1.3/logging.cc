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
// $Id: logging.cc,v 1.2 2004/01/08 07:01:50 rutherfo Exp $
//
#include <iostream>
#include <siena/forwarding.h>
#include <siena/ssimp_types.h>
#include "cbnsim/logging.h"

using namespace std;
using namespace ssimp;

void cbnsim::sff_print( const SimplifyingPredicate& pred, siena::if_t id )
{
  if( ! pred.size() )
    {
      return;
    }
  cout.setf( ios_base::fixed, ios_base::floatfield );
  cout << "ifconfig " << id << " ";
  const set<SimplifyingFilter>& filters = pred.getFilters();
  set<SimplifyingFilter>::const_iterator i;
  for( i = filters.begin(); i != filters.end(); ++i )
    {
      if( i != filters.begin() )
	{
	  cout << endl << "\t|\t";
	}
      const set<Constraint>& constraints = (*i).getConstraints();
      set<Constraint>::const_iterator j;
      for( j = constraints.begin(); j != constraints.end(); ++j )
	{
	  if( j != constraints.begin() )
	    {
	      cout << ", ";
	    }
	  cout << string( (*j).name().begin, (*j).name().length() ) << " ";
	  switch( (*j).op() )
	    {
	    case siena::eq_id:
	      cout << "=";
	      break;
	    case siena::lt_id:
	      cout << "<";
	      break;
	    case siena::gt_id:
	      cout << ">";
	      break;
	    case siena::sf_id:
	      cout << "*=";
	      break;
	    case siena::pf_id:
	      cout << "=*";
	      break;
	    case siena::ss_id:
	      cout << "**";
	      break;
	    case siena::ne_id:
	      cout << "!=";
	      break;
	    default:
	      cout << "??";
	      break;
	    }
	  cout << " ";
	  switch( (*j).type() )
	    {
	    case siena::bool_id:
	      if( (*j).bool_value() )
		{
		  cout << "true";
		}
	      else
		{
		  cout << "false";
		}
	      break;
	    case siena::double_id:
	      cout << (*j).double_value();
	      break;
	    case siena::int_id:
	      cout << (*j).int_value();
	      break;
	    case siena::string_id:
	      cout << '"' << string( (*j).string_value().begin, (*j).string_value().length() ) << '"';
	      break;
	    default:
	      cout << "???";
	      break;
	    }
	}
    }
  cout << endl;
}

void cbnsim::sff_print( const Message& msg )
{
  cout.setf( ios_base::fixed, ios_base::floatfield );
  cout << "select ";
  const map<string,Message::Value>& attrs = msg.getAttributes();
  map<string,Message::Value>::const_iterator k;
  for( k = attrs.begin(); k != attrs.end(); ++k )
    {
      if( k != attrs.begin() )
	{
	  cout << " ";
	}
      cout << (*k).first;
      cout << " = ";
      switch( (*k).second.type )
	{
	case siena::bool_id:
	  if( (*k).second.b_value )
	    {
	      cout << "true";
	    }
	  else
	    {
	      cout << "false";
	    }
	  break;
	case siena::double_id:
	  cout <<  (*k).second.d_value;
	  break;
	case siena::int_id:
	  cout << (*k).second.i_value;
	  break;
	default:
	  cout << '"' << (*k).second.s_value << '"';
	}
    }
  cout << endl;
}
