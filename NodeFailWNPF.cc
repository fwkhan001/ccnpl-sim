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
// $Id: NodeFailWNPF.cc,v 1.3 2004/12/13 17:50:56 carzanig Exp $
//
#include <string>
#include <cmath>
#include "cbnsim/NodeFailWNP.h"
#include "cbnsim/NodeFailWNPF.h"

using namespace cbnsim;
using namespace std;

NodeFailWNPF :: NodeFailWNPF()
    : WorkloadNodeProcessFactory( "node_fail" ), m_delay( 0 ),
      m_recover_delay( 0 ), m_final_delay ( 0 )
{
}

bool NodeFailWNPF :: applies_to( NodeType nt )
{
    // if delay is non-zero, any node can fail
    return m_delay != 0;
}

bool NodeFailWNPF :: handle_argument( vector<string>::const_iterator& i,
				      vector<string>::const_iterator end )
    throw ( argument_exception )
{
    bool ret = false;
    
    if( (*i) == "-ft" )
    {
	if( ++i == end )
	{
	    throw argument_exception( "-ft argument expects a value" );
	}
	m_delay = atoi( (*i).c_str() );
	ret = true;
    } else if( (*i) == "-rt" ) {
	if( ++i == end )
	{
	    throw argument_exception( "-rt argument expects a value" );
	}
	m_recover_delay = atoi( (*i).c_str() );
	ret = true;
    } else if( (*i) == "-fdt" ) {
	if( ++i == end )
	{
	    throw argument_exception( "-fdtt argument expects a value" );
	}
	m_final_delay = atoi( (*i).c_str() );
	ret = true;
    }
    
    return ret;
}

WorkloadNodeProcess* NodeFailWNPF :: newWorkloadNodeProcess( unsigned int nid,
							     ostream& out, 
							     ssbg::SSBG& ssbg,
							     double wtu ) throw ()
{
    return new NodeFailWNP( nid, out, (unsigned int)floor( m_delay / wtu ),
			    (unsigned int)floor( m_final_delay / wtu ),
			    (unsigned int)floor( m_recover_delay / wtu ));
}

void NodeFailWNPF :: print_optional_usage( std::ostream& out )
{
    cerr << "\t -ft			mean time to node failure (default is 0 (off))\n"
	 << "\t -rt			mean time to node recovery (default is 0 (off))\n"
	 << "\t -fdt			mean time to final node death (default is 0 (off))"
	 << endl;
}

void NodeFailWNPF :: print_required_usage( std::ostream& out )
{
}

void NodeFailWNPF :: print_setting_summary( ostream& out )
{
    out << "# node_fail: mean time to event = " << m_delay << " (sec)" << endl;
}

bool NodeFailWNPF :: satisfied()  const
{
    return true; // no required settings.
}
