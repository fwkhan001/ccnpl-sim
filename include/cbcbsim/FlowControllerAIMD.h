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


#ifndef CBCBSIM_FLOWCONTROLLERAIMD_H
#define CBCBSIM_FLOWCONTROLLERAIMD_H


namespace cbcbsim
{
  class FlowControllerAIMD : public FlowController
  { 
       
    protected: 
      
       /** @brief the type for the interst window (an hash with)
	*/
      typedef boost::unordered_map<unsigned int, ssim::Time> interest_win;
      
      /** @brief interest window size
       *  it is the maximum number of interests that this window controller can express
       *  each window controller is assigned to a specific interface
       */
      unsigned int	                m_interest_win_size;
      
      /** @brief the interest window, 
       *  unordered map that contains chunk id and interest issuing time for RTT_min estimation 
       */ 
      interest_win        		m_interest_win;
      
      /** @brief keeps track of the number of interests satisfied during the lifetime of this window controller 
       *  this param is used to compute the average window size
       */
      unsigned int			m_received;
      
      /** @brief cumulative value of the interest window 
       *  each time an interest is satisfied by a data
       */
      unsigned int			m_cumul_win;
      
      /** @brief receiver timer
       */
      ssim::Time			m_timer;
   
      
      /** @brief ordered map of chunk_ids for which a timer expired before the chunk reception
       */
      std::map<int, double> 		m_reissuing_chunks;
      
      /** @brief eta parameter of the AIMD window, multiplied by the window itself
       *  indicates the increment of the window after a complete window reception
       */
      unsigned int			m_win_inc;
      
    public:
    
      FlowControllerAIMD(unsigned int id, std::string content_name, unsigned int port, const ProtocolOptions& options, 
		    unsigned int ifid, ssim::Time start_time):
		    FlowController(id, content_name, port, options, ifid, start_time){}

      virtual ~FlowControllerAIMD(){}     
      
      /** @brief returns the current averegage window size
       */  
      double average_win(){ return (double)m_cumul_win/(double)m_received; }
	
      /** @brief manages a receiver TO expiration
       */  	
      void receiver_TO(int chunk_id );
      
      /** @brief managess a chunk reception
       *  - deletes the received chunk from win and eventually from m_reissuing_chunks that contains
       *    chunk_ids for which a timer expired before the chunk reception 
       *    (possibly lost chunk, need re-expression of the interestfrom the same 
       *    window controller)
       *  
       */
      virtual int received_chunk(int , bool , string )=0;
      //int received_chunk( int ,bool, string){return 1;}
	  
      /** @brief inserts a chunk id in the current window and launch a receiver timer
       *  NOTICE that once the chunk is assigned to one window controller (one for each interface), 
       *  it will be requested always from there! this was done in order to not modify the request 
       *  process by the receiver
       */
      int request_chunk(unsigned int,double);  
	  
      /** @brief increases the window
       *  Additive Increase
       */
      virtual void increase_win()=0;
      
      /** @brief decreases the window
       *  Multiplicative Decrease
       */
      virtual void decrease_win()=0;
      
      /** @brief returns the number of interests this window controller can express
       *  number of "free slots" in the interest window
       */      
      int free_slot_win(){return (m_interest_win_size-m_interest_win.size());}
	 
      
      /** @brief returns the value of the current interest window
       */      
      int interest_win_size(){return m_interest_win_size;}
	  
      /** @brief returns the first of the chunks that are in the list m_reissuing_chunks
       *  in order to re-schedule an Interest expression (notice that this req. may be 
       *  filtered by the PIT)
       */    
      std::pair<int,double> timed_out_chunk();
      
      /** @brief remove the first packet from the ordered set of timed_out chunks
       */    
      void pop_timed_out();
      
  };
  
  
};

#endif