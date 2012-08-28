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


#ifndef CBCBSIM_PITable_H
#define CBCBSIM_PITable_H

#include <string>
#include <stdlib.h>
#include <list>
#include <siena/types.h>
#include <siena/ssim.h>
#include <siena/ssimp_types.h>
#include <cbcbsim/cbcbsim_types.h>
#include "CCNxPackets.h"
#include <cbcbsim/Socket.h>
#include <cbcbsim/SocketManager.h>



using namespace std;
using namespace ssim;


namespace cbcbsim
{
  class PIT_entry;
  
  /** @brief PIT,a map ordered according the content+chunk name
   *  first:  conten/chunk name
   *  second: PIT record
   */   
  typedef boost::unordered_multimap<string, PIT_entry> store_by_name_PIT;

  /** @brief PIT,a map ordered according the timer expiration
   * first:  Time at which the PIT entry will become no more valid
   * second: iterator to the store_by_name entry in order to easily remove the entry from both tables
   */   
  typedef std::multimap<ssim::Time, store_by_name_PIT::iterator> store_by_time_PIT;  
  
  class PIT_entry 
  {
    unsigned int						m_ifid;
    Time							m_time;
    unsigned int 						m_chunk_id;
    Time	 						m_interest_issue_time; 
    Message							m_msg;
    unsigned int						m_data_size; 
    int 							m_port;
    cname							m_content_name;
    store_by_time_PIT::iterator					m_store_by_time_it;
  
  public:
    PIT_entry();
    
    PIT_entry( unsigned int ifid, Time time, unsigned int chunk_id,  Time interest_issue_time, Message msg, 
	       unsigned int data_size, unsigned int port, string content_name,store_by_time_PIT::iterator store_by_time_it )
    : m_ifid( ifid ), m_time( time ), m_chunk_id ( chunk_id ), m_interest_issue_time( interest_issue_time ), 
    m_msg( msg ), m_data_size( data_size ), m_port( port ), m_content_name( content_name ),
    m_store_by_time_it(store_by_time_it){}
    
    PIT_entry( unsigned int ifid, Time time, INTERESTPacket interest,store_by_time_PIT::iterator store_by_time_it )
    : m_ifid( ifid ), m_time( time ), m_chunk_id ( interest.chunk_id() ), m_interest_issue_time( interest.issue_time() ), 
    m_msg( interest.msg() ), m_data_size( interest.data_size() ), m_port( interest.port() ), 
    m_content_name( interest.content_name() ),m_store_by_time_it(store_by_time_it) {}
    

    void set_store_by_time_it(store_by_time_PIT::iterator it) { m_store_by_time_it = it;}

    unsigned int ifid() const {return m_ifid; } 
    
    unsigned int chunk_id() const {return m_chunk_id; } 
     
    Message msg() const { return m_msg; }
    
    Time time() const {return m_time; }
    
    Time interest_issue_time() const {return m_interest_issue_time; }
    
    unsigned int data_size() const {return m_data_size; }
        
    unsigned int port() const {return m_port; }
    
    cname content_name() const {return m_content_name; }

    store_by_time_PIT::iterator store_by_time_it() { return m_store_by_time_it;}


  };
  
  class PITable
  {
    store_by_name_PIT 		m_store_by_name_PIT;   
    store_by_time_PIT 		m_store_by_time_PIT;    
    
    ssim::Time			m_timer;   
    ProtocolOptions::Filtering  m_filtering;
    
  
  public:
    PITable(unsigned int size, ssim::Time timer, ProtocolOptions::Filtering filtering ) :  m_timer(timer), m_filtering(filtering)
    { m_store_by_name_PIT.rehash(size);}
    
    /** @brief returns a couple of iterators that indicates the first and last PIT entry that match 
     *  the content_name+chunk_id
     */
    pair<store_by_name_PIT::iterator, store_by_name_PIT::iterator > lookup(cname content_name, unsigned int chunk_id);
    
    /** @brief process the forwarding interfaces of a given data looking up the PITable
     */
    match process_out(unsigned int ifid, DATAPacket data);
  
    /** @brief records, if needed, the interest in the PITable
     */
    bool record_interest(unsigned int ifid, INTERESTPacket interest, ssim::Time, Socket* socket );

    /** @brief remove expired entries from the Pending Interest Table 
     */
    int PIT_clean();
    
    /** @brief removes the PIT entry and add an entry to a structure used to collect interfaces that were interested 
     *  in receiving that DATA
     */
    void consume_PIT_entry(store_by_name_PIT::iterator it, match& match_out);
    
    /** @brief return the registration time of an interest. If there are multiple entries it returns the oldest one
     */
    Time registration_time(unsigned int ifid,DATAPacket data); 

  };
};
#endif
