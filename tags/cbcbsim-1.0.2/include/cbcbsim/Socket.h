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
#include <cbcbsim/cbcbsim_types.h>
#include <cbcbsim/CCNxPackets.h>
#include <list>
#include "cbcbsim/ProtocolOptions.h"



#ifndef CBCBSIM_SOCKET_H
#define CBCBSIM_SOCKET_H


namespace cbcbsim
{
  typedef struct chunk_info
  {
    /** @brief indicates if the chunk has been received or not
     */
    bool 		received;
    /** @brief auxiliary value used to compute the RTT of the chunk
     *  whenever an interest for a chunk is issued, the clock time is saved here,
     *  when the data for that interest is received, the delay of that chunk is computed
     */    
    double		chunk_RTT;
    /** @brief auxiliary value used to compute the delay of the chunk

     *  whenever an interest for a chunk is issued FOR THE FIRST TIME, the clock time is saved here,
     *  when the data for that interest is received, the delay of that chunk is computed
     */      
    double		chunk_retrieving_time;     
    /** @brief keeps track of the number of times this chunk has been requested
     *  notice that a chunk is requested if after the PIT lookup the interest is forwarded
     */
    int			chunk_request;
    /** @brief keeps track of the number of times this a timer for this chunk has been expired
     */
    int			chunk_timeout;
  } chunk_info;
  
  /** @brief Node class for socket that control many receiver socket and dispatch chunk requests between them.
   */ 
  class Socket
  {
    protected:  
      /** @brief simulation options
       */
      const ProtocolOptions&			m_options;
      
      /** @brief id of the node from which this socket manager belongs to
       */
      unsigned int				m_id;
      
      /** @brief content name of the group of receiver sockets 
       *  (each receiver socket, manage an output interface with a separate window)
       */
      cname					m_content_name;
	
      /** @brief message that represents the content name written in Content Based Networking's semantic 
       *  (each receiver socket, manage an output interface with a separate window)
       */
      Message					m_msg;
      
      /** @brief number of chunks that constitute the file 
       */
      unsigned int				m_tot_filesize;
      
      /** @brief size of a single chunk 
       *  (for the moment we assume constant chunk size within a single content)
       */
      unsigned int				m_chunk_size;
	
      /** @brief time at which the socket was started
       *  (INTEREST, INFO message exchanged)
       */
      ssim::Time				m_start_time;
      
      /** @brief port of the socket
       *  (two socket on the same content name, are identified through the port)
       */
      unsigned int				m_port;      
	
      /** @brief average of the RTT experienced by chunks 
       *  (RTT is the time elapsed between the first request and the data reception) 
       */
      double					m_VRTT;
	
      /** @brief cumulative sum of the delay experienced by chunks 
       *  (delay is the time elapsed between the last request and the data reception) 
       */
      double					m_delay;
	    
      /** @brief current filesize in bit
       */
      unsigned int 				m_filesize;
      
      /** @brief popularity class of this socket 
       */
      unsigned int				m_popularity_class;
	  
      /** @brief number of received chunks
       */
      unsigned int				received; 
	
      /** @brief vectors that contains info on the chunk (received or not, ecc.. see chunk_info struct)
       */
      std::vector<chunk_info> 			m_file_chunks;
	
      /** @brief ordered set that list all the chunks that has never been requested
       *  we need that structure to be a set (hence, ordered) because for the moment, the next 
       *  requested chunk is the one with lower chunk id
       */
      std::set<int>				m_notreq_chunks;
   
      /** @brief ordered set that list all the chunks that has not been received
       *  used to keep track of the last chunk received in sequence
       */
      std::set<int>				m_missing_chunks;
      
      
    public:
      Socket(const ProtocolOptions& options, unsigned int id, const INFOPacket &info, 
	     unsigned int port, const std::set<siena::if_t>& matches); 

      /** @brief returns the node id 
       */
      unsigned int nid()  {return m_id;}
      
      /** @brief returns the content name of this socket 
       */
      std::string content_name() const { return m_content_name; }
      
      /** @brief returns the msg associated to the content name of this socket 
       */
      Message msg() const { return m_msg;}
      
      /** @brief returns the time at which the socket has been opened
       */
      ssim::Time start_time() const { return m_start_time;}
      
      /** @brief returns the port associated to this content download
       */   
      unsigned int port() const { return m_port;}

      /** @brief returns the popularity index associated to the content managed by this socket
       */   
      unsigned int popularity_class() const { return m_popularity_class;}

      /** @brief returns the Virtual RTT of this content download
       *  VRTT: average time elapsed between the first request and the data reception
       */   
      double VRTT() const { return m_VRTT/received;}
      
      /** @brief returns the average chunk delay associated with this download
       *  delay: time elapsed between the last request and the data reception) 
       */       
      double delay() const { return m_delay/received;}
      
      /** @brief returns the current file size 
       *  (#received chunks * chunk_size)
       */    
      unsigned int filesize() const{ return m_filesize;}
	  
      /** @brief returns the number of chunks that constitutes the file  
       */ 
      unsigned int tot_filesize() const{return m_tot_filesize;}
      
      /** @brief returns the total chunk size of the content that is downloaded using this socket
       */ 
      double chunk_size() const {return m_chunk_size;}
      
      /** @brief updates the VRTT statistic
       */ 
      void VRTT_update(double chunk_RTT){m_VRTT += chunk_RTT;}
      
      /** @brief updates the delay statistic
       */
      void delay_update (double chunk_delay){m_delay += chunk_delay;}
      
      /** @brief updates the current filezize
       */    
      void size_update(unsigned int size){m_filesize+=size;}

      /** @brief checks if a chunk id has been already received or not
       */  
      bool check_received(unsigned int chunk_id) {return m_file_chunks[chunk_id].received;}

      /** @brief returns a chunk that has not been yet requested (for the first time) and received
       *  NOTICE that once the chunk is assigned to one interface, it will be requested always from there!
       *  this was done in order to not modify the request process by the receiver
       */
      int select_chunk();

      /** @brief keeps track of the forwarded chunk request (after the PIT lookup)
        */
      void requested_chunk(unsigned int chunk_id);
      
      /** @brief removes the chunk from the missing vector
        */
      void remove_notreq(unsigned int chunk_id);

      /** @brief manages the received chunk passing it to upper layers
       */ 
      virtual bool received_chunk( int received_chunk , unsigned int size, unsigned int ifid, string source)=0;
      
      /** @brief computes the chunk id that can be requested and from which interface.
       */ 
      virtual std::list< requesting_chunk >  get_interested_chunk()=0;

  };
  
  /** @brief socket unordered map,
   *  first:  content name/port:port_id 
   *  second: socket manager (a socket manager instance, manages multiple windows, each one on a different matching interface)
   */ 
  typedef boost::unordered_map<std::string, Socket*> socket_map;
};

#endif
