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
#ifndef CBCBSIM_PACKETS_H
#define CBCBSIM_PACKETS_H


#include <cbnsim/types.h>
#include <cbcbsim/cbcbsim_types.h>
#include <cbnsim/Packet.h>
#include <siena/ssimp_types.h>
#include <string>
#include <stdlib.h>

using namespace std;
using namespace ssim;
using namespace cbnsim;
using namespace cbcbsim;


namespace cbcbsim
{
  #define INTEREST_SIZE = 200
  
  class CCNxPacket : public Packet
  {
  public:
    /** @brief chunk id requested through this packet
     *  chunk_id = 0 means requesting INFO packet
     */
    unsigned int	        m_chunk_Id; 
    
    /** @brief content name of the chunk requested through this packet
     */
    cname          		m_content_name;
    
    /** @brief size [bit] of the packet
     */
    unsigned int		m_size;
    
    /** @brief cbcbsim like message, representing the content name
     */
    Message			m_msg;
    
    /** @brief port associated to this interest 
     * 2 request precesses on the same node, are identified through the port number
     */
    int 			m_port;
    
    /** @brief popularity class of the requested content 
     * used to keep stats. 
     */
    unsigned int 		m_popularity_class;
    
    /** @brief tot. # of packets(chunks) constituting the content 
     */
    unsigned int 		m_content_size;
    
    string			m_source;
    
     
    
    CCNxPacket(ProcessId sender, cname content_name, unsigned int chunk_Id, unsigned int size,Message msg, unsigned int port, 
	       unsigned int popularity_class, unsigned int content_size) :Packet( sender, true),
    m_chunk_Id( chunk_Id ), m_content_name( content_name ), m_size( size ), m_msg( msg ), 
    m_port( port ), m_popularity_class( popularity_class ), m_content_size( content_size ), m_source("") {};
    
    CCNxPacket(ProcessId sender, cname content_name, unsigned int chunk_Id, unsigned int size,Message msg, unsigned int port, 
	       unsigned int popularity_class, unsigned int content_size, string source) :Packet( sender, true),
    m_chunk_Id( chunk_Id ), m_content_name( content_name ), m_size( size ), m_msg( msg ), 
    m_port( port ), m_popularity_class( popularity_class ), m_content_size( content_size ), m_source(source) {};
    
    
    
    unsigned int size() const
    {return m_size; }

    unsigned int chunk_id() const
    { return m_chunk_Id; }
   
    cname content_name() const
    { return m_content_name; }
    
    Message msg() const
    { return m_msg; }
    
    unsigned int port() const
    { return m_port; }
    
    unsigned int popularity_class() const
    { return m_popularity_class; }
    
    unsigned int content_size() const
    { return m_content_size; }
    
    string source() const
    { return m_source; }
    
    void set_source(string p)
    { m_source = p;}
    
  };
  
  class INTEREST : public CCNxPacket
  { 
    /** @brief time at which interest was issued
     */    
    Time			m_issue_time;
   
    /** @brief size [bit] of the content's data packets 
     */
    unsigned int 		m_data_size;


  public:
    INTEREST( ProcessId sender, cname content_name, unsigned int chunk_Id, Time issue_time, 
	      unsigned int retransmission, Message msg, unsigned int data_size, unsigned int port, 
	      unsigned int popularity_class, unsigned int content_size):
	      CCNxPacket( sender, content_name, chunk_Id, (25*8), msg, port, popularity_class, content_size ), m_issue_time( issue_time ), 
    m_data_size( data_size ) {}

    INTEREST( ProcessId sender, const INTEREST& interest):
    CCNxPacket( sender, interest.content_name(), interest.chunk_id(), 25*8, interest.msg(), interest.port(), 
		 interest.popularity_class(), interest.content_size() ),
    m_issue_time( interest.issue_time() ), m_data_size( interest.data_size() ){}
  
    unsigned int data_size() const
    {return m_data_size; } 

    Time issue_time() const
    { return m_issue_time; }
  };
 
  class DATA : public CCNxPacket
  {
  protected:
    /** @brief time at which interest that requested this DATA was issued
     */    
    Time			m_interest_i_time;
    
    
  public:
    DATA( ProcessId sender, cname content_name, unsigned int chunk_Id, Message msg, unsigned int size, 
	  Time interest_i_time, unsigned int port, unsigned int content_size, unsigned int popularity_class)
      :CCNxPacket( sender, content_name, chunk_Id, size ,msg, port, popularity_class, content_size ), 
      m_interest_i_time( interest_i_time ) {}
      
    DATA(ProcessId sender, const DATA& data, unsigned int port)
      :CCNxPacket( sender, data.content_name(), data.chunk_id(), data.size(), data.msg(), port , data.popularity_class(), 
    		   data.content_size() ), m_interest_i_time( data.interest_issue_time() ) {}
    		   
    DATA(ProcessId sender, const DATA& data, unsigned int port, string source)
      :CCNxPacket( sender, data.content_name(), data.chunk_id(), data.size(), data.msg(), port , data.popularity_class(), 
    		   data.content_size(), source ), m_interest_i_time( data.interest_issue_time() ){}
      
      
    DATA(ProcessId sender, const INTEREST& interest)
      :CCNxPacket( sender, interest.content_name(), interest.chunk_id(), interest.data_size(), interest.msg(), interest.port(),
		   interest.popularity_class(), interest.content_size() ) , m_interest_i_time( interest.issue_time() ){}
		   		   
    Time interest_issue_time() const
    { return m_interest_i_time; }

    
  };
    
  class INFO : public DATA
  {
    unsigned int 			   m_receiver_win;
    unsigned int			   m_data_chunk_size;
    unsigned int 			   m_fec_perc;


  public:
    INFO( ProcessId sender, cname content_name, unsigned int chunk_Id, Message msg, cbnsim::Predicate pred, 
	  unsigned int content_size, unsigned int data_chunk_size, Time interest_i_time, unsigned int receiver_win, 
	  unsigned int fec_perc, unsigned int popularity_class,int port):
          DATA(sender, content_name,  chunk_Id, msg, 0 , interest_i_time ,port , content_size, popularity_class), 
          m_receiver_win( receiver_win ), m_data_chunk_size(data_chunk_size), m_fec_perc( fec_perc ) {}
          
    INFO( ProcessId sender, const INFO& info, unsigned int port=0):
          DATA(sender, info.content_name(), info.chunk_id(), info.msg(), 0 ,info.interest_issue_time() ,port, 
	       info.content_size(),info.popularity_class()), 
          m_receiver_win( info.receiver_win() ), m_data_chunk_size(info.data_chunk_size()), m_fec_perc( info.fec_perc() ){}
          
    INFO( ProcessId sender, cname content_name, Message msg, Time interest_i_time, ServedContent served):
          DATA(sender, content_name, 0, msg, 0 ,interest_i_time ,0, served.file_size(),served.popularity_class()), 
          m_receiver_win( served.receiver_win() ), m_data_chunk_size(served.chunk_size()), m_fec_perc( served.fec_perc() ){}
          
   
    unsigned int receiver_win() const
    {return m_receiver_win; }
    
    unsigned int fec_perc() const
    {return m_fec_perc; }
    
    unsigned int data_chunk_size() const
    {return m_data_chunk_size; }
    
  };
  
  typedef INTEREST INTERESTPacket;
  
  typedef DATA DATAPacket;

  typedef INFO INFOPacket;
  
};


#endif
