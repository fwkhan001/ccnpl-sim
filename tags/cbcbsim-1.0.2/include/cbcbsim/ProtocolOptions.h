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
#ifndef CBCBSIM_PROTOCOLOPTIONS_H
#define CBCBSIM_PROTOCOLOPTIONS_H

#include <siena/ssim.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cbnsim/types.h>
#include <cbcbsim/cbcbsim_types.h>
#include <map>
#include "cbcbsim_types.h"

namespace cbcbsim
{
  /** @brief Encapsulates the various protocol options that are available.
   */
  class ProtocolOptions
  {
  public:
    enum PredicateIssuePolicy { PIP_IMMEDIATE, PIP_TIMED };
    enum PredicateRefreshPolicy { PRP_ALWAYS, PRP_TIMED, PRP_MSG_COUNT_CONST, PRP_MSG_COUNT_PARETO, PRP_MSG_COUNT_PARETO_MEAN };
    enum SRSendPolicy { SRSP_NEVER, SRSP_TIMED, SRSP_TIMED_COUNT };
    enum CachingPolicy { CACHING_ON_RND, CACHING_ON_LRU, CACHING_ON_AUTO, CACHING_OFF };
    enum MecSizePolicy { MSP_CONST, MSP_PARETO };
    enum QueueDiscipline { QD_FIFO, QD_DRR };
    enum ReceiverBasedFlowControlPolicy {RBFCP_ON, RBFCP_OFF, RBFCP_AIMD};
    enum TOPolicy { TO_ON, TO_V, TO_OFF };
    enum DataRequestOrder { INSEQUENCE, RANDOM };
    enum Filtering { FILTERING_ON, FILTERING_OFF };
    enum CacheManagement { MANAGEMENT_STANDARD, MANAGEMENT_DIFF, MANAGEMENT_CODING};
    enum forwarding { FWD_RND,FWD_BALANCE };
    enum FlowControllerType{fc_ICP,fc_RAQM,fc_RAQMsource,fc_CBR};
    
    


    ProtocolOptions()
    {
      pip = PIP_IMMEDIATE; pip_sec = 0; pip_time = 0;
      prp = PRP_ALWAYS; prp_sec = 0; prp_time = 0; prp_count_const = 0; prp_pareto_mean = 0; prp_pareto_alpha = 0; prp_pareto_kappa = 0;
      srsp = SRSP_NEVER; srsp_count = 0; srsp_sec = 0; srsp_time = 0;
      cache = CACHING_OFF; cache_limit = 0;
      msp = MSP_CONST; msp_const = 0; msp_pareto_alpha = 0; msp_pareto_kappa = 0;
      to = TO_ON; to_sender = 0; to_router = 0; to_limit = 0;
      rbfcp = RBFCP_ON; rbfcp_win = 1;
      request_order = INSEQUENCE; 
      sim_startup = 0;
      m_filtering = FILTERING_ON;
      cache_manag = MANAGEMENT_STANDARD;
      store_perc = 100;
      fwd = FWD_RND; // default forward random to all available interfaces
      m_minimum_chunk_size = 8; // min chunk_size in kbit, used to correctly size the hash table for the cache
      pmax=0.5;
      beta=0.5;
      fc_type=fc_RAQMsource;
    }

    PredicateIssuePolicy		pip;
    ssim::Time				pip_sec;
    ssim::Time				pip_time;
    PredicateRefreshPolicy		prp;
    ssim::Time				prp_sec;
    ssim::Time				prp_time;
    unsigned int			prp_count_const;
    double				prp_pareto_mean;
    double				prp_pareto_alpha;
    unsigned int			prp_pareto_kappa;
    SRSendPolicy			srsp;
    unsigned int			srsp_count;
    ssim::Time				srsp_sec;
    ssim::Time				srsp_time;
    CachingPolicy			cache;
    double				cache_limit;
    MecSizePolicy			msp;
    unsigned int			msp_const;
    double				msp_pareto_alpha;
    unsigned int			msp_pareto_kappa;
    ssim::Time				to_sender;
    ssim::Time				to_router;
    int					to_limit;
    ssim::Time				to_sender_time;   //this is the receiver time out
    ssim::Time				to_router_time;   //now this is the PIT time out
    ssim::Time				sim_length;
    TOPolicy				to;
    QueueDiscipline			qd;
    std::string				m_clients_file;
    std::string				m_routing_file;
    std::map<std::string, std::vector<std::string> >	m_filesmap;
    std::map<std::string, std::vector<std::string> >	m_print_filesmap;
    std::map<unsigned int,unsigned int >	m_clients;
    ssim::Time				sim_time_unit;
    ReceiverBasedFlowControlPolicy	rbfcp;
    DataRequestOrder			request_order;
    unsigned int			rbfcp_win;
    double 				pmax;
    double 				beta;
    FlowControllerType			fc_type;
    double				cbr_rate;
    
    /** @brief Sim. startup fase in which statistics are not collected. 
     * expressed in seconds
     */
    double				sim_startup; 
    
    Filtering				m_filtering;
    CacheManagement 			cache_manag;
    std::string 			cache_manag_file;
    unsigned int 			classes;
    unsigned int 			store_perc;
    routing_map				m_static_routing_info;
    forwarding				fwd;
    ssim::Time				m_fwd_info_interval;
    
    unsigned int			m_minimum_chunk_size;
  };
};
#endif
