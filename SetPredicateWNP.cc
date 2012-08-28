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
// $Id: SetPredicateWNP.cc,v 1.5 2004/04/19 05:26:16 rutherfo Exp $
//
#include <exception>
#include <siena/ssbg.h>
#include <siena/ssim.h>
#include <stdexcept>
#include "cbnsim/SetPredicateWNP.h"
#include "cbnsim/logging.h"

using namespace cbnsim;
using namespace std;

SetPredicateWNP :: SetPredicateWNP( unsigned int nid,
				    ostream& out,
				    ssbg::SSBG& ssbg,
				    unsigned int delay,
				    bool t0_pred,
				    unsigned int msg_rate,
				    bool ispoisson )
    : WorkloadNodeProcess( nid, out ),
      m_delay( delay ), m_ssbg( ssbg ), m_t0_pred( t0_pred ),
      m_msg_rate(msg_rate), m_poisson ( ispoisson )
{
}

void SetPredicateWNP :: init()
{
    if( m_t0_pred )
    {
	ssbg::predicate pred;
	m_ssbg.new_predicate( pred );
	print( pred );
    }
    if( m_delay )
    {
	if ( !m_poisson )
	{
          // fixed time interval to set predicate 
          ssim::Sim::self_signal_event( NULL, m_delay );
	}
	if ( m_poisson )
	{
	  ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
	}
    }
}

void SetPredicateWNP :: print( const ssbg::predicate& pred )
{
    ssbg::predicate::const_iterator pi;
    
    unsigned int rate = ssbg::rand_poisson_delta( m_msg_rate );
	
    m_out << "set_predicate " << node_id() << " " << ssim::Sim::clock() << " "
	  << (m_msg_rate ? rate : 0) << " ";
    for( pi = pred.begin(); pi != pred.end(); ++pi )
    {
	if( (*pi).empty() )
	{
	    continue;
	}
	if( pi != pred.begin() )
	{
	    m_out << " | ";
	}
	ssbg::filter::const_iterator j;
	for( j = (*pi).begin(); j != (*pi).end(); ++j )
	{
	    if( j != (*pi).begin() )
	    {
		m_out << " & ";
	    }
	    switch( (*j).type )
	    {
	    case ssbg::BOOL:
		output_attr( m_out, (*j).name, (*j).op, (*j).bool_val );
		break;
	    case ssbg::DOUBLE:
		output_attr( m_out, (*j).name, (*j).op, (*j).double_val );
		break;
	    case ssbg::INT:
		output_attr( m_out, (*j).name, (*j).op, 1 );
		break;
	    case ssbg::STRING:
		output_attr( m_out, (*j).name, (*j).op, (*j).string_val );
		break;
	    default:
		throw logic_error( "unknown constraint type" );
	    }
            /*if( node_id() == 24 || node_id() == 26 || node_id() == 28 || node_id() == 30 ||
                node_id() == 32 || node_id() == 34 || node_id() == 36 || node_id() == 38 ||
                node_id() == 41 || node_id() == 43 || node_id() == 45 || node_id() == 47  )*/     //temp
           /* if( node_id() % 2 == 0 )
            {
	      switch( (*j).type )
	    {
	    case ssbg::BOOL:
		output_attr( m_out, (*j).name, (*j).op, (*j).bool_val );
		break;
	    case ssbg::DOUBLE:
		output_attr( m_out, (*j).name, (*j).op, (*j).double_val );
		break;
	    case ssbg::INT:
		output_attr( m_out, (*j).name, (*j).op, 0 );
		break;
	    case ssbg::STRING:
		output_attr( m_out, (*j).name, (*j).op, (*j).string_val );
		break;
	    default:
		throw logic_error( "unknown constraint type" );
	    }
            }
            if( node_id() == 25 || node_id() == 27 || node_id() == 29 || node_id() == 31 ||
                node_id() == 33 || node_id() == 35 || node_id() == 37 || node_id() == 39 ||
                node_id() == 40 || node_id() == 42 || node_id() == 44 || node_id() == 46 )     //temp
            if( node_id() % 2 != 0 )
            {
	      switch( (*j).type )
	    {
	    case ssbg::BOOL:
		output_attr( m_out, (*j).name, (*j).op, (*j).bool_val );
		break;
	    case ssbg::DOUBLE:
		output_attr( m_out, (*j).name, (*j).op, (*j).double_val );
		break;
	    case ssbg::INT:
		output_attr( m_out, (*j).name, (*j).op, 1 );
		break;
	    case ssbg::STRING:
		output_attr( m_out, (*j).name, (*j).op, (*j).string_val );
		break;
	    default:
		throw logic_error( "unknown constraint type" );
	    }
            }*/
	}
    }
    m_out << " ;" << endl;
}

void SetPredicateWNP :: process_event( const ssim::Event* evt )
{
  CBNASSERT( !evt, "SetPredicateWNP can only handle NULL events" );
  ssbg::predicate pred;
  m_ssbg.new_predicate( pred );
  print( pred );
  if ( !m_poisson )
  {
    // fixed time interval to set predicate  
    ssim::Sim::self_signal_event( NULL, m_delay );
  }
  if ( m_poisson )
  {
    ssim::Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
  }
}
