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
#ifndef CBCBSIM_CBCB_DATA_COLLECTOR_H
#define CBCBSIM_CBCB_DATA_COLLECTOR_H

#include "cbnsim/DataCollector.h"
#include "cbcbsim/CBCBNode.h"
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <fstream>





namespace cbcbsim
{
  class CBCBDataCollector : public cbnsim::DataCollector
    {
      unsigned int  m_ra_inl_count;
      unsigned int  m_ra_tot_count;
      unsigned int  m_me_inl_count;
      unsigned int  m_me_tot_count;
      unsigned int  m_srur_inl_count;
      unsigned int  m_srur_tot_count;
      unsigned int  m_interest_inl_count;
      unsigned int  m_interest_tot_count;
      unsigned int  m_data_inl_count;
      unsigned int  m_data_tot_count;
      unsigned int  m_open_connection;
      std::vector<string> m_files;
      
      std::vector<std::vector<unsigned long long> >  m_miss;
      std::vector<std::vector<unsigned long long> >  m_hit;
      std::vector<std::vector<unsigned long long> >  m_filter;
      std::vector<std::vector<unsigned long long> >  m_miss_tot;
      std::vector<std::vector<unsigned long long> >  m_hit_tot;
      std::vector<std::vector<unsigned long long> >  m_filter_tot;
      
      std::vector<std::vector<double> >		     m_rate;
      std::vector<std::vector<double> >		     m_rate_tot;
      std::vector<std::vector<double> >		     m_loss;
      std::vector<std::vector<double> >		     m_loss_tot;
      std::vector< prefix_table > m_split_ratio;

      unsigned int intervals;
      double sim_length;
      
      std::vector<std::vector<double> > m_queues_size;
      std::vector<std::vector<ssim::ProcessId> > m_if;
      const ProtocolOptions&	m_options;
      std::map<std::string, std::vector<string> > m_filesmap;
      std::map<std::string, std::vector<string> > m_print_filesmap;    
    
    public:
    CBCBDataCollector(ssim::Time* time_block, const ProtocolOptions& options) : m_srur_inl_count( 0 ), m_srur_tot_count( 0 ), m_interest_inl_count( 0 ),  m_interest_tot_count( 0 ), m_data_inl_count( 0 ), m_data_tot_count( 0 ), m_options( options )
    {
      time_block_collector = time_block; 
    }
    
      ssim::Time* time_block_collector;
      ssim::Time last_time_block;
      ssim::Time interval;
  
      void init_structures();
      
      void poll_node( cbnsim::Node& node, ssim::Time time  );
      
      void output_inline( std::ostream& out, ssim::Time time );

      void output_final( std::ostream& out, ssim::Time time );

      
    };
};

#endif
