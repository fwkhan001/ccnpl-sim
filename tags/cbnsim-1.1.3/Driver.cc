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
// $Id: Driver.cc,v 1.10 2004/06/11 23:57:36 hallcp Exp $
//
#include <cstdio>
#include <stdexcept>
#include "cbnsim/DeliveryOracle.h"
#include "cbnsim/Driver.h"
#include "cbnsim/NodeEvent.h"
#include "cbnsim/NodeFailEvent.h"
#include "cbnsim/NodeFailPacket.h"
#include "cbnsim/NodeRecoverEvent.h"
#include "cbnsim/NodeRecoverPacket.h"
#include "cbnsim/SendMessageEvent.h"
#include "cbnsim/SendMessagePacket.h"
#include "cbnsim/SetPredicateEvent.h"
#include "cbnsim/SetPredicatePacket.h"
#include "cbnsim/DownloadContentEvent.h"
#include "cbnsim/DownloadContentPacket.h"
#include "cbnsim/PublishContentEvent.h"
#include "cbnsim/PublishContentPacket.h"
#include "cbnsim/SetMultipathEvent.h"
#include "cbnsim/SetMultipathPacket.h"

#include "cbnsim/logging.h"

using namespace cbnsim;
using namespace siena;
using namespace std;
using namespace ssim;

void Driver :: init()
{
  if( (m_next_event = m_parser->next_event()) )
    {
      Sim::self_signal_event( NULL, m_next_event->timestep() );
    }
}

void Driver :: process_event( const ssim::Event* evt )
{
  CBNASSERT( evt == NULL, "Driver can only process timeouts" );
  do
    {
      if( m_next_event->node_event() )
	{
	  const NodeEvent* ne = static_cast<const NodeEvent*>( m_next_event );
	  
	  CBNASSERT( ne->node_id() < m_nodes->size(),
		     "workload node id out of expected range: " << ne->node_id() );
	  Node* p_node = (*m_nodes)[ne->node_id()];

	  /*
	   * special handling for workload events that we know about
	   */
	  if( ne->event_name() == "node_fail" )
	    {
	      Sim::signal_event( p_node->pid(), new NodeFailPacket() );
	    }
	  else if( ne->event_name() == "node_recover" )
	  {
	      Sim::signal_event( p_node->pid(), new NodeRecoverPacket() );
	  }
	  else if( ne->event_name() == "send_message" )
	  {
	      const SendMessageEvent* sme = static_cast<const SendMessageEvent*>( ne );

	      //hackish here
	      MessageId mid = 0;
	      if(!p_node->has_failed())
		  mid = DeliveryOracle::send_message( p_node->pid(), 
							     sme->message() );
	      Sim::signal_event( p_node->pid(), new SendMessagePacket( mid, 
							    sme->message() ) );
	    }
	  else if( ne->event_name() == "set_predicate" )
	    {
	      const SetPredicateEvent* spe = static_cast<const SetPredicateEvent*>( ne );
	      if(!p_node->has_failed())
		  DeliveryOracle::set_predicate( p_node->pid(), 
						 spe->predicate() );
	      Sim::signal_event( p_node->pid(), 
				 new SetPredicatePacket( spe->predicate(),
							 spe->msg_rate() ) );
	    }
          else if( ne->event_name() == "download_content" )
	    {
	      const DownloadContentEvent* dce = static_cast<const DownloadContentEvent*>( ne );
	      //if(!p_node->has_failed())
		//  DeliveryOracle::send_interest( p_node->pid(), 
		//				 spe->predicate() );
	      Sim::signal_event( p_node->pid(), 
				 new DownloadContentPacket( dce->msg(),
							 dce->msg_rate() ) );
	    }
          else if( ne->event_name() == "publish_content" )
	    {
	      const PublishContentEvent* pce = static_cast<const PublishContentEvent*>( ne );
	      //if(!p_node->has_failed())
		//  DeliveryOracle::send_interest( p_node->pid(), 
		//				 spe->predicate() );
	      Sim::signal_event( p_node->pid(), 
				 new PublishContentPacket( pce->predicate(),pce->size(),pce->bytesize() ,pce->receiver_win(),pce->fec_perc(), pce->popularity_class() ) );
	    }
	  else if( ne->event_name() == "set_multipath" )
	    {
	      const SetMultipathEvent* sme = static_cast<const SetMultipathEvent*>( ne );
// 	      if(!p_node->has_failed())
// 		  DeliveryOracle::set_multipath( p_node->pid(), 
// 						 sme->predicate() );
	      Sim::signal_event( p_node->pid(), 
				 new SetMultipathPacket( sme->to(),sme->predicate()) );
	    }
	  else
	    {
	      handle_event( *p_node, *ne );
	    }
	}
      else
	{
	  // haven't dealt with link events yet.
	  CBNASSERT( false,
		     "link events not yet enabled" );
	}
    }
  while( (m_next_event = m_parser->next_event()) && m_next_event->timestep() == Sim::clock() );
  
  if( m_next_event )
    {
      // have to schedule the next timeout
      Sim::self_signal_event( NULL, m_next_event->timestep() - Sim::clock() );
    }
}
