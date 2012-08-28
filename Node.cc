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
// $Id: Node.cc,v 1.19 2005/08/11 15:11:34 hallcp Exp $
//
#include "cbnsim/Node.h"
#include "cbnsim/NodeFailPacket.h"
#include "cbnsim/NodeRecoverPacket.h"
#include "cbnsim/DeQueuePacket.h"
#include "cbnsim/ReceiverCBR.h"
#include "cbnsim/ReceiverTO.h"
#include "cbnsim/SendMessagePacket.h"
#include "cbnsim/SetPredicatePacket.h"
#include "cbnsim/SetMultipathPacket.h"
#include "cbnsim/DownloadContentPacket.h"
#include "cbnsim/PublishContentPacket.h"
#include "cbnsim/logging.h"
#include "stdlib.h"

using namespace cbnsim;
using namespace ssim;
using namespace std;

ProcessId Node :: activate()
{
  CBNASSERT( m_if_table.empty(),
	     "node " << id() << " interface table not empty upon activation" );
  CBNASSERT( m_delay_table.empty(),
	     "node " << id() << " delay table not empty upon activation" );
  CBNASSERT( m_pid_map.empty(),
	     "node " << id() << " pid map not empty upon activation" );

  ProcessId p = ProcessWithPId::activate();

  m_if_table.push_back( p );
  m_pid_map.insert( make_pair( p, 0 ) );
  m_delay_table.push_back( 0 );

  return p;
}

void Node :: add_neighbor( ProcessId nid, Time length , double buffer, double capacity)
{
  CBNDEBUG( "add_neighbor(id=" << id() << ",pid=" << pid() << ") = " << nid << " with length " << length );

  m_if_table.push_back( nid );
  m_pid_map.insert( make_pair( nid, (m_if_table.size() - 1) ) );
  m_delay_table.push_back( length );
}

bool Node::broadcast_packet( const Packet *packet, Time lag, bool count_all )
{
    unsigned int size = m_delay_table.size();
    // don't want to send to zero since that is the localhost
    for( unsigned int i = 1; i < size; ++i ) {
      issue_packet( i, packet, lag, count_all || i == 1 );
    }

    m_power_left -= SEND_POWER_DRAIN;
    if(m_power_left < 0) {
	node_fail();
	return false;
    }

    return true;
}

void Node :: issue_packet( unsigned int ifid, const Packet* packet, Time lag, bool count )
{
  
  CBNASSERT( ifid < m_delay_table.size(),
	     "interface id (" << ifid << ") out of range for node: " << id() );
  if( count )
    {
      if( packet->is_control() )
	{
	  m_ctrl_packets++;
	}
      else
	{
	  m_data_packets++;
	}
    }
  //cout << "Issuing packet to " <<  m_if_table[ifid] << " using " <<  ifid << " at " << m_delay_table[ifid] << endl;	
  Sim::signal_event( m_if_table[ifid], packet, (m_delay_table[ifid] + lag) );
}

void Node :: issue_packet_buffer( unsigned int ifid, const Packet* packet, Time lag, bool count, bool single_chunk , unsigned int data_size, std::string address)
{
  CBNASSERT( ifid < m_delay_table.size(),
	     "interface id (" << ifid << ") out of range for node: " << id() );
  if( count )
    {
      if( packet->is_control() )
	{
	  m_ctrl_packets++;
	}
      else
	{
	  m_data_packets++;
	}
    }
  //cout << "deleting this packet at buffer " << m_delay_table[id()] << endl;
//  cout << "packet size "<< packet->size() << endl;
  if (single_chunk)
  {
    Sim::signal_event( m_if_table[ifid], packet, (m_delay_table[ifid]*data_size/80000) );
  }
  else
  {
    Sim::signal_event( m_if_table[ifid], packet, (m_delay_table[ifid] + lag) );
/*    Sim::signal_event( id(), new NodeDeletePacket(packet->size(), ifid, address), ( lag));     */
  }
}


void Node :: issue_packet( ProcessId pid, const Packet* packet, Time lag )
{
  CBNASSERT( m_pid_map.find( pid ) != m_pid_map.end(),
	     "node " << id() << " attempting to issue packet to non-neighboring process: " << pid );
  //cout << "Issuing packet to " << pid << " using " <<  m_pid_map[pid]<< " at " << lag << endl;	     
  issue_packet( m_pid_map[pid], packet, lag );
}

void Node :: process_event( const Event* evt )
{
  const NodeFailPacket* nfp;
  const NodeRecoverPacket* nrp;
  const SendMessagePacket* smp;
  const SetPredicatePacket* spp;
/*  const NodeDeletePacket* ndp;*/
  const DeQueuePacket* dqp;
  const DownloadContentPacket* dcp;
  const SetMultipathPacket* smpk;
  const PublishContentPacket* pcp;
  const ReceiverTO* rto;
  const ReceiverCBR* rcbr;

  
/*  cout << "Node processing event address:" << endl;*/
  // handle the "special" events coming from the driver
  if( (nfp = dynamic_cast<const NodeFailPacket*>( evt )) )
    {
      node_fail();
    }
  else if( (nrp = dynamic_cast<const NodeRecoverPacket*>( evt )) )
  {
      node_recover();
  }
  else if( (smp = dynamic_cast<const SendMessagePacket*>( evt )) )
    {
	if(m_power_left >= 0)
	    send_message( smp->mid(), smp->message() );
    }
  else if( (spp = dynamic_cast<const SetPredicatePacket*>( evt )) )
    {
	if(m_power_left >= 0)
	    set_predicate( spp->predicate(), spp->msg_rate(), spp->is_predicate_null());
    }
  else if( (dqp = dynamic_cast<const DeQueuePacket*>( evt )) )
    {
      dequeue_packet(dqp->get_queue());
    }
  else if( (rto = dynamic_cast<const ReceiverTO*>( evt )) )
    {
	 receiver_TO(rto->ifid_sent(), rto->socket_address(), rto->chunk_id(), rto->start_time());
    }
  else if( (rcbr = dynamic_cast<const ReceiverCBR*>( evt )) )
    {
	 receiver_CBR(rcbr->ifid_sent(), rcbr->socket_address(), rcbr->start_time());
    }
  else if( (dcp = dynamic_cast<const DownloadContentPacket*>( evt )) )
    {
        download_content( dcp->msg() );
    }
  else if( (pcp = dynamic_cast<const PublishContentPacket*>( evt )) )
    {
        publish_content( pcp->predicate(), pcp->size(), pcp->bytesize(), pcp->receiver_win(), pcp->fec_perc(), pcp->popularity_class());
    } 
  else if( (smpk = dynamic_cast<const SetMultipathPacket*>( evt )) )
    {
// - but no "packet" is recved here, this is a enternal notification
//	m_power_left -= RECV_POWER_DRAIN;
	set_multipath( smpk->to() ,smpk->predicate());
    }
  else
    {
      const Packet* pkt = dynamic_cast<const Packet*>( evt );
      
      CBNASSERT( pkt, "Node::process_event received non-Packet event" );
      CBNASSERT( m_pid_map.find( pkt->sender() ) != m_pid_map.end(),
		 "node " << id()
		 << " received packet from non-neigboring process: "
		 << pkt->sender() );
      process_packet( m_pid_map[pkt->sender()], *pkt );
      
    }

  if(m_power_left < 0) {
      node_fail();
  }
}

unsigned int Node :: p2i( ProcessId pid ) const
{
  map<ProcessId,unsigned int>::const_iterator itr = m_pid_map.find( pid );
  CBNASSERT( itr != m_pid_map.end(),
	     "node " << id() << " requesting interface for non-neighboring process: " << pid );
  return (*itr).second;
}

