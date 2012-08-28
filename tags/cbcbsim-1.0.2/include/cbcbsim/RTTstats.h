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
#include "cbcbsim/FlowController.h"

#ifndef CBCBSIM_RTTstats_H
#define CBCBSIM_RTTstats_H

namespace cbcbsim
{
  
    /** @brief structure used to hold a sample in the RTT estimation
    */
    typedef struct{
      double				RTT_sample;
      std::multiset<double>::iterator	it_ordered_set;
    }sample;
    
    
   /** @brief Class used to collect RTT statistics
   */
  class RTTstats
  {  
        
    private:
    
      /** @brief maximum number of samples used for the statistics
      */
      unsigned int 			estimation_length;
      
      /** @brief samples orders by time arrival
      */
      std::list<sample>			samples;
      
      /** @brief samples orders by RTT value
      */
      std::multiset<double>		ordered_samples;
      
      /** @brief estimated timer (used by ICP)
      */
      double				timer_estim;
      
      /** @brief value used to compute timer_estim
      */
      double				threshold;
      
      /** @brief alpha parameter in the RTT ewma
      */
      double				alpha_rtt;
      
      double				RTT_avg;
      double				RTT_min;
      double				RTT_max;
      double 				RTT_actual;
   
    
    public:
    
      RTTstats(unsigned int estimation_length, double timer_estim, double threshold, double alpha);
     
    
      double max() {return RTT_max;}
      double min() {return RTT_min;}
      double actual() {return RTT_actual;}
      double avg() {return RTT_avg;}
      double timer() {return timer_estim;}

      /** @brief return the number of samples in samples 
       *  we use a sliding window of size estimation_length (max_samples())
       */
      int received_samples() {return samples.size();}
      unsigned int max_samples() {return estimation_length;}
      
      /** @brief computes the Exponentially-Weighted Moving Average of the RTT
      */   
      void ewma(double sample) {RTT_avg=(1-alpha_rtt)*RTT_avg+alpha_rtt*sample;}
      
      void samples_update(double chunk_RTT);
    
  };
};

#endif