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

#include <vector>
#include <siena/ssim.h>
#include <cbnsim/types.h>
#include <siena/forwarding.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <limits>
#include <cbcbsim/cbcbsim_types.h>
#include "cbcbsim/BroadcastLayer.h"
#include "cbcbsim/ProtocolOptions.h"

#ifndef CBCBSIM_FLOWCONTROLLER_H
#define CBCBSIM_FLOWCONTROLLER_H



namespace cbcbsim
{
  
  class FlowController
  { 
    protected:
      
       /** @brief id of the node from which this window controller belongs to
       *  NOTICE that we need this parameter in order to schedule receiver timer to the Simulator 
       */
      unsigned int 			m_id;
      
       /** @brief content name of the chunks requested through this  group of receiver sockets 
        *  NOTICE that we need this parameter in order to schedule receiver timer to the Simulator 
        */     
      cname 				m_content_name;
      
      /** @brief port number to which the upper SocketManager is associated
       *  NOTICE that we need this parameter in order to reconstruct the socket_address (content_name+"port:"+m_port)
       *  that is needed when scheduling a receiver timer
       */  
      unsigned int			m_port;
      
      /** @brief simulation options
       */      
      const ProtocolOptions&		m_options;
      
      /** @brief interface id from which this window controller belongs to
       */          
      unsigned int			m_ifid;
         
      /** @brief start time of the socket
       */
      ssim::Time			m_start_time;
      
      
      
    public:
      
      FlowController(unsigned int id, std::string content_name, unsigned int port, const ProtocolOptions& options, 
		    unsigned int ifid, ssim::Time start_time):m_id(id),m_content_name(content_name),m_port(port),
		    m_options(options),m_ifid(ifid),m_start_time(start_time){}
 
      virtual ~FlowController(){}
      
       /** @brief returns the node id 
       */
      unsigned int nid(){return m_id;}      
      
      /** @brief returns the content name this window controller is associated to
       */
      cname content_name(){return m_content_name;}   
	
      /** @brief returns the interf. id this window controller is associated to
       */
      unsigned int ifid(){return m_ifid;}
 
      /** @brief returns the port this window controller is associated to
       */
      unsigned int port(){return m_port;}
      
      /** @brief returns the start time
       */
      ssim::Time start_time(){return m_start_time;}
      
      /** @brief returns the current averegage window size
       */  
      virtual double average_win()=0;
	
      /** @brief manages a receiver TO expiration
       */  	
      virtual void receiver_TO(int chunk_id){}
      
      /** @brief managess a chunk reception
       *  - deletes the received chunk from win and eventually from m_reissuing_chunks that contains
       *    chunk_ids for which a timer expired before the chunk reception 
       *    (possibly lost chunk, need re-expression of the interestfrom the same 
       *    window controller)
       * 
       */  	
      virtual int received_chunk( int ,bool, std::string )=0;
	  
      /** @brief inserts a chunk id in the current window and launch a receiver timer
       *  NOTICE that once the chunk is assigned to one window controller (one for each interface), 
       *  it will be requested always from there! this was done in order to not modify the request 
       *  process by the receiver
       */
      virtual int request_chunk(unsigned int,double)=0;
	  
      
      /** @brief returns the number of interests this window controller can express
       *  number of "free slots" in the interest window
       */      
      virtual int free_slot_win()=0;

	  
      /** @brief returns the first of the chunks that are in the list m_reissuing_chunks
       *  in order to re-schedule an Interest expression (notice that this req. may be 
       *  filtered by the PIT)
       */    
      virtual std::pair<int,double> timed_out_chunk(){return make_pair(-1,0);}
      
      /** @brief remove the first packet from the ordered set of timed_out chunks
       */    
      virtual void pop_timed_out(){}
      
      
  };
  
    /** @brief a vector that contains multiple window controllers
    */   
  typedef std::vector<FlowController*>	window_controllers;
  
};

#endif
