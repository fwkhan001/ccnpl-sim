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

#include <math.h>
#include "cbcbsim/CBCBDataCollector.h"
#include "cbcbsim/CBCBNode.h"

using namespace cbcbsim;
using namespace cbnsim;
using namespace std;


void CBCBDataCollector :: init_structures()
{
  m_hit.resize( m_nodes->size(),std::vector<unsigned long long>  (m_options.classes,0));
  m_miss.resize( m_nodes->size(),std::vector<unsigned long long>  (m_options.classes,0));
  m_filter.resize( m_nodes->size(),std::vector<unsigned long long>  (m_options.classes,0));
  m_hit_tot.resize( m_nodes->size(),std::vector<unsigned long long>  (m_options.classes,0));
  m_miss_tot.resize( m_nodes->size(),std::vector<unsigned long long>  (m_options.classes,0));
  m_filter_tot.resize( m_nodes->size(),std::vector<unsigned long long>  (m_options.classes,0));
  std::map<cname, std::vector<unsigned long long> > stat;

  //TODO generalize the std::vector <Node* > using the typedef NodeList already in DataCollector.h
  //     when modify and compile cbnsim, it does not take into account the modifications on .h files
  for (std::vector <Node* >::iterator node = m_nodes->begin();node != m_nodes->end(); node++)
  {
    CBCBNode& n = static_cast<CBCBNode&>( *(*node) );
    m_if.push_back(n.m_if_table);
    m_loss_tot.push_back(std::vector<double> (n.m_if_table.size(),0));
    m_rate_tot.push_back(std::vector<double> (n.m_if_table.size(),0));
    m_queues_size.push_back(std::vector<double> (n.m_if_table.size(),0));
    m_split_ratio.push_back(prefix_table (n.m_if_table.size()));
  }
  
}

void CBCBDataCollector :: poll_node( cbnsim::Node& node, ssim::Time time  )
{
  CBCBNode& n = static_cast<CBCBNode&>( node );
  m_srur_inl_count += n.srur_packets();
  m_srur_tot_count += n.srur_packets();
  n.reset_srur_packets();
  m_ra_inl_count += n.ra_packets();
  m_ra_tot_count += n.ra_packets();
  n.reset_ra_packets();
  m_me_inl_count += n.me_packets();
  m_me_tot_count += n.me_packets();
  n.reset_me_packets();
  
  m_interest_inl_count += n.interest_packets();
  m_interest_tot_count += n.interest_packets();
  m_data_inl_count += n.data_packets();
  m_data_tot_count += n.data_packets();
  n.reset_data_packets();
  
  
  if (time > (m_options.sim_startup/m_options.sim_time_unit ) )
  {
    
    //HIT stats
    std::vector<unsigned long long> hit = n.hit_rate();   
    for (unsigned int i = 0; i < hit.size();i++)
    {
      m_hit[n.id()][i] = hit[i];
      m_hit_tot[n.id()][i] +=  hit[i];
    }

    //MISS stats
    std::vector<unsigned long long> miss = n.miss_rate();   
    for (unsigned int i = 0; i < hit.size();i++)
    {
      m_miss[n.id()][i] = miss[i];
      m_miss_tot[n.id()][i] +=  miss[i];
    }

    //FILTER stats
    std::vector<unsigned long long> filter = n.filter_rate();   
    for (unsigned int i = 0; i < hit.size();i++)
    {
      m_filter[n.id()][i] = filter[i];
      m_filter_tot[n.id()][i] +=  filter[i];
    }
     
    //RATE stats 
    m_rate.push_back(n.link_rate());
    m_loss.push_back(n.loss_rate());
    
    for(unsigned int iface = 0;iface < m_rate[n.id()].size(); iface++)
      m_rate_tot[n.id()][iface]+=m_rate[n.id()][iface];
    
    for(unsigned int iface = 0;iface < m_loss[n.id()].size(); iface++)
      m_loss_tot[n.id()][iface]+=m_loss[n.id()][iface];
    
    //QUEUE stats
    m_queues_size[n.id()]= n.queues_length();
    
    //SPLIT stats
    m_split_ratio[n.id()]=n.split_stats();

  }

  n.reset_hit_rate();
  n.reset_miss_rate();
  n.reset_filter_rate();
  n.reset_link_rate();
  n.reset_loss_rate();
}

void CBCBDataCollector :: output_inline( std::ostream& out, ssim::Time time )
{
    
   ssim::Time time_s = time*m_options.sim_time_unit;
  out << endl << "Stats Time  " << time_s << endl << endl;

  unsigned long long request = 0;
  double miss_prob =0;
  double hit_prob =0;
  double filter_prob =0;
  
  if (time_s<m_options.sim_startup)
    cout << "# Warning: stratup phase not completed. No data collected!" << endl;
  out << "#Legend" << endl;
  out << "#CACHE FINAL NODE | node_id | miss_rate | hit_rate | request_rate | miss_prob (with filt) | hit_prob |" 
      << "filter_prob | miss_prob (w/o filt) | \"class\" class_id " << endl << endl<< endl;
      
  out << "Stats:"  << endl << endl;
    
  for (unsigned int n=0;n < m_nodes->size(); n++)
  {
    for (unsigned int c=0;c< m_options.classes;c++)
    {
      request = m_miss_tot[n][c]+m_hit_tot[n][c];
      if ( request != 0 )
      {
	miss_prob = (double)m_miss_tot[n][c]/(double)request;
	hit_prob = (double)m_hit_tot[n][c]/(double)request; 
	filter_prob = (double)m_filter_tot[n][c]/(double)request;
      }
      else
      {
	miss_prob = 0;
	hit_prob = 0;
	filter_prob = 0;
      }
      /** Print out the statistics; notice that class i is saved in vector's position i-1
       */
      out << "CACHE " << time_s << " NODE "<< n << " " << m_miss_tot[n][c]/(time_s-m_options.sim_startup) << " " 
	  << m_hit_tot[n][c]/(time_s-m_options.sim_startup) << " "  << request/(time_s-m_options.sim_startup) 
	  << " " << miss_prob <<" "<< hit_prob << " " << filter_prob << " " 
	  << (m_miss_tot[n][c]-m_filter_tot[n][c])/(time_s-m_options.sim_startup) << " class "<< c+1 << " " << std::endl;
    }
  }  
  out << std::endl;
  

  out << std::endl << "QUEUE stats: " << endl << endl;

  for( NodeList::iterator i = m_nodes->begin();i != m_nodes->end(); ++i )
  {
    CBCBNode& n = static_cast<CBCBNode&>( *(*i) );
    for (unsigned int j=1; j<m_queues_size[n.id()].size(); j++){
      out << "QUEUE " << time_s << " NODE " << n.id() << " to " << n.i2p(j) << " " << m_queues_size[n.id()][j] << endl; 
    }
  }
  
  out << endl;
  
  out << endl;
  out << std::endl << "SPLIT stats: " << endl << endl;

  for( NodeList::iterator i = m_nodes->begin();i != m_nodes->end(); ++i )
  {
    CBCBNode& n = static_cast<CBCBNode&>( *(*i) );
    prefix_table::iterator it;
    for (it=m_split_ratio[n.id()].begin();it!=m_split_ratio[n.id()].end();it++){
      out << "Prefix " << (*it).first << endl;
      double sum_rates =0.0;
      for(unsigned int j=0; j<(*it).second.stats.size(); j++){
	sum_rates+=(*it).second.stats[j].split_stats.rate;
      }
      for(unsigned int j=0; j<(*it).second.stats.size(); j++){
	if (sum_rates!=0)
	  out << "SPLIT " << time_s << " NODE " << n.id() << " to " << n.i2p((*it).second.stats[j].ifid) << " " 
	  << (*it).second.stats[j].split_stats.rate/sum_rates << " " << (*it).second.stats[j].split_stats.rate << endl;
	else
	  out << "SPLIT " << time_s << " NODE " << n.id() << " to " << n.i2p((*it).second.stats[j].ifid) << " 0 0 " << endl;
      }
    }
  }
  
  out << endl;
  
}

void CBCBDataCollector :: output_final( std::ostream& out, ssim::Time time )
{
  ssim::Time time_s = time*m_options.sim_time_unit;
  out << endl << "FINAL Statistics: " << endl << endl;

  unsigned long long request = 0;
  double miss_prob =0;
  double hit_prob =0;
  double filter_prob =0;
  
  if (time_s<m_options.sim_startup)
    cout << "# Warning: stratup phase not completed. No data collected!" << endl;
  out << "#Legend" << endl;
  out << "#CACHE FINAL NODE | node_id | miss_rate | hit_rate | request_rate | miss_prob (with filt) | hit_prob |" 
      << "filter_prob | miss_prob (w/o filt) | \"class\" class_id " << endl << endl<< endl;
      
  out << "Stats:"  << endl << endl;
    
  for (unsigned int n=0;n < m_nodes->size(); n++)
  {
    for (unsigned int c=0;c< m_options.classes;c++)
    {
      request = m_miss_tot[n][c]+m_hit_tot[n][c];
      if ( request != 0 )
      {
	miss_prob = (double)m_miss_tot[n][c]/(double)request;
	hit_prob = (double)m_hit_tot[n][c]/(double)request; 
	filter_prob = (double)m_filter_tot[n][c]/(double)request;
      }
      else
      {
	miss_prob = 0;
	hit_prob = 0;
	filter_prob = 0;
      }
      /** Print out the statistics; notice that class i is saved in vector's position i-1
       */
      out << "CACHE FINAL NODE "<< n << " " << m_miss_tot[n][c]/(time_s-m_options.sim_startup) << " " 
	  << m_hit_tot[n][c]/(time_s-m_options.sim_startup) << " "  << request/(time_s-m_options.sim_startup) 
	  << " " << miss_prob <<" "<< hit_prob << " " << filter_prob << " " 
	  << (m_miss_tot[n][c]-m_filter_tot[n][c])/(time_s-m_options.sim_startup) << " class "<< c+1 << " " << std::endl;
    }
  }  
  out << std::endl;
  

  out << std::endl << "QUEUE stats: " << endl << endl;

  for( NodeList::iterator i = m_nodes->begin();i != m_nodes->end(); ++i )
  {
    CBCBNode& n = static_cast<CBCBNode&>( *(*i) );
    for (unsigned int j=1; j<m_queues_size[n.id()].size(); j++){
      out << "QUEUE FINAL NODE " << n.id() << " to " << n.i2p(j) << " " << m_queues_size[n.id()][j] << endl; 
    }
  }
  
  out << endl;
  
  out << endl;
  out << std::endl << "SPLIT stats: " << endl << endl;

  for( NodeList::iterator i = m_nodes->begin();i != m_nodes->end(); ++i )
  {
    CBCBNode& n = static_cast<CBCBNode&>( *(*i) );
    prefix_table::iterator it;
    for (it=m_split_ratio[n.id()].begin();it!=m_split_ratio[n.id()].end();it++){
      out << "Prefix " << (*it).first << endl;
      double sum_rates =0.0;
      for(unsigned int j=0; j<(*it).second.stats.size(); j++){
	sum_rates+=(*it).second.stats[j].split_stats.rate;
      }
      for(unsigned int j=0; j<(*it).second.stats.size(); j++){
	if (sum_rates!=0)
	  out << "SPLIT FINAL NODE " << n.id() << " to " << n.i2p((*it).second.stats[j].ifid) << " " 
	  << (*it).second.stats[j].split_stats.rate/sum_rates << " " << (*it).second.stats[j].split_stats.rate << endl;
	else
	  out << "SPLIT FINAL NODE " << n.id() << " to " << n.i2p((*it).second.stats[j].ifid) << " 0 0" << endl;
      }
    }
  }
  
  out << endl;
  
}
