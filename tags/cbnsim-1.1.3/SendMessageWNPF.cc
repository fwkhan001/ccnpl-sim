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
// $Id: SendMessageWNPF.cc,v 1.5 2004/12/13 17:50:56 carzanig Exp $
//
#include <string>
#include <cmath>
#include "cbnsim/SendMessageWNP.h"
#include "cbnsim/SendMessageWNPF.h"

#include <iostream>

using namespace cbnsim;
using namespace std;

SendMessageWNPF :: SendMessageWNPF()
    : WorkloadNodeProcessFactory( "send_message" ), m_delay( 10 ), m_const(false), m_poisson(false),
      m_change_interval( 0 ), m_pareto(false), m_pareto_alpha( 0 )
{
}

bool SendMessageWNPF :: applies_to( NodeType nt )
{
    // no need to allocate a process if the delay is zero
    return m_delay && nt == Publisher_Node;
}

bool SendMessageWNPF :: handle_argument( vector<string>::const_iterator& i,
					 vector<string>::const_iterator end ) throw ( argument_exception )
{
    if( (*i) == "-pt" )
    {
	if( ++i == end )
	{
	    throw argument_exception( "-pt argument expects a value" );
	}
	m_delay = atof( (*i).c_str() );

        if( ++i == end )
	{
	    throw argument_exception( "-pt needs to chose a mode between f<fix> and p<poisson>" );
	}
	if( (*i) == "f" )
	{
	    m_poisson = false;
	}
	if( (*i) == "p" )
	{
	    m_poisson = true;
	}
	return true;
    }
    else if( (*i) == "-p-const")
    {
	m_const = true;
	return true;
    }
    else if( (*i) == "-pch" )
    {
	if( ++i == end )
	{
	    throw argument_exception( "-pch argument expects a value" );
	}
	m_change_interval = atof( (*i).c_str() );
	if(m_change_interval < m_delay)
	{
	    throw argument_exception( "message change interval should be greater than mean time to publish" );
	}
        if( ++i == end )
	{
	    throw argument_exception( "-pch needs to chose a mode between f<fix> and m<pareto_mean>" );
	}
	if( (*i) == "f" )
	{
	    m_pareto = false;
	}
	if( (*i) == "m" )
	{
	    m_pareto = true;
	    if( ++i == end )
	    {
		throw argument_exception( "pareto_mean mode needs a shape parameter" );
	    }
	    m_pareto_alpha = atof( (*i).c_str() );
	}
	return true;
    }
    return false;
}

WorkloadNodeProcess* SendMessageWNPF :: newWorkloadNodeProcess( unsigned int nid,
								ostream& out, 
								ssbg::SSBG& ssbg,
								double wtu ) throw ()
{
    return new SendMessageWNP( nid, out, ssbg,
			       (unsigned int)floor( m_delay / wtu ), 
                               m_const, m_poisson, 
			       (unsigned int)floor(m_change_interval/ wtu ),
                               m_pareto, m_pareto_alpha );
}

void SendMessageWNPF :: print_optional_usage( std::ostream& out )
{
    cerr << "\t -pt <sec> 		mean time to publish (in seconds) (defaults to 10)" << endl;
    cerr << "\t           f		fixed time interval publish event (default)" << endl;
    cerr << "\t           p		poisson time interval with average pt publish event" << endl;
    cerr << "\t -p-const		hold publications at each node constant (default false)" << endl;
    cerr << "\t -pch <sec> 		set the change of publish content (default is changing all the time)" << endl;
    cerr << "\t 	   f		fixed interval between the change of publish content" << endl;
    cerr << "\t 	   m <alpha>	the interval follows the pareto distribution, <alpha> - shape parameter" << endl;
}

void SendMessageWNPF :: print_required_usage( std::ostream& out )
{
}

void SendMessageWNPF :: print_setting_summary( ostream& out )
{
    out << "# send_message: mean time to event = " << m_delay << " (sec)" << endl;
    if ( m_poisson == false )
    {
      out << "# send_message: fixed time interval" << endl;
    }
    if ( m_poisson == true )
    {
      out << "# send_message: poisson distribution event" << endl;
    }
    out << "# send_message: hold publications constant = " << m_const << endl;
    if ( m_change_interval == 0 )
    {
      out << "# send_message: the content of message is changing all the time" << endl;
    }
    else
    {
      out << "# send_message: average time interval of changing the message content = " << m_change_interval << "(sec)";
      if ( m_pareto == false )
      {
        out << " fixed " << endl;
      }
      else
      {
        out << " pareto distribution, shape paramater = " << m_pareto_alpha << endl;
      }
    }

}

bool SendMessageWNPF :: satisfied()  const
{
    return true; // no required settings
}
