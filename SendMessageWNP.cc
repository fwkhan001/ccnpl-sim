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
// $Id: SendMessageWNP.cc,v 1.3 2004/04/19 05:26:16 rutherfo Exp $
//
#include <exception>
#include <siena/ssbg.h>
#include <siena/ssim.h>
#include <stdexcept>
#include "cbnsim/SendMessageWNP.h"
#include "cbnsim/logging.h"
#include <stdlib.h>

using namespace cbnsim;
using namespace std;

SendMessageWNP :: SendMessageWNP( unsigned int nid,
				  ostream& out,
				  ssbg::SSBG& ssbg,
				  unsigned int delay,
				  bool isconst, bool ispoisson,
				  double change_interval, 
				  bool ispareto, double pareto_alpha)
    : WorkloadNodeProcess( nid, out ),
      m_delay( delay ), m_ssbg( ssbg ), m_const (isconst), m_poisson (ispoisson),
      m_change_interval( change_interval ), m_pareto( ispareto ), m_pareto_alpha( pareto_alpha )
{
    if( m_const )
    {
	m_ssbg.new_notification( m_notif );
    }
    if( m_change_interval != 0 )
    {
	m_time_last_change = ssim::Sim::clock();
    }
	
}

void SendMessageWNP :: init()
{
    if( m_delay )
    {
	if ( !m_poisson )
	{
          // fixed time interval to send msg   --liufeng 28/09/2009
          ssim::Sim::self_signal_event( NULL, m_delay );
	}
	if ( m_poisson )
	{
	  ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
	}
    }
}

void SendMessageWNP :: print( const ssbg::notification& notif )
{
    ssbg::notification::const_iterator ni;
    m_out << "send_message " << node_id() << " " << fixed << ssim::Sim::clock();
    for( ni = notif.begin(); ni != notif.end(); ++ni )
    {
	m_out << " ";
	switch( (*ni).type )
	{
	case ssbg::BOOL:
	    output_attr( m_out, (*ni).name, "=", (*ni).bool_val );
	    break;
	case ssbg::DOUBLE:
	    output_attr( m_out, (*ni).name, "=", (*ni).double_val );
	    break;
	case ssbg::INT:
	    //output_attr( m_out, (*ni).name, "=", (*ni).int_val );
	    output_attr( m_out, (*ni).name, "=", 1 );
	    break;
	case ssbg::STRING:
	    output_attr( m_out, (*ni).name, "=", (*ni).string_val );
	    break;
	default:
	    throw logic_error( "unknown attribute type" );
	}
        /*if( node_id() % 2 == 0 )     
        {
	  switch( (*ni).type )
	{
	case ssbg::BOOL:
	    output_attr( m_out, (*ni).name, "=", (*ni).bool_val );
	    break;
	case ssbg::DOUBLE:
	    output_attr( m_out, (*ni).name, "=", (*ni).double_val );
	    break;
	case ssbg::INT:
	    output_attr( m_out, (*ni).name, "=", 0 );
	    break;
	case ssbg::STRING:
	    output_attr( m_out, (*ni).name, "=", (*ni).string_val );
	    break;
	default:
	    throw logic_error( "unknown attribute type" );
	}
        }
        if( node_id() % 2 != 0 )    
        {
	  switch( (*ni).type )
	{
	case ssbg::BOOL:
	    output_attr( m_out, (*ni).name, "=", (*ni).bool_val );
	    break;
	case ssbg::DOUBLE:
	    output_attr( m_out, (*ni).name, "=", (*ni).double_val );
	    break;
	case ssbg::INT:
	    output_attr( m_out, (*ni).name, "=", 1 );
	    break;
	case ssbg::STRING:
	    output_attr( m_out, (*ni).name, "=", (*ni).string_val );
	    break;
	default:
	    throw logic_error( "unknown attribute type" );
	}
        }*/
    }
    m_out << " ;" << endl;
}

void SendMessageWNP :: process_event( const ssim::Event* evt )
{
  CBNASSERT( !evt, "SendMessageWNP can only handle timeout events" );
  if(!m_const)
    {
      if( m_change_interval == 0 )
      {
        m_ssbg.new_notification( m_notif );
      }

      // option of hold the content of msgs during m_change_interval 
      // optioin 1. the interval follows the pareto distribution
      else if( m_pareto )
      {
        long temp_seed = rand();
        m_change_pareto_interval = m_random_num.pareto_m( m_pareto_alpha, m_change_interval, &temp_seed );
        if( ( ssim::Sim::clock() - m_change_pareto_interval ) >= m_time_last_change || m_time_last_change == 0 )
        {
          m_time_last_change = ssim::Sim::clock();
          m_ssbg.new_notification( m_notif );
        }
      }
      // option 2. the interval is fixed
      else if( ( ssim::Sim::clock() - m_change_interval ) >= m_time_last_change || m_time_last_change == 0 )
      {
        m_time_last_change = ssim::Sim::clock();
        m_ssbg.new_notification( m_notif );
      }
    }
  
  print( m_notif );
  	if ( !m_poisson )
	{
          // fixed time interval to send msg  
          ssim::Sim::self_signal_event( NULL, m_delay );
	}
	if ( m_poisson )
	{
	  ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
	}
}
