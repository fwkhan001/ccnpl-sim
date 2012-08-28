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

#include <stdio.h>
#include <cbcbsim/cbcbsim_types.h>
#include <cbcbsim/ProtocolOptions.h>
#include <cbcbsim/CCNxPackets.h>
#include <iostream>
#include <cstring>
#include <set>
#include <siena/ssim.h>
#include "cbcbsim/PITable.h"//

using namespace ssim;
using namespace cbnsim;
using namespace std;

#ifndef CBCBSIM_LOAD_BALANCING_H
#define CBCBSIM_LOAD_BALANCING_H

namespace cbcbsim
{
  

  
  class CBCBNode;
   
  class SplitStats 
  {
    protected:
	unsigned int	win_counter;
	unsigned int	samples;
	double		alpha;
    public:
      	double		m_time_window_max;
	double		rate;
	Time		start_time;
	Time		time_prev;

	SplitStats() {}
	
	void init(Time start);
	  
	bool time_counter(Time t_now, bool sum);
	
	void ewma(double smp, ssim::Time &var) 
	{ var = alpha * var + (1-alpha) * smp;  }
 
  };
  
  typedef struct stats_entry{
    unsigned int ifid;
    unsigned int pending;
    double	 avg_pending;
    double	 weight;
    SplitStats	 split_stats;
  } stats_entry;
  
  typedef struct prefix_stats{
    vector<stats_entry>		stats;
    double			weight_sum;
  }prefx_stats;
     
  typedef boost::unordered_map<cname,prefix_stats> prefix_table;
  
  class LoadBalancing
  {
  public:
    prefix_table		m_ptable;
    CBCBNode*			m_node;
    double			m_alpha;
    
  
    LoadBalancing(CBCBNode* node): m_node(node),m_alpha(0.9){}
    
    
    /** @brief add an entry to the prefix table
     */ 
    void add(cname pn,unsigned int ifid);
    
    /** @brief remove an entry to the prefix table
     */ 
    void del(cname pn){m_ptable.erase(pn);};
    
    /** @brief update number of pending interests and some other variables
     *  op == '+' indicates that we are updating after an interest fwd
     *  op == '-' indicates that we are updating after a data reception 
     */ 
    void pending(cname pn, unsigned int ifid, char op);
    
    /** @brief update the interest average delay 
     */ 
    void avg_delay(cname pn, unsigned int ifid, Time reg_time);
    
    /** @brief computation of the exponentially-weighted moving average
     */ 
    void ewma(unsigned int smp, double &var, double alpha) { var = alpha * var + (1-alpha) * smp;  }
    

    /** @brief computes the interface to which forward the interest, given its prefix
     */
    unsigned int fwd_interest(cname pname);
    
    /** @brief rnd forward
     */
    unsigned int rnd(prefix_table::iterator pti);
        
    /** @brief weighted rnd forward
     */
    unsigned int weighted_rnd(prefix_table::iterator pti);
    

  };
    
}

#endif
