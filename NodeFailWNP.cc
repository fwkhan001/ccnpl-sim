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
// $Id: NodeFailWNP.cc,v 1.3 2004/04/19 05:26:16 rutherfo Exp $
//
#include <exception>
#include <siena/ssbg.h>
#include <siena/ssim.h>
#include <stdexcept>
#include "cbnsim/NodeFailWNP.h"
#include "cbnsim/logging.h"

using namespace cbnsim;
using namespace std;

NodeFailWNP :: NodeFailWNP( unsigned int nid,
			    ostream& out,
			    unsigned int delay,
			    unsigned int final_delay,
			    unsigned int recover_delay)
    : WorkloadNodeProcess( nid, out ), m_delay( delay ),
      m_final_delay( final_delay ), m_recover_delay( recover_delay )
{
    m_failed = false;
}

void NodeFailWNP :: init()
{
    if( m_final_delay )
    {
	m_final_time = ssbg::rand_poisson_delta( m_final_delay );
	ssim::Sim::self_signal_event( NULL, m_final_time );
    }
    
    if( m_delay )
    {
	ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
    }
}

void NodeFailWNP :: print()
{
    if(m_failed)
    {
	m_out << "node_recover " << node_id() << " " << ssim::Sim::clock()
	      << " ;" << endl;
	m_failed = false;
    } else {
	m_out << "node_fail " << node_id() << " " << ssim::Sim::clock()
	      << " ;" << endl;
	m_failed = true;
    }
}

void NodeFailWNP :: process_event( const ssim::Event* evt )
{
  CBNASSERT( !evt, "NodeFailWNP can only handle NULL events" );
  if( m_final_time && ssim::Sim::clock() >  m_final_time )
    {
      m_failed = false;
      print();
    } 
  else
    {
      print();
      
      if( m_failed && m_recover_delay )
	{
	  ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_recover_delay ) );
	}
      else if( !m_failed )
	{
	  ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
	}
    }
}
