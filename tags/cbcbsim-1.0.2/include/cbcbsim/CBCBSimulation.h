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
#ifndef CBCBSIM_CBCBSIMULATION_H
#define CBCBSIM_CBCBSIMULATION_H
#define DEBUG 1
#define CBNDEBUG_ON 1

#include <cbnsim/CBNSimulation.h>
#include "cbcbsim/MSTBroadcastLayer.h"
#include "cbcbsim/ProtocolOptions.h"
#include "cbcbsim/SPTsBroadcastLayer.h"
#include <cbnsim/types.h>
#include <string.h>
#include <fstream>

namespace cbcbsim
{
  class CBCBSimulation : public cbnsim::CBNSimulation
    {
      //MSTBroadcastLayer	m_bl;
      SPTsBroadcastLayer m_bl;
      ProtocolOptions	m_options;
      
    public:
      //add a allocation of mem to write the value of time_block, for limiting the capacity  -liufeng
      ssim::Time time_block_CB;  
  
      CBCBSimulation();

      const BroadcastLayer& broadcast_layer() const
      { return m_bl; }

      void pip( ProtocolOptions::PredicateIssuePolicy pip )
      { m_options.pip = pip; }

      void pip_sec( ssim::Time pip_sec )
      { m_options.pip_sec = pip_sec; }

      void pre_run_simulation_hook() throw ();

      void prp( ProtocolOptions::PredicateRefreshPolicy prp )
      { m_options.prp = prp; }

      void prp_sec( ssim::Time prp_sec )
      { m_options.prp_sec = prp_sec; }

      void prp_count_const( int prp_count_const )
      { m_options.prp_count_const = prp_count_const; }

      void prp_pareto_alpha( double prp_pareto_alpha )
      { m_options.prp_pareto_alpha = prp_pareto_alpha; }

      void prp_pareto_mean( double prp_pareto_mean )
      { m_options.prp_pareto_mean = prp_pareto_mean; }

      void prp_pareto_kappa( int prp_pareto_kappa )
      { m_options.prp_pareto_kappa = prp_pareto_kappa; }

      void msp( ProtocolOptions:: MecSizePolicy msp )
      { m_options.msp = msp; }

      void msp_const( int msp_const )
      { m_options.msp_const = msp_const; }

      void msp_pareto_alpha( double msp_pareto_alpha )
      { m_options.msp_pareto_alpha = msp_pareto_alpha; }

      void msp_pareto_kappa( int msp_pareto_kappa )
      { m_options.msp_pareto_kappa = msp_pareto_kappa; }

      void request_order( ProtocolOptions::DataRequestOrder request_order )
      { m_options.request_order = request_order; }

      void rbfcp( ProtocolOptions:: ReceiverBasedFlowControlPolicy rbfcp )
      { m_options.rbfcp = rbfcp; }

      void rbfcp_win( unsigned int rbfcp_win )
      { m_options.rbfcp_win = rbfcp_win; }

      void qd( ProtocolOptions::QueueDiscipline qd )
      { m_options.qd = qd; }

      void sim_time_unit( ssim::Time sim_time_unit)
      { m_options.sim_time_unit = sim_time_unit; }

      void to( ProtocolOptions::TOPolicy to )
      { m_options.to = to; }

      void to_sender( ssim::Time to_sender )
      { m_options.to_sender = to_sender; }

      void to_router( ssim::Time to_router )
      { m_options.to_router = to_router; }

      void to_limit( unsigned int to_limit )
      { m_options.to_limit = to_limit; }

      void srsp( ProtocolOptions::SRSendPolicy srsp )
      { m_options.srsp = srsp; }
      
      void srsp_count( unsigned int srsp_count )
      { m_options.srsp_count = srsp_count; }

      void srsp_sec( ssim::Time srsp_sec )
      { m_options.srsp_sec = srsp_sec; }

      void cache( ProtocolOptions::CachingPolicy cache )
      { m_options.cache = cache; }
      
      void startup( double startup_limit )
      { m_options.sim_startup = startup_limit; }
      
      void filtering (ProtocolOptions::Filtering filt)
      { m_options.m_filtering = filt; }
      
      void n_classes( unsigned int classes)
      { 
	m_options.classes = classes;
      }
      
      void forwarding(ProtocolOptions::forwarding fwd)
      { m_options.fwd = fwd;}
      
      void cache_limit( double cache_limit )
      { m_options.cache_limit = cache_limit; }
      
      void cache_manag( ProtocolOptions::CacheManagement manag)
      { m_options.cache_manag = manag; }

      void file_store_perc(unsigned store_perc )
      { m_options.store_perc = store_perc; }
      
      void cache_manag_file( std::string str)
      { m_options.cache_manag_file = str; }
      
      void setup_link_hook( ssim::ProcessId from, ssim::ProcessId to, double length ) throw ()
      { m_bl.addLink( from, to, length ); }

      void setup_node_hook( cbnsim::Node* node ) throw ()
      { m_bl.addNode( node->pid() );}

      void setup_topology_hook() throw ();
      
      void pmax(double p)
      { m_options.pmax = p;}
      
      void beta(double b)
      { m_options.beta = b;}
      
      void fc_type(ProtocolOptions::FlowControllerType fc)
      { m_options.fc_type=fc;}
      
      void cbr_rate(double rate)
      { m_options.cbr_rate=rate;}

    };
};

#endif
