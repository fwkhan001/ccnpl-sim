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

#include "cbcbsim/RAQM.h"
#include <cbnsim/ReceiverTO.h>


using namespace cbcbsim;
using namespace ssim;
using namespace std;

void RAQM :: init()
{
  /** @brief initial value for the timer (input of the sim.)
    */
  m_timer = m_options.to_router_time;
  
  /** @brief initialize RTTstats
    * RTTstats(unsigned int estimation_length, double timer_estim, double threshold, double rtt_alpha);
    * estimation_length: number of samples used for the statistics
    * timer_estim: not used
    * threshold: not used
    * rtt_alpha: alpha used to compute the RTT ewma 
    */
  
  
  m_RTT_stats = new RTTstats(30, 0.0, 0.5, 0.1);
    
  /** @brief initialize interest window parameters
    */
  m_interest_win_size = m_options.rbfcp_win; 
  m_interest_win_size_double = m_options.rbfcp_win;
  m_received =0;
  m_cumul_win = 0;
  beta=m_options.beta;
  m_cumul_win = 0;
  
  /** @brief assign the hash size to 20 times the initial value 
    *  (collisions are managed by the boost::unordered_map structure using chaining )
    */   
  m_interest_win.rehash(m_options.rbfcp_win*20);
  m_win_inc=1;

}


int RAQM :: received_chunk( int received_chunk, bool retransmitted, string source)
{	
 
  double cur_RTT=0;
  
  /** @brief update statistics
   */
  m_cumul_win+=m_interest_win_size;
  m_received++;

  
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
	 /** @brief if this chunk_id was not requested more than one time
	  *  update timer_estim estimation
	  */
	 if((retransmitted==false))
	  m_RTT_stats->samples_update(cur_RTT);
	increase_win();
	remoteAQM(cur_RTT,source);
    }
  }
  return 1;
}


void RAQM :: increase_win( )
{

  m_interest_win_size_double+=m_win_inc/m_interest_win_size_double;
  m_interest_win_size=floor(m_interest_win_size_double);
//   cout << "WIN+\t"<< content_name() << "\tnode=" << nid() << "\tface=" << ifid() << "\t"  << Sim::clock() << "\t" 
//   << m_interest_win_size << "\t" << m_interest_win_size_double << endl;

}

void RAQM :: decrease_win( )
{ 
  double new_win_size = m_interest_win_size_double * (1-beta);
  if (new_win_size > 1){
    m_interest_win_size_double = new_win_size;
    m_interest_win_size = floor(m_interest_win_size_double);
//     cout << "WIN-\t"<< content_name() << "\tnode=" << nid() << "\tface=" << ifid() << "\t"  << Sim::clock() << "\t" 
//     << m_interest_win_size << "\t" << m_interest_win_size_double << endl;
  }

}


void RAQM::remoteAQM(double RTT, string source){
  double p_decrease = 0.0;
  double p_min = 0.00001;

  if(m_RTT_stats->received_samples()==m_RTT_stats->max_samples()){
    if (m_RTT_stats->max()==m_RTT_stats->min())
      p_decrease=p_min; 
    else
      p_decrease=p_min + m_options.pmax*(RTT-m_RTT_stats->min())/(m_RTT_stats->max()-m_RTT_stats->min());
  }
  
  double val = (double) rand()/RAND_MAX;
  
  if (p_decrease > val)
    decrease_win();
  
}
