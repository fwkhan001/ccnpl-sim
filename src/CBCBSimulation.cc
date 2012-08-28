// -*- C++ -*-
//
//  This file is part of CCN-PLsim, a CCNx simulator
//
//  Authors: See the file AUTHORS for full details. 
//
//  Copyright (C) 2012 France Telecom R&D
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
//  USA, or send email to luca.muscariello@orange.com 
//
#include <cmath> // for rint()
#include <unistd.h> // for getcwd()
#include <cbnsim/SendMessageParser.h>
#include <cbnsim/SetPredicateParser.h>
#include <cbnsim/SetMultipathParser.h>
#include <cbnsim/DownloadContentParser.h>
#include <cbnsim/PublishContentParser.h>
#include <cbnsim/logging.h>
#include <stdexcept>
#include <limits.h>
#include "cbcbsim/CBCBDataCollector.h"
#include "cbcbsim/CBCBNodeFactory.h"
#include "cbcbsim/CBCBSimulation.h"

using namespace cbcbsim;
using namespace cbnsim;
using namespace std;

CBCBSimulation :: CBCBSimulation()
{
  add_event_parser( new SendMessageParser() );
  add_event_parser( new SetPredicateParser() );
  add_event_parser( new SetMultipathParser() );
  add_event_parser( new DownloadContentParser() );
  add_event_parser( new PublishContentParser() );  
  set_factory( new CBCBNodeFactory( m_bl, m_options, &time_block_CB) );
  set_data_collector( new CBCBDataCollector(&time_block_CB, m_options) ); 
  cout << "#Legend" << endl;
  cout << "#time " << "| CLOSED SOCKET " << "node_id " << "| content_name+port_id "
       << "| delivery_time " << "| file_size[bit] "  << "| VRTT[s] " << "| delay[s] " 
       << "| CLASS" << " popularity " << "| opened_sockets " << "| avg_win" << endl;
}

void CBCBSimulation :: setup_topology_hook() throw ()
{
  // finalize the broadcast layer
  m_bl.computePaths();
  // compute the time delays for the protocol options
  m_options.sim_time_unit = (ssim::Time)time_unit();
  m_options.pip_time = (ssim::Time)rint( m_options.pip_sec / time_unit() );
  m_options.prp_time = (ssim::Time)rint( m_options.prp_sec / time_unit() );
  m_options.srsp_time = (ssim::Time)rint( m_options.srsp_sec / time_unit() );
  m_options.to_sender_time = (ssim::Time)rint( m_options.to_sender / time_unit() );
  m_options.to_router_time = (ssim::Time)rint( m_options.to_router / time_unit() );
  m_options.sim_length = sim_length()*time_unit();
  if (m_options.sim_length < m_options.sim_startup)
    cout << "Startup period is longer than the simulation, CACHES' STATISTICS WILL NOT BE COLLECTED!" << endl;
}

void CBCBSimulation :: pre_run_simulation_hook() throw ()
{
  char cwd[PATH_MAX];
  getcwd( cwd, PATH_MAX );
  CBNDEBUG( "*************************************************" );
  CBNDEBUG( "cbcbsim invoked with the following settings:" );
  CBNDEBUG( "cwd = " << cwd );
  CBNDEBUG( "debug = " << debug_messages() );
  CBNDEBUG( "dci = " << dci() << " (sec)" );
  CBNDEBUG( "info = " << info_messages() );
  CBNDEBUG( "outfile = " << output_fname() );

  if( m_options.pip == ProtocolOptions::PIP_IMMEDIATE )
    {
      CBNDEBUG( "pip = immediate" );
    }
  else if( m_options.pip == ProtocolOptions::PIP_TIMED )
    {
      CBNDEBUG( "pip = timed " << m_options.pip_sec << " (sec), "
		<< m_options.pip_time << " (ticks)" );
    }
  else
    {
      throw logic_error( "unrecognized PIP" );
    }
  
  if( m_options.prp == ProtocolOptions::PRP_ALWAYS )
    {
      CBNDEBUG( "prp = always" );
    }
  else if( m_options.prp == ProtocolOptions::PRP_TIMED )
    {
      CBNDEBUG( "prp = timed " << m_options.prp_sec << " (sec), "
		<< m_options.prp_time << " (ticks)" );
    }
  else if( m_options.prp == ProtocolOptions::PRP_MSG_COUNT_CONST )
    {
      CBNDEBUG( "prp = msg_count_const " << m_options.prp_count_const << " (msgs) " );
    }
  else if( m_options.prp == ProtocolOptions::PRP_MSG_COUNT_PARETO )
    {
      CBNDEBUG( "prp = msg_count_pareto " << m_options.prp_pareto_alpha << " (alpha) "
                << m_options.prp_pareto_kappa << " (kappa) " );
    }
  else if( m_options.prp == ProtocolOptions::PRP_MSG_COUNT_PARETO_MEAN )
    {
      CBNDEBUG( "prp = msg_count_pareto_mean " << m_options.prp_pareto_alpha << " (alpha) "
                << m_options.prp_pareto_mean << " (msgs) " );
      if( m_options.prp_pareto_alpha <= 1.0 ) throw logic_error( "alpha must be greater than 1.0" );
    }
  else
    {
      throw logic_error( "unrecognized PRP" );
    }

  if( m_options.rbfcp == ProtocolOptions::RBFCP_ON )
    {
      CBNDEBUG( "rbfcp = off" );
    }
  else if( m_options.rbfcp == ProtocolOptions::RBFCP_AIMD )
    {
      CBNDEBUG( "rbfcp = on, flow control window size: " << m_options.rbfcp_win );
    }
  else if( m_options.rbfcp == ProtocolOptions::RBFCP_OFF )
    {
      throw logic_error( "RBFCP no more available" );
    }
  else 
    {
      throw logic_error( "unrecognized RBFCP" );
    }

  if( m_options.msp == ProtocolOptions::MSP_CONST )
    {
      CBNDEBUG( "msp = const " << m_options.msp_const << " (msgs) " );
    }
  else if( m_options.msp == ProtocolOptions::MSP_PARETO )
    {
      CBNDEBUG( "msp = pareto " m_options.msp_pareto_alpha << " (alpha) "
                << m_options.msp_pareto_kappa << " (kappa) " );
    }
  else
    {
      throw logic_error( "unrecognized MSP" );
    }

if( m_options.qd == ProtocolOptions::QD_FIFO )
    {
      CBNDEBUG( "rlp = FIFO " );
    }
  else if( m_options.qd == ProtocolOptions::QD_DRR )
    {
      CBNDEBUG( "rlp = DRR " );
    }
  else
    {
      throw logic_error( "unrecognized RLP" );
    }
  if( m_options.to == ProtocolOptions::TO_OFF )
    {
      CBNDEBUG( "to = off " );
    }
  else if ( m_options.to == ProtocolOptions::TO_ON )
    {
      CBNDEBUG( "to = on, time_out: " << m_options.to_sender_time << " (ticks),  time out routert:  " << m_options.to_router_time << ", retransmission limit: " <<m_options.to_limit );
    }
  else if ( m_options.to == ProtocolOptions::TO_V )
    {
      CBNDEBUG( "to = on, time_out: " << m_options.to_sender_time << " (ticks),  time out routert:  " << m_options.to_router_time << ", retransmission limit: " <<m_options.to_limit );
    }
  else
    {
      throw logic_error( "unrecognized RAFTOP" );
    }
  
  if( m_options.srsp == ProtocolOptions::SRSP_NEVER )
    {
      CBNDEBUG( "srsp = never" );
    }
  else if( m_options.srsp == ProtocolOptions::SRSP_TIMED )
    {
      CBNDEBUG( "srsp = timed " << m_options.srsp_sec << " (sec), "
		<< m_options.srsp_time << " (ticks)" );
    }
  else if( m_options.srsp == ProtocolOptions::SRSP_TIMED_COUNT )
    {
      CBNDEBUG( "srsp = timed-count " << m_options.srsp_sec << " (sec), "
		<< m_options.srsp_time << " (ticks)" );
    }
  else
    {
      throw logic_error( "unrecognized SRSP" );
    }
  CBNDEBUG( "wtu = " << time_unit() );
  CBNDEBUG( "workload = " << input_fname() );
  CBNDEBUG( "*************************************************" );
}
