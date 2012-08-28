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
#include "cbcbsim/RAQMsource.h"
#include <cbnsim/ReceiverTO.h>



using namespace cbcbsim;
using namespace ssim;
using namespace std;


int RAQMsource :: received_chunk( int received_chunk, bool retransmitted, string source)
{	

  
  double cur_RTT=0;
  
  /** @brief update statistics
   */
  m_cumul_win+=m_interest_win_size;
  m_received++;
  
  std::map< string, int >::iterator routes_it=routes_usage.find(source);
  if(routes_it==routes_usage.end()){
    routes_usage.insert(make_pair(source,1));
  }else{
    (*routes_it).second++;
  }
    
  /** @brief if received chunk, was in the reissuing list (it arrived after the timer expiration)
   */
  std::map <int, double >::iterator reissuing_it;
  reissuing_it=m_reissuing_chunks.find(received_chunk);
  if (reissuing_it != m_reissuing_chunks.end())
  {
    cur_RTT=Sim::clock()-(*reissuing_it).second;
    m_reissuing_chunks.erase(reissuing_it);
  }
  else
  {
    interest_win::iterator win_ite;
    win_ite = m_interest_win.find(received_chunk);
    if ( win_ite != m_interest_win.end() )
    {
      cur_RTT = Sim::clock()-(*win_ite).second;
      m_interest_win.erase(win_ite);
      
      if((retransmitted==false)){
	boost::unordered_map<string, RTTstats* >::iterator it;
	it = m_RTT_per_path.find(source);
	if (it==m_RTT_per_path.end()){
	   /** @brief initialize the RTTstats for this source
	    * for more info look RAQM::init()
	    */
	   m_RTT_per_path.insert(make_pair(source,new RTTstats(30, 0.0, 0.5, 0.1)));
	   it = m_RTT_per_path.find(source);
	}
	(*it).second->samples_update(cur_RTT);
      }
      increase_win();
      remoteAQM(cur_RTT,source);
    }
  }
  
  return 1;
}


void RAQMsource::remoteAQM(double RTT, string source){
  double p_decrease = 0.0;
  double p_min = 0.00001;
  
//   boost::unordered_map<string, RTTstats* >::iterator p;
//   for(p=m_RTT_per_path.begin();p!=m_RTT_per_path.end();p++)
//      cout << Sim::clock() << " Timers " << (*p).first << " content " << m_content_name << " RTT " << (*p).second->actual() << " min "
//      << (*p).second->min() << " max " << (*p).second->max() << " T " << (*p).second->timer() << endl; 
    
  boost::unordered_map<string, RTTstats* >::iterator it;
  it = m_RTT_per_path.find(source);
  if(it!=m_RTT_per_path.end()){
    RTTstats* RTTs = (*it).second;
    if(RTTs->received_samples()==RTTs->max_samples()){
      if (RTTs->max()==RTTs->min())
	p_decrease=p_min;
      else
	p_decrease=p_min+m_options.pmax*(RTT-RTTs->min())/(RTTs->max()-RTTs->min());
    }
//       cout << m_content_name << " " << source << " " << RTTs->received_samples() << " " << RTTs->max_samples() <<
//       " " << p_decrease << endl;
  }
  
   double val = (double) rand()/RAND_MAX;
  
   if (p_decrease > val)
     decrease_win();
}


std::map<string,int> RAQMsource::get_paths_stat()
{
   boost::unordered_map<string, RTTstats*>::iterator it;
      for(it=m_RTT_per_path.begin();it!=m_RTT_per_path.end();it++)
	 if((*it).second->received_samples()<(*it).second->max_samples()){
	   routes_usage.erase((*it).first);
	 }
  return routes_usage;
}