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
#ifndef CBCBSIM_CBCBNODE_H
#define CBCBSIM_CBCBNODE_H

#include <cbnsim/Node.h>
#include <cbnsim/types.h>
#include <cbcbsim/cbcbsim_types.h>
#include <siena/forwarding.h>
#include <list>
#include "cbcbsim/BroadcastLayer.h"
#include "cbcbsim/ME.h"
#include "cbcbsim/ProtocolOptions.h"
#include "cbcbsim/RA.h"
#include "cbcbsim/SR.h"
#include "cbcbsim/UR.h"
#include "cbnsim/Crandom.h"
#include <cbnsim/MatchHandler.h>
#include <stdlib.h>
#include <iostream>
#include <cbcbsim/CCNxPackets.h>
#include <cbcbsim/OutputQueue.h>
#include <cbcbsim/CCNxNode.h>

#include <cbcbsim/LoadBalancing.h>

using namespace std;
using namespace ssim;
using namespace cbnsim;
using namespace cbcbsim;

namespace cbcbsim
{
  /** @brief Node class for CBCB protocol.
   */
  class CBCBNode : public Node
  {
    class sr_response
    {
    public:
      set<ProcessId> respondees;
      Predicate pred;
    };
    
    void process_me( unsigned int ifid, const ME& me );
    void process_ra( unsigned int ifid, const RA& ra );
    void process_sr( unsigned int ifid, const SR& sr );
    void process_ur( unsigned int ifid, const UR& ur );
    int process_interest( unsigned int ifid, const INTERESTPacket& interest );
    int process_data( unsigned int ifid, const DATAPacket& data );
    bool propagate_me( ProcessId issuer, unsigned int ifid, MessageId msg_id, const Message& msg );
    int propagate_ra( ProcessId issuer, ProcessId dest,unsigned int path_number, const Predicate& pred );
    unsigned int propagate_sr( ProcessId issuer, unsigned int number );
    void update_fwd_table();
    void set_pred_const_counter(); 

    const BroadcastLayer&				m_bl;
    siena::FwdTable					m_fwd_table;
    LoadBalancing*					m_load_balancer;
    bool						m_issue_pending;
    vector<unsigned int>				m_msg_sent_counts;
  public:
    const ProtocolOptions&				m_options;
  private:
    Time						m_avg_fwd_info_interval;
    
    bool						m_route_table_dirty;
    map<unsigned int,sr_response>			m_sr_responses;
    int							pred_const_counter;
    
    vector<Queue*>					m_output_queues;
    vector<Predicate>					m_route_table;    
    CCNxNode*						m_transport_node;

    
    /* define these parameters for raf retransmission
     */
    Message						raf_msg_want;
    ProcessId						raf_parent_want;
    ProcessId						raf_meIssuer_want;
    MessageId						raf_msgId_want;
    unsigned int					raf_retrans_count;
    unsigned int					ra_same_count;

    /* collector of different packets' and throughput statistic
     */
    
    unsigned int					m_ra_packets;
    unsigned int					m_me_packets;
    unsigned int					m_srur_packets;
    unsigned long long					m_interest_packets;
    unsigned long long					m_fwd_interest_packets;
    unsigned int					m_data_packets;
    vector<double>					m_link_rate;
    vector<double>					m_loss_rate;
    vector<double>					m_queues_lenght;
    
  public:
    Time*						time_block; 
    Time						last_time_block;  
    
    
    CBCBNode( unsigned int nid, const BroadcastLayer& bl, const ProtocolOptions& options, Time* time_block_collector, unsigned int cache_size, char cache_repl);
    
    virtual ProcessId activate();
    
    virtual void add_neighbor( ProcessId id, Time length , double buffer, double capacity);
    
    void setup_FIB_entry(unsigned int id,Predicate pred);

    
    virtual size_t bytesize() const;

    void init();

    void dequeue_packet(void* q);

    void receiver_TO(unsigned int ifid_sent, string socket_addr, unsigned int chunk_id, ssim::Time start_time);
    
    void receiver_CBR(unsigned int ifid_sent,string socket_addr,  ssim::Time start_time);
    
    virtual void issue_packet( unsigned int ifid, const Packet* packet, Time lag = 0, bool count = true );

    virtual void issue_packet( ProcessId pid, const Packet* packet, Time lag = 0 )
    { Node::issue_packet( pid, packet, lag ); }
    
    bool update_route_table(unsigned int, const Predicate, int);

    void issue_predicate( bool force = false, ProcessId dest = -1,unsigned int path_number=1);

    void issue_sr();

    void process_packet( unsigned int ifid, const Packet& pkt );

    void reset_ra_packets()
    { m_ra_packets = 0; }

    void reset_me_packets()
    { m_me_packets = 0; }

    void reset_srur_packets()
    { m_srur_packets = 0; }

    void reset_interest_packets()
    { m_interest_packets = 0; }
    
    void reset_fwd_interest_packets()
    { m_fwd_interest_packets = 0; }

    void reset_data_packets()
    { 
      m_data_packets = 0; 
      if (m_transport_node != NULL)
      { 
	m_transport_node->reset_data( ); 
      }
    }
    
    void reset_link_rate()
    { fill( m_link_rate.begin(), m_link_rate.end(), 0.0); }

    void reset_loss_rate()
    { fill( m_loss_rate.begin(), m_loss_rate.end(), 0.0); }

    void reset_miss_rate()
    { 
      if (m_transport_node != NULL)
        m_transport_node->reset_miss_rate();
    }
    
    void reset_hit_rate()
    { 
      if (m_transport_node != NULL)
        m_transport_node->reset_hit_rate();
    }
    
    void reset_filter_rate()
    { 
      if (m_transport_node != NULL)
        m_transport_node->reset_filter_rate();
    }
    

    virtual void send_message( MessageId mid, const Message& message );

    virtual void set_predicate( const Predicate& predicate, unsigned int, bool );
    
    virtual void set_multipath(unsigned int, const Predicate& predicate);

    void download_content(const Message& predicate);
 
    void publish_content(const Predicate& predicate, unsigned int, unsigned int, unsigned int, unsigned int, 
			 unsigned int);

    unsigned int ra_packets() const
    { return m_ra_packets; }

    unsigned int me_packets() const
    { return m_me_packets; }

    unsigned int srur_packets() const
    { return m_srur_packets; }

    unsigned long long interest_packets() const
    { 
      return m_interest_packets;    
    }

    unsigned long long fwd_interest_packets() const
    { 
      return m_fwd_interest_packets;
    }


    unsigned int data_packets() const
    { 
      unsigned int cached_dat = 0;
      if (m_transport_node != NULL)
      {
        cached_dat = m_transport_node->data( );
      }
      return m_data_packets+cached_dat;      
    }

    vector<double> link_rate() const
    { return m_link_rate; }

    unsigned int cache() const
    {  
      if (m_transport_node != NULL)
        return m_transport_node->cache_size( );
      else
        return 0;
    }

    vector<double> loss_rate() const
    { return m_loss_rate; }
    
    vector<unsigned long long> hit_rate() const
    { 
      vector<unsigned long long> empty;
	if (m_transport_node != NULL)
	    return m_transport_node->hit_rate();
	else
	    return empty;
    }
      
    vector<unsigned long long> miss_rate() const
    {   
      vector<unsigned long long> empty;
      if (m_transport_node != NULL)
	return m_transport_node->miss_rate();
      else
	return empty;
    }
    
    vector<unsigned long long> filter_rate() const
    { 
      
      vector<unsigned long long> empty;
      if (m_transport_node != NULL){
	return m_transport_node->filter_rate();
      }
      else return empty;
    }
        
    vector<double> queues_length() const
    {
      return m_queues_lenght;
    }
    
    prefix_table split_stats() const		
    { return m_load_balancer->m_ptable;}
    

    
    
    void update_fwd_info(Message m, unsigned int id, char op);
    
    void update_fwd_info(list<fwd_info>::iterator, unsigned int id, char op);
   
    void remove_old_fwd_info(list <fwd_info>::iterator list_it, unsigned int ifid);
    
    void rnd_forward(const INTERESTPacket& interest, set<siena::if_t>& matches);
    
    void weighted_rnd_forward(const INTERESTPacket& interest,set<siena::if_t>& matches);

    virtual bool has_failed() const { return false; };
    
    virtual bool count_power() const { return false; };

  };
};

#endif
