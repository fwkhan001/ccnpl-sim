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
#include <cbcbsim/cbcbsim_types.h>
#include <cbcbsim/CCNxPackets.h>
#include <list>
#include "cbcbsim/ProtocolOptions.h"
#include "cbcbsim/RAQM.h"
#include "cbcbsim/RAQMsource.h"
#include "cbcbsim/ICP.h"
#include "cbcbsim/CBR.h"
#include "cbcbsim/Socket.h"


#ifndef CBCBSIM_SOCKETMANAGER_H
#define CBCBSIM_SOCKETMANAGER_H


namespace cbcbsim
{
  
  
  /** @brief Node class for socket that control many receiver socket and dispatch chunk requests between them.
   */ 
  class SocketManager : public Socket
  {
    
    private:      
      /** @brief unordered map (hash table) that list all the windows that are currently used by this socket
       *  notice that there is one window for each matching output interface connected at the receiver
       */

      
      bool					m_aborted;
      int					m_lower_rx_chunk;
      ProtocolOptions::FlowControllerType	m_fc_type;

    public:
      SocketManager(const ProtocolOptions& options, unsigned int id, const INFOPacket &info, 
		    unsigned int port, const std::set<siena::if_t>& matches);
      

      ProtocolOptions::FlowControllerType FlowControllerType(){return m_fc_type;}
      
      window_controllers			m_window_controllers;
      
      /** @brief returns the average window that has been used until now
       * 	it is an average of the average windows used in all the different interfaces
       */    
      double avg_win();

      /** @brief deletes the receivers used to download the file from different matching interfaces at the end of 
       *  the downloading process
       */ 
      void delete_receiver();
      
      /** @brief manages the received chunk passing it to the correct receiver window, 
       *  according to the interface from which the interest has been received
       *  returns true if the download process is finished
       */ 
      bool received_chunk( int received_chunk , unsigned int size, unsigned int ifid, string source);
      
      /** @brief computes the chunk id that can be requested and from which interface.
       */ 
      std::list< requesting_chunk >  get_interested_chunk();
      
      /** @brief manages a receiver time out 
       */
      void receiver_TO(unsigned int ifid, unsigned int chunk_id);       
      
      /** @brief CHECK if the file download has been aborted due to many successive requests for the same chunk
       */
      bool aborted(){return m_aborted;}
      
      /** @brief CHECK if the file has been completely downloaded
       */
      bool completed(){ return (m_filesize == (m_chunk_size * m_tot_filesize)); }
      
      std::map<string,int> get_paths_stat();
  };
};

#endif
