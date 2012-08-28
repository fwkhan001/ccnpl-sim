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
// $Id: cbnwlgen.cc,v 1.3 2004/03/04 04:25:17 rutherfo Exp $
//
#include "cbnsim/NodeFailWNPF.h"
#include "cbnsim/SendMessageWNPF.h"
#include "cbnsim/SetPredicateWNPF.h"
#include "cbnsim/WorkloadGenerator.h"

using namespace cbnsim;
using namespace std;

int main( int argc, char** argv )
{
  try
    {
      vector<string> args;
      for( int i = 1; i < argc; ++i )
	{
	  args.push_back( argv[i] );
	}
      WorkloadGenerator  wlgen;
      // add the basic node processes
      wlgen.add( new NodeFailWNPF() );
      wlgen.add( new SendMessageWNPF() );
      wlgen.add( new SetPredicateWNPF() );
      return wlgen.main( string( argv[0] ), args );
    }
  catch( exception& e )
    {
      cerr << "ERROR: " << e.what() << endl
	   << "exiting..." << endl;
      return 1;
    }
}
