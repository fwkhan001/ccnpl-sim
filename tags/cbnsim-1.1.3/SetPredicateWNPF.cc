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
// $Id: SetPredicateWNPF.cc,v 1.6 2004/12/13 17:50:56 carzanig Exp $
//
#include <string>
#include <cmath>
#include "cbnsim/SetPredicateWNP.h"
#include "cbnsim/SetPredicateWNPF.h"

using namespace cbnsim;
using namespace std;

SetPredicateWNPF :: SetPredicateWNPF()
    : WorkloadNodeProcessFactory( "set_predicate" ), 
      m_delay( 30 ), m_t0_pred( false ), m_msg_rate( 0 ), m_poisson( false )
{
}

bool SetPredicateWNPF :: applies_to( NodeType nt )
{
    // no need to allocate a process for a node if it won't
    // do anything
    return (m_delay || m_t0_pred) && nt == Subscriber_Node;
}

bool SetPredicateWNPF :: handle_argument( vector<string>::const_iterator& i,
					  vector<string>::const_iterator end )
    throw ( argument_exception )
{
    bool ret = false;
    
    if( (*i) == string( "-t0-pred" ) )
    {
	m_t0_pred = true;
	ret = true;
    }
    else if( (*i) == "-st" )
    {
	if( ++i == end )
	{
	    throw argument_exception( "-st argument expects a value" );
	}
	m_delay = atof( (*i).c_str() );

	if( ++i == end )
	{
	    throw argument_exception( "-st needs to chose a mode between f<fix> and p<poisson>" );
	}
	if( (*i) == "f" )
	{
	    m_poisson = false;
	}
	if( (*i) == "p" )
	{
	    m_poisson = true;
	}
	ret = true;
    }
    else if( (*i) == "-mrt" )
    {
	if( ++i == end )
	{
	    throw argument_exception( "-mrt argument expects a value" );
	}
	m_msg_rate = atoi( (*i).c_str() );
	ret = true;
    }
    
    return ret;
}

WorkloadNodeProcess* SetPredicateWNPF::newWorkloadNodeProcess(unsigned int nid,
							      ostream& out, 
							      ssbg::SSBG& ssbg,
							      double wtu )
    throw ()
{
    return new SetPredicateWNP(nid, out, ssbg,
			       (unsigned int)floor( m_delay /wtu ), m_t0_pred,
			       (unsigned int)m_msg_rate, m_poisson );
}

void SetPredicateWNPF :: print_optional_usage( std::ostream& out )
{
    cerr << "\t -t0-pred		create a set_predicate events at t=0 (default false)" << endl;
    cerr << "\t -st <sec> 		mean time to subscribe (in seconds) (defaults to 30)" << endl;
    cerr << "\t           f		fixed time interval subscribe event (default)" << endl;
    cerr << "\t           p		poisson time interval with average st sbuscribe event" << endl;
    
}

void SetPredicateWNPF :: print_required_usage( std::ostream& out )
{
}

void SetPredicateWNPF :: print_setting_summary( ostream& out )
{
    out << "# set_predicate: mean time to event = " << m_delay << " (sec)" << endl;
    if ( m_poisson == false )
    {
      out << "# set_predicate: fixed time interval" << endl;
    }
    if ( m_poisson == true )
    {
      out << "# set_predicate: poisson distribution event" << endl;
    }
    out << "# set_predicate: mean rate for pubs = " << m_msg_rate << " (msec)" 
	<< "\n# set_predicate: t=0 predicate = " << m_t0_pred << endl;
}

bool SetPredicateWNPF :: satisfied()  const
{
    return true; // no required settings.
}
