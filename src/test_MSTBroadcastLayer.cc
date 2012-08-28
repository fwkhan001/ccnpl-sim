// -*- C++ -*-
//
//  This file is part of CBCBSIM, the Combined Broadcast Content-Based
//  (CBCB) routing simulation.
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
// $Id: test_MSTBroadcastLayer.cc,v 1.2 2004/04/30 19:42:41 rutherfo Exp $
//
#include <cassert>
#include <iostream>
#include <sstream>
#include "cbcbsim/MSTBroadcastLayer.h"

using namespace std;
using namespace ssim;
using namespace cbcbsim;

int main( int ac, char** av )
{
  try 
    {
      MSTBroadcastLayer mbl;
      
      mbl.addNode( 11 );
      mbl.addNode( 28 );
      mbl.addNode( 31 );
      mbl.addNode( 43 );

      mbl.addLink( 11, 28, 1 );
      mbl.addLink( 11, 31, 3 );
      mbl.addLink( 11, 43, 4 );
      mbl.addLink( 28, 43, 5 );
      mbl.addLink( 31, 43, 1 );
      mbl.addLink( 31, 28, 6 );

      mbl.computePaths();
      assert( mbl.getHopCount( 11, 11 ) == 0 );
      assert( mbl.getHopCount( 11, 28 ) == 1 );
      assert( mbl.getHopCount( 11, 31 ) == 1 );
      assert( mbl.getHopCount( 11, 43 ) == 2 );

      assert( mbl.getHopCount( 28, 11 ) == 1 );
      assert( mbl.getHopCount( 28, 28 ) == 0 );
      assert( mbl.getHopCount( 28, 31 ) == 2 );
      assert( mbl.getHopCount( 28, 43 ) == 3 );

      assert( mbl.getHopCount( 31, 11 ) == 1 );
      assert( mbl.getHopCount( 31, 28 ) == 2 );
      assert( mbl.getHopCount( 31, 31 ) == 0 );
      assert( mbl.getHopCount( 31, 43 ) == 1 );

      assert( mbl.getHopCount( 43, 11 ) == 2 );
      assert( mbl.getHopCount( 43, 28 ) == 3 );
      assert( mbl.getHopCount( 43, 31 ) == 1 );
      assert( mbl.getHopCount( 43, 43 ) == 0 );
      /*
       * 11 as root
       */
      {
	set<ProcessId> c11; c11.insert( 28 ); c11.insert( 31 );
	assert( mbl.getChildren( 11, 11 ) == c11 );
	set<ProcessId> c28;
	assert( mbl.getChildren( 11, 28 ) == c28 );
	set<ProcessId> c31; c31.insert( 43 );
	assert( mbl.getChildren( 11, 31 ) == c31 );
	set<ProcessId> c43;
	assert( mbl.getChildren( 11, 43 ) == c43 );

	assert( mbl.getParent( 11, 11 ) == 11 );
	assert( mbl.getParent( 11, 28 ) == 11 );
	assert( mbl.getParent( 11, 31 ) == 11 );
	assert( mbl.getParent( 11, 43 ) == 31 );
      }
      /*
       * 28 as root
       */
      {
	set<ProcessId> c11; c11.insert( 31 );
	assert( mbl.getChildren( 28, 11 ) == c11 );
	set<ProcessId> c28; c28.insert( 11 );
	assert( mbl.getChildren( 28, 28 ) == c28 );
	set<ProcessId> c31; c31.insert( 43 );
	assert( mbl.getChildren( 28, 31 ) == c31 );
	set<ProcessId> c43;
	assert( mbl.getChildren( 28, 43 ) == c43 );

	assert( mbl.getParent( 28, 11 ) == 28 );
	assert( mbl.getParent( 28, 28 ) == 28 );
	assert( mbl.getParent( 28, 31 ) == 11 );
	assert( mbl.getParent( 28, 43 ) == 31 );
      }
      /*
       * 31 as root
       */
      {
	set<ProcessId> c11; c11.insert( 28 );
	assert( mbl.getChildren( 31, 11 ) == c11 );
	set<ProcessId> c28;
	assert( mbl.getChildren( 31, 28 ) == c28 );
	set<ProcessId> c31; c31.insert( 11 ); c31.insert( 43 );
	assert( mbl.getChildren( 31, 31 ) == c31 );
	set<ProcessId> c43;
	assert( mbl.getChildren( 31, 43 ) == c43 );

	assert( mbl.getParent( 31, 11 ) == 31 );
	assert( mbl.getParent( 31, 28 ) == 11 );
	assert( mbl.getParent( 31, 31 ) == 31 );
	assert( mbl.getParent( 31, 43 ) == 31 );
      }
      /*
       * 43 as root
       */
      {
	set<ProcessId> c11; c11.insert( 28 );
	assert( mbl.getChildren( 43, 11 ) == c11 );
	set<ProcessId> c28;
	assert( mbl.getChildren( 43, 28 ) == c28 );
	set<ProcessId> c31; c31.insert( 11 );
	assert( mbl.getChildren( 43, 31 ) == c31 );
	set<ProcessId> c43; c43.insert( 31 );
	assert( mbl.getChildren( 43, 43 ) == c43 );

	assert( mbl.getParent( 43, 11 ) == 31 );
	assert( mbl.getParent( 43, 28 ) == 11 );
	assert( mbl.getParent( 43, 31 ) == 43 );
	assert( mbl.getParent( 43, 43 ) == 43 );
      }
    }
  catch( exception e )
    {
      cerr << "EXCEPTION: " << e.what() << endl;
      return 1; 
    }
  catch( ... )
    {
      cerr << "UNKNOWN EXCEPTION" << endl;
    }
}
