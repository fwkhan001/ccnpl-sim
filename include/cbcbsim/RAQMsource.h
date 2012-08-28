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

#include <cbnsim/types.h>
#include <siena/forwarding.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <limits>
#include <cbcbsim/cbcbsim_types.h>
#include "cbcbsim/BroadcastLayer.h"
#include "cbcbsim/ProtocolOptions.h"
#include <cbnsim/WorkloadEventParser.h>
#include "cbcbsim/RAQM.h"


#ifndef CBCBSIM_RAQM_source_H
#define CBCBSIM_RAQM_source_H



namespace cbcbsim
{
  /**this class can be used to collect rate statistics
  
//   class Stats 
//   {
//     protected:
// 	unsigned int	win_counter;
// 	unsigned int	samples;
// 	double		m_time_window_max;
// 	double		alpha;
// 	double		m_value;
// 	double		average;
//     public:
// 	double		rate;
// 	double		arrival_time;
// 	double		time_prev;
// 
// 	Stats():win_counter(0),samples(0),m_time_window_max(0.1),alpha(0.9),m_value(0.0),average(0.0),
// 	rate(0.0),arrival_time(0.0),time_prev(0.0) {}
// 
// 	bool time_counter(double t_now, double value)
// 	{
// 	  unsigned int new_measure = (unsigned int)floor((t_now - arrival_time)/m_time_window_max);
// 	  if ( win_counter == new_measure )
// 	  {
// 	    samples++;
// 	    m_value+=value;
// 	    return false;
// 	  }
// 	  ewma((double)samples/m_time_window_max, rate);
// 	  ewma((samples) ? m_value/(double)samples: 0, average);
// 	  samples = 0;
// 	  m_value = 0.0;
// 	  win_counter++;
// 	  time_counter(t_now, value);
// 	  return true;
// 	}
// 	
// 	double get_average() { return average; }
// 	
// 	double get_rate() { return rate; }
// 	
// 	void ewma(double smp, ssim::Time &var) 
// 	{ var = alpha * var + (1-alpha) * smp;  }
//  
//   }; **/
  
  /** @brief Node class for socket that implements a window protocol.
   */
  class RAQMsource : public RAQM
  {  
   
    protected:
      
       boost::unordered_map<string, RTTstats*> m_RTT_per_path;
       
       std::map<string,int> routes_usage; 
       
    public:
      
    RAQMsource(unsigned int id, string content_name, unsigned int port, const cbcbsim::ProtocolOptions& options, 
	       unsigned int ifid, ssim::Time start_time):RAQM(id,content_name,port,options,ifid,start_time){}
	       
    ~RAQMsource(){
      boost::unordered_map<string, RTTstats*>::iterator it;
      for(it=m_RTT_per_path.begin();it!=m_RTT_per_path.end();it++)
	  delete((*it).second);
      };
      
      /** @brief managess a chunk reception
        *  - deletes the received chunk from win and eventually from m_reissuing_chunks that contains
        *    chunk_ids for which a timer expired before the chunk reception 
        *    (possibly lost chunk, need re-expression of the interestfrom the same 
        *    window controller)
        *  - updates RTT_min, RTT and timer
        */  	
       int received_chunk( int ,bool, string);

	/** @brief probabilistic increase/decrease of window size
        *  
        */
       void remoteAQM(double cur_RTT, string source);
      
       /** @brief return a map with all the routes used from the cotroller
        *  
        */
       std::map<string,int> get_paths_stat();
   
  };
};


#endif

