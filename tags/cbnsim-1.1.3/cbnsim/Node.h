// -*- C++ -*-
//
//  This file is part of CBNSIM, the Content-Based Networking
//  Simulation Library.
//
//  Authors: See the file AUTHORS for full details.
//
//  Copyright (C) 2002-2004 University of Colorado
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
//  USA, or send email to serl@cs.colorado.edu.
//
// $Id: Node.h,v 1.23 2005/10/03 21:27:17 hallcp Exp $
//
#ifndef CBNSIM_NODE_H
#define CBNSIM_NODE_H

#include <cassert>
#include <map>
#include <vector>

#include <siena/types.h>
#include <siena/ssim.h>

#include "cbnsim/NodeEvent.h"
#include "cbnsim/Packet.h"
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief Abstract class representing a node in the network.
   */
  class Node : public ssim::ProcessWithPId
  {
    
  private:
      static const unsigned long SEND_POWER_DRAIN = 20;  //nAh
      static const unsigned long RECV_POWER_DRAIN = 10;

      unsigned int				m_ctrl_packets;
      unsigned int				m_data_packets;
      std::vector<ssim::Time>			m_delay_table;
      unsigned int				m_id;
      

      std::map<ssim::ProcessId,unsigned int>	m_pid_map;
      unsigned int                              m_x, m_y;
      power_type                                m_power_left;

  protected:
    Node( unsigned int id, unsigned int x, unsigned int y)
      : m_id( id ), m_x(x), m_y(y) { reset_packet_counters(); }
  
  public:
    virtual ~Node() { }

    /** @brief Override <code>ssim::ProcessWithPId::activate</code> method
     * to initialize some local variables.
     */
    virtual ssim::ProcessId activate();

    /** @brief Method used to notify a Node about a neighboring process.
     */
    virtual void add_neighbor( ssim::ProcessId nid, ssim::Time length , double buffer, double capacity);
    
    virtual void setup_FIB_entry( unsigned int neigh, Predicate){};

    /** @brief Issues a packet to all neighbors (does not include
     * localhost).  The <code>count_all</code> parameter controls
     * whether the packets are counted individually for each neighbor
     * pair (<code>true</code>), or if broadcast counts as a single
     * packet that happens to go to all neighbors (<code>false</code>). Default
     * is <code>false</code>.
     */
    bool broadcast_packet( const Packet *packet, ssim::Time lag = 0, bool count_all = false );
      
    /** @brief Methods used to compute the amount of memory being
     * used by the simulation.
     */
    virtual size_t bytesize() const = 0;

    /** @brief Returns the number of control packets sent by this node
     * since the last time they were cleared.
     */
    unsigned int control_packets() const
    { return m_ctrl_packets; }

    /** @brief Returns the number of data packets sent by this node
     * since the last time they were cleared.
     */
    unsigned int data_packets() const
    { return m_data_packets; }

    /** @brief Returns the node's ID, as it is read from the topology
     * file.
     */
    unsigned int id() const
    { return m_id; }

    /** @brief Returns the <code>ssim::ProcessId</code> associated with
     * an interface id.
     */
    ssim::ProcessId i2p( unsigned int ifid ) const
    { assert( ifid < m_if_table.size() ); return m_if_table[ifid]; }

    /** @brief Method that issues a packet down the specified interface.
     * The <code>count</code> parameter controls whether the data/control counts
     * should be updated with this packet or not.
     */
    virtual void issue_packet( unsigned int ifid, const Packet* packet, ssim::Time lag = 0, bool count = true );
    
    virtual void issue_packet_buffer( unsigned int ifid, const Packet* packet, ssim::Time lag = 0, bool count = true, bool single_chunk=false, unsigned int data_size = 0, std::string = "" );


    /** @brief Method that issues a packet to the specified process.
     */
    virtual void issue_packet( ssim::ProcessId pid, const Packet* packet, ssim::Time lag = 0 );

    /** @brief Returns the number of neighbors.
     */
    unsigned int neighbor_count() const
    { return m_if_table.size() - 1; }

    /** @brief Method to handle <code>node_fail</code> workload
     * events.
     *
     * The default implementation is empty.
     */
    virtual void node_fail() {}
    virtual bool has_failed() const = 0;

      /** @brief Method to handle <code>node_recover</code> workload
     * events.
     *
     * The default implementation is empty.
     */
    virtual void node_recover() {}

    /** @brief Returns the interface id associated with a
     * particular process.
     */
    unsigned int p2i( ssim::ProcessId pid ) const;

    /** @brief Method that handles incoming events, converts
     * them to Packets and performs the lookup to determine
     * what interface they came in on.
     */
    void process_event( const ssim::Event* evt );

    /** @brief Method to handle incoming packets. The second argument
     * contains the interface that the packet came in on.  The third
     * argument tells the simulation if the packet is readable or not.
     *
     * The default implementation is empty.
     */
    virtual void process_packet( unsigned int ifid, const Packet& pkt ) = 0;


//     virtual void delete_packet(unsigned int ifid, unsigned int size, std::string address) {};
    
    virtual void dequeue_packet(void* q) {};
    
    /** @brief triggers a receiver time out
     */
    virtual void receiver_TO(unsigned int ifid_sent,std::string socket_addr, unsigned int chunk_id, ssim::Time start_time) {};

    virtual void receiver_CBR(unsigned int ifid_sent,std::string socket_addr, ssim::Time start_time) {};

    
    /** @brief Resets the packet counters to zero.
     */
    void reset_packet_counters()
    { m_ctrl_packets = m_data_packets = 0; }

    /** @brief Method that handles <code>send_message</code> workload
     * events. The <code>mid</code> argument is the unique message id
     * provided by the <code>DeliveryOracle</code>. This must be propagated
     * so that 
     *
     * Default implementation is empty.
     */
    virtual void send_message( MessageId mid, const Message& message ) {}

    /** @brief Method that handles <code>set_predicate</code> workload
     * events.
     *
     * Default implementation is empty.
     */
    virtual void set_predicate(const Predicate& predicate, unsigned int rate,
				 bool pred_null) {}

    /** @brief Method that handles <code>set_multipath</code> workload
     * events.
     *
     * Default implementation is empty.
     */
    virtual void set_multipath(unsigned int to, const Predicate& predicate) {}

    virtual void download_content(const Message& message) {}
    
    virtual void publish_content(const Predicate& predicate, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) {}

    virtual void set_power(power_type p) { m_power_left = p; }

    virtual bool count_power() const = 0;
    virtual power_type power_left() { return (m_power_left > 0 ? m_power_left : 0); }

      /** @brief Returns the x location in the simulated topology
       */
    unsigned int x_location() const { return m_x; }

      /** @brief Returns the y location in the simulated topology
       */
    unsigned int y_location() const { return m_y; }
     
    std::vector<ssim::ProcessId>		m_if_table;
   
  
#ifdef COLLISION_DETECTION_ON
      static CC2420RadioModel                   m_rm;
      static CollisionField                     m_collisions;
#endif

  };
  typedef std::vector<Node*>        NodeList;
};

#endif
