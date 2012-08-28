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

#include <cbcbsim/cbcbsim_types.h>
#include <cbnsim/types.h>
#include <siena/forwarding.h>
#include <list>
#include "cbcbsim/BroadcastLayer.h"
#include "cbcbsim/ProtocolOptions.h"
#include "cbcbsim/Socket.h"
#include "cbcbsim/SocketManager.h"
#include <cbnsim/WorkloadEventParser.h>
#include <cbnsim/Packet.h>
#include <cbcbsim/ServedContent.h>
#include <cbcbsim/CBCBNode.h>
#include <cbcbsim/PITable.h>
#include <stdlib.h>
#include <stdio.h>
#include <cbcbsim/Cache.h>
#include <cbcbsim/CacheLRU.h>
#include <cbcbsim/CacheRND.h>
#ifndef CBCBSIM_CCNXNODE_H
#define CBCBSIM_CCNXNODE_H
#include "CacheLRU.h"
#include "CBR.h"




namespace cbcbsim
{

  /** @brief Node class for CCNx Transport protocol.
   */
  class CCNxNode 
  {
    public:
    unsigned int 						NOpenConnections;

    protected:
    ssim::ProcessId	                			m_id; //phisical identifier of the node to witch this "Transport node" is related
    const ProtocolOptions&					m_options;

    served_contents						m_served_contents;
    
    /** @brief List of SocketManagers responsible to send interest packet and receive data packet 
     */
    socket_map							m_socket_manager; 
    /** @brief PIT table, will change; create a class
     */
    PITable*		 					m_PIT_table;    
    
    double 							m_cache_size; //current size of the cache
    unsigned int 						m_cache_limit; //maximum size of the cache
  
    Cache*							m_cache;
    
    unsigned int 						m_interest_count;
    unsigned int 						m_data_count;
    
    std::vector<unsigned long long>				m_miss;
    std::vector<unsigned long long>				m_hit;
    std::vector<unsigned long long>				m_filter;
    
    unsigned int 						NOpenConnections_single;
    char							m_replacement;
    
     
    public:
    CCNxNode(unsigned int id, const ProtocolOptions& options, char replacement='o', unsigned int cache_size = 0) : 
    m_id( id ), m_options( options ), m_cache_size(cache_size), m_replacement(replacement)
    { 
      //some statistics initializations
      m_hit.resize(m_options.classes,0);
      m_miss.resize(m_options.classes,0);
      m_filter.resize(m_options.classes,0);
      /** @brief init size of the PIT and served contents' hash tables
       */
      m_served_contents.rehash(1e+05);
      
      //declare LRU or RND cache according to options passed to the simulation
      if ( m_replacement == 'l') 
	m_cache = new CacheLRU(m_id,cache_size,m_options.m_minimum_chunk_size);
      else if ( m_replacement == 'r') 
	m_cache = new CacheRND(m_id, cache_size,m_options.m_minimum_chunk_size);
      m_PIT_table = new PITable(10000,m_options.to_router/m_options.sim_time_unit,m_options.m_filtering);
      NOpenConnections = 0;
      srand(0);
    }
    
    unsigned int opened_connections() const
    { return NOpenConnections; }

    void reset_interest( )
    { m_interest_count = 0; }
    
    void reset_data( )
    { m_data_count = 0; }
    
    void reset_miss_rate(){ std::fill(m_miss.begin(),m_miss.end(),0); }
    
    void reset_hit_rate(){ std::fill(m_hit.begin(),m_hit.end(),0); }
    
    void reset_filter_rate(){ std::fill(m_filter.begin(),m_filter.end(),0); }

    unsigned int id(){ return m_id; }
 
    unsigned int cache_size( ){ return m_cache_size; }
    
    unsigned int interest( ){ return m_interest_count; }
    
    unsigned int data( ){ return m_data_count; }
    
    void hit_rate_update( unsigned int popularity_class)
    {
      if (popularity_class < m_options.classes)
	m_hit[popularity_class-1]++;
    }
    
    void miss_rate_update( unsigned int popularity_class)
    { 
      if (popularity_class <= m_options.classes)
	m_miss[popularity_class-1]++;    
    }
    
    void filter_rate_update( unsigned int popularity_class)
    {  
      if (popularity_class <= m_options.classes)
	m_filter[popularity_class-1]++;
    }
    
    std::vector<unsigned long long> hit_rate() const { return m_hit; }
    
    std::vector<unsigned long long> miss_rate() const { return m_miss; }
    
    std::vector<unsigned long long> filter_rate() const { return m_filter; }
    
    unsigned int open_connection() const { return NOpenConnections; }

    /** @brief given a socket manager, the received data (or aux data in the case of a receiver time out) and the interface
     *  from which data was received (or receiver timeout is expired) computes the set of interests to be issued and 
     *  from which interface and
     *  records the interest in the PIT if needed
     */ 
    outgoing_packets set_outgoing_interests(Socket* socket, unsigned int ifid);
    
    /** @brief Record the served content from local node
     */
    void record_content( const cbnsim::Predicate&, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

    /** @brief Record the interest in the PIT if needed (check if it has to be filtered based on filtering option and type of node: RX or Router)
     */
    bool record_interest( unsigned int ifid, const cbcbsim::INTERESTPacket& interest, ssim::Time start_time, 
			  cbcbsim::Socket* socket);
    
    /** @brief return the registration time of an interest. If there are multiple entries it returns the oldest one
     */
    Time registration_time(unsigned int ifid,DATAPacket data); 
			  
			  
    /** @brief Manage arrived interest. 
     * 	       Figure out if the predicate in the arrived INTEREST is served by this node. returns the DATA packet 
     *	       to send through the requesting interface   
     */
    CCNxPacket* manage_interest( const INTERESTPacket& interest );

    
    /** @brief process the forwarding interfaces of a given data looking up the PITable
     */
    match process_out( unsigned int ifid, DATAPacket data );
    /** @brief Open a socket whenever a content download starts 
     * it returns the number of the socket port;
     * 2 socket for the same content has different port numbers
     */
    unsigned int open_socket( const INFOPacket&, const std::set<siena::if_t>&);
      
    /** @brief Once a data is received, gives it to upper levels and compute which is/are the next interest/s to be sent
     *  and from which interface
     */
    outgoing_packets manage_data(const DATAPacket&, unsigned int ifid);
    
    /** @brief manage a time out on the receiver, re-send if necessary the interest for which the T.O. is expired
     */
    outgoing_packets receiver_TO(unsigned int ifid_sent, std::string socket_addr, unsigned int chunk_id,ssim::Time start_time);
    
    outgoing_packets receiver_CBR(unsigned int ifid_sent, std::string socket_addr, ssim::Time start_time);
    
    /** @brief add a data-chunk in the cahe 
     */
    void add_in_cache( const DATAPacket& data ); 
   
    /** @brief search the chunk indicated by the interest in the cache
     */
    const DATAPacket* search_in_cache( const INTERESTPacket& interest );
    
    /** @brief Translate a Message as defined by Carzaniga in a CCNx address (URL like)
     *  the second parameter indicates the number of attributes to consider in the translation 
     *  (notice that attributes in a message are ordered according to their alphabetic order)
     */
    cname content_name( Message msg, unsigned int an);
    
    /** @brief Translate a Filter as defined by Carzaniga in an URL like in a CCNx prefix
     *  the second parameter indicates the number of attributes to consider in the translation 
     *  (notice that attributes in a message are ordered according to their alphabetic order)
     */ 
    cname content_name(const cbnsim::Filter, unsigned int an);

  };
};

#endif
