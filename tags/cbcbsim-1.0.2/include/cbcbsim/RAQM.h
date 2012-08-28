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
#include "cbcbsim/FlowControllerAIMD.h"
#include "cbcbsim/RTTstats.h"

#ifndef CBCBSIM_RAQM_H
#define CBCBSIM_RAQM_H



namespace cbcbsim
{
  /** @brief Node class for socket that implements a window protocol.
   */
  class RAQM : public FlowControllerAIMD
  {  
 
    protected:
      
      
      /** @brief real interest window size
       *  it is the maximum number of interests that this window controller should express
       *  we use m_interest_win_size as approximation of this real window
       */
      double 				m_interest_win_size_double;
      
      /** @brief decreasing factor
       *  at each received packet the window is decresed by this factor with probability
       *  that depends by the RTT
       */
      double 				beta;
      
      /** @brief structure used to estimate the RTT_min 
       *  (with the formula used in TCP-LP algorithm)
       */
      RTTstats*				m_RTT_stats;
      
      
    public:
    
      RAQM(unsigned int m_id, std::string content_name, unsigned int m_port, const ProtocolOptions& options, 
	  unsigned int ifid, ssim::Time start_time):FlowControllerAIMD(m_id, content_name, m_port, options, ifid, 
	  start_time){init();}
		    
      virtual ~RAQM(){delete(m_RTT_stats);}

      
      /** @brief initialization
       *
       **/
      void init();
      
      /** @brief managess a chunk reception
       *  - deletes the received chunk from win and eventually from m_reissuing_chunks that contains
       *    chunk_ids for which a timer expired before the chunk reception 
       *    (possibly lost chunk, need re-expression of the interestfrom the same 
       *    window controller)
       *  - updates RTT_min, RTT and timer
       */  	
      virtual int received_chunk( int ,bool, string);
	  
      /** @brief increases the window
       *  Additive Increase
       */
      void increase_win();
      
      /** @brief decreases the window
       *  Multiplicative Decrease
       */
      void decrease_win();
      
      /** @brief probabilistic increase/decrease of window size
       *  
       */
      virtual void remoteAQM(double cur_RTT,string source);
      
  };

};

#endif
