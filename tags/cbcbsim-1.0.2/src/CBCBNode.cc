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
#include <cbnsim/DeliveryOracle.h>
#include <cbnsim/logging.h>
#include <iostream>
#include <set>
#include <siena/ssim.h>
#include "cbcbsim/CBCBNode.h"
#include "cbcbsim/ME.h"
#include "cbcbsim/PredicateIssueTimer.h"
#include "cbcbsim/PredicateRefreshTimer.h"
#include "cbcbsim/RA.h"
#include "cbcbsim/SR.h"
#include "cbcbsim/SRSendTimer.h"
#include "cbcbsim/UR.h"
#include <stdlib.h>
#include <iomanip>
#include <sstream>


using namespace cbcbsim;
using namespace cbnsim;
using namespace std;

#define NDEBUG(m) CBNDEBUG( "CBCBNode[" << pid() << "@" << Sim::clock() << "]: " << m )


CBCBNode :: CBCBNode( unsigned int nid, const BroadcastLayer& bl, const ProtocolOptions& options, 
		      ssim::Time* time_block_collector, unsigned int cache_size,char cache_repl)
    : Node( nid, 0, 0 ), m_bl( bl ), m_issue_pending( false ), m_options( options ), 
    m_route_table_dirty( false ),m_srur_packets( 0 ), m_interest_packets( 0 ), m_data_packets( 0 )
{
  time_block = time_block_collector;
  if (options.cache != ProtocolOptions::CACHING_ON_AUTO)
  {
    if (options.cache == ProtocolOptions::CACHING_OFF)
      cache_repl = 'o';
    cache_size = m_options.cache_limit;
    if (options.cache == ProtocolOptions::CACHING_ON_LRU)
      cache_repl = 'r';
    else if (options.cache == ProtocolOptions::CACHING_ON_RND)
      cache_repl = 'c';
  }
  if (options.cache_manag == ProtocolOptions::MANAGEMENT_STANDARD)
    m_transport_node = new CCNxNode( nid, options, cache_repl, cache_size);
  
  m_load_balancer = new LoadBalancing(this);
  
}

void CBCBNode :: setup_FIB_entry(unsigned int id,cbnsim::Predicate pred)
{
  update_route_table(p2i(id),pred,-1);
}


ProcessId CBCBNode :: activate()
{
  ProcessId id = Node::activate();
  CBNASSERT( id != ssim::NULL_PROCESSID, "activate should not return a NULL processid" );
  CBNASSERT( m_route_table.empty(), "route table should be empty during activate" );
  m_route_table.push_back( Predicate() );
  
  m_msg_sent_counts.push_back( 0 );
  m_loss_rate.push_back( 0 );
  m_link_rate.push_back( 0 );
  m_avg_fwd_info_interval = m_options.m_fwd_info_interval;
  Queue*	queue=NULL;
  m_output_queues.push_back(queue);
  m_queues_lenght.push_back(0);
  return id;
}

void CBCBNode :: add_neighbor( ssim::ProcessId id, ssim::Time delay , double buffer, double capacity)
{
  Node::add_neighbor( id, delay , buffer , capacity);
  m_route_table.push_back( Predicate() );
  Queue*	queue=NULL;

  if (m_options.qd == ProtocolOptions::QD_FIFO)
    queue = new FIFO(this, pid(), id,m_output_queues.size(),buffer,capacity,delay,m_options.sim_time_unit );
  else if (m_options.qd == ProtocolOptions::QD_DRR)
    queue = new FQ(this, pid(), id,m_output_queues.size(),buffer,capacity,delay,m_options.sim_time_unit );
  
  
  m_output_queues.push_back(queue);
  m_queues_lenght.push_back(0);
  
  CBNASSERT( ! m_route_table.empty(), "route table should not be empty during add_neighbor call" );
  
  m_msg_sent_counts.push_back( 0 );
  m_loss_rate.push_back( 0 );
  m_link_rate.push_back( 0 );
}

size_t CBCBNode :: bytesize() const
{
  size_t bsize = 0;
  unsigned int size = m_route_table.size();
  for( unsigned int i = 0; i < size; ++i )
    {
      bsize += m_route_table[i].bytesize();
    }
  return bsize + m_fwd_table.bytesize();
}

void CBCBNode :: init()
{
  /** predicate issue policy
   */
  if( m_options.pip == ProtocolOptions::PIP_TIMED )
    {
  /** the memory leak doesn't matter 
   * since the timer lasts until the simulation is finished.
   */
      PredicateIssueTimer* pit = new PredicateIssueTimer( *this, m_options.pip_time );
      pit->activate();
    }
  /** predicate refresh policy
   */
  if( m_options.prp == ProtocolOptions::PRP_TIMED )
    {
      PredicateRefreshTimer* prt = new PredicateRefreshTimer( *this, m_options.prp_time );
      prt->activate();
    }
  /** sr send policy
   */
  if( m_options.srsp == ProtocolOptions::SRSP_TIMED ||
      m_options.srsp == ProtocolOptions::SRSP_TIMED_COUNT )
    {
      SRSendTimer* srst = new SRSendTimer( *this, m_options.srsp_time );
      srst->activate();
    }
}

void CBCBNode :: dequeue_packet(void* q)
{
  ((Queue *)q)->transmit();
  ((Queue *)q)->dequeue();
}

void CBCBNode :: receiver_TO(unsigned int ifid_sent,string socket_addr, unsigned int chunk_id, ssim::Time start_time)
{
  outgoing_packets send_interest;
  send_interest = m_transport_node->receiver_TO(ifid_sent,socket_addr,chunk_id, start_time);
  
  for (std::list<std::pair<unsigned int,Packet*> >::iterator it = send_interest.begin(); it!=send_interest.end(); it++)
    m_output_queues[(*it).first]->enqueue((CCNxPacket*)(*it).second);
}

void CBCBNode :: receiver_CBR(unsigned int ifid_sent,string socket_addr,  ssim::Time start_time)
{
  outgoing_packets send_interest;
  send_interest = m_transport_node->receiver_CBR(ifid_sent, socket_addr, start_time);
  
  for (std::list<std::pair<unsigned int,Packet*> >::iterator it = send_interest.begin(); it!=send_interest.end(); it++)
    m_output_queues[(*it).first]->enqueue((CCNxPacket*)(*it).second);
}

void CBCBNode :: issue_packet( unsigned int ifid, const Packet* packet, Time lag, bool count )
{
}


void CBCBNode :: issue_predicate( bool force,ProcessId dest, unsigned int path_number )
{
  NDEBUG( "****************** start issue_predicate() ******************" );
  if( m_issue_pending || force )
    {
      if( !m_issue_pending )
	{
	  NDEBUG( "predicate issue is being forced" );
	}
      propagate_ra( pid(), dest, path_number, m_route_table[0] );
      m_issue_pending = false;
    }
  NDEBUG( "****************** end issue_predicate() ******************" );
}

void CBCBNode :: issue_sr()
{
  NDEBUG( "****************** start issue_sr() ******************" );
  propagate_sr( pid(), SR::next_number() );
  NDEBUG( "****************** end issue_sr() ******************" );
}

void CBCBNode :: process_packet( unsigned int ifid, const Packet& pkt )
{
  NDEBUG( "****************** start process_packet ******************" );
  NDEBUG( "received packet from process: " << i2p(ifid) << " (interface " << ifid << ")" );
  const RA* ra;
  const ME* me;
  const SR* sr;
  const UR* ur;
  const INTERESTPacket* interest;
  const INFOPacket* info;
  const DATAPacket* data;

  if( (ra = dynamic_cast<const RA*>( &pkt )) )
    process_ra( ifid, *ra );
  else if( (me = dynamic_cast<const ME*>( &pkt )) )
    process_me( ifid, *me );
  else if( (sr = dynamic_cast<const SR*>( &pkt )) )
    process_sr( ifid, *sr );
  else if( (ur = dynamic_cast<const UR*>( &pkt )) )
    process_ur( ifid, *ur );
  else if( (interest = dynamic_cast<const INTERESTPacket*>( &pkt )) )
  {
    m_interest_packets++; 
    process_interest( ifid, *interest);
  } 
  else if( (info = dynamic_cast<const INFOPacket*>( &pkt )) )
    process_data( ifid, *info); 
  else if( (data = dynamic_cast<const DATAPacket*>( &pkt )) )
    process_data( ifid, *data);      
  else
  {
    CBNASSERT( false, "Unexpected event type" );
    cout << "error unknown packet" << endl;
  }
  NDEBUG( "****************** end process_packet ******************" );
}

int CBCBNode :: process_data( unsigned int ifid, const DATAPacket& data )
{

//    cout << setiosflags(ios::fixed) << setprecision(10) << Sim::clock() << " from node " << i2p(ifid) <<  " processing DATA at node=" << pid() << " for " << data.content_name() 
//         << "/" << data.chunk_id() <<  " port "<< data.port() << " content size " << data.content_size() << " interest time "
//         << Sim::clock()-data.interest_issue_time() << endl;

       
  const Message msg=data.msg();
  
  /** Lookup into the PIT and forward the packet 
   */
  
  //update delay before remove the interests from the PIT
  MatchHandler mh;
  unsigned int port;
  m_fwd_table.match( msg, mh );
  const set<siena::if_t>& matches = mh.getIds();
  cname prefix = m_transport_node->content_name(data.msg(),mh.match_len());
  m_load_balancer->pending(prefix,ifid,'-');

  match match_out = m_transport_node->process_out(ifid, data);
  outgoing_packets send_interest;
  
  
  /** iterate on each interface and process data
   * for all outgoing interface for a data packet, different to local interf, send out the data 
   * and cache if required. if data is for local interface, pass it to upper layers and if necessary, 
   * send next INTEREST in the content download process
   */
  for ( match::iterator it = match_out.begin(); it != match_out.end(); it++ )
  { 
    /** compute the match in the fwd table even if this is a data 
     *  this information will be used to update prefix_table stats and when the data is for the localhost in order
     *  to find out mathces for the next interest
     */

    /** DATA/INFO is not for the local node
     */
    if ( (*it).ifid != 0)
    {
      /** build the matching prefix using the match length given by the "modified" Match Handle
       */
      if (data.chunk_id() == 0)
      {
	 Sim::signal_event(i2p((*it).ifid), new INFOPacket( pid(), (INFOPacket&)(data)), m_output_queues[(*it).ifid]->delay());
	 continue;
      }    
      std::stringstream source;
      source << data.source() << pid();
      m_output_queues[(*it).ifid]->enqueue(new DATAPacket( pid(), data,(*it).port,source.str()));

      m_queues_lenght[(*it).ifid]=(*m_output_queues[(*it).ifid]).avg_length();
      m_transport_node->add_in_cache( data );
      continue;
    }
    /** DATA/INFO is for the local node, (open the socket if you received the INFO)
     *  manage packet and compute if receiver needs to express further 
     *  interests in the content download process 
     */

    
    /** if no matches for DATA/INFO content name, do not compute interests that need to be  eventually 
     *  sent out, they will be dropped.
     */
    if (matches.size() == 0)
    {
      cout << "received a DATA but are not matching interfaceces for INTEREST on the same content name!!"<< endl;
      cout << "Something wrong, INFO or DATA packet was data.content_name() previously requested, routing is changed!"<< endl;
      cout << "Node " << pid() << " No Route to host" << endl;
      continue;
    }
    
    /** if this is an INFO open socket and manage it, if it is a DATA, socket is already opened, manage DATA
     */
    if (data.chunk_id()==0)
    {
       port = m_transport_node->open_socket(INFOPacket( pid(), (INFOPacket&)(data)), matches);
       send_interest = m_transport_node->manage_data(DATAPacket(pid(),data,port), ifid);
    }
    else
       send_interest = m_transport_node->manage_data(data, ifid); 
    if( m_route_table_dirty )
    {
      NDEBUG( "route table dirty, updating fwd table..." );
      update_fwd_table();
    }
    
    /** forward INTERESTS if needed
     */
    for (outgoing_packets::iterator it = send_interest.begin(); it!=send_interest.end(); it++)
    {
      /** if the interface (indicated the Socket manager) is matching, forward the interest
       *  and eventually, update forwarding informations
       */
      if (matches.find((*it).first) != matches.end()){
	m_output_queues[(*it).first]->enqueue((CCNxPacket*)(*it).second);
	m_queues_lenght[(*it).first]=(*m_output_queues[(*it).first]).avg_length();
      }
    }
  }
  return 1;
}


int CBCBNode :: process_interest( unsigned int ifid, const INTERESTPacket& interest )
{

//   cout  <<  setiosflags(ios::fixed) << setprecision(10) << Sim::clock() << " from node " << i2p(ifid) 
//         << " processing INTEREST at " << pid() << " on "<< interest.content_name() << "/"<< interest.chunk_id() 
//         << " received from interface " << ifid << " port " << interest.port() << " content size " << interest.content_size() 
//         << endl;


  NDEBUG( "received INTEREST: " << interest );
  if( m_route_table_dirty )
  {
    NDEBUG( "route table dirty, updating fwd table..." );
    update_fwd_table();
  }
  
// Activate Jim's mechanism
//   FQ* fq;
//   if( (fq = dynamic_cast<FQ*>( m_output_queues[ifid] )) )
//   {
//     if (!fq->dec_count(interest))
//     {
//       cout << Sim::clock() << " dropped interest"<< interest.content_name() << "/"<< interest.chunk_id() << endl;
//       return 1; 
//     }
//   }

 const DATAPacket* data = NULL;

  MatchHandler mh;
  m_fwd_table.match( interest.msg(), mh );
  set<siena::if_t>& matches = (set<siena::if_t>&) mh.getIds();
  set<siena::if_t>::iterator i = matches.find(0);
  /** If Interest for a local process, pass it to upper levels
   */
  if (i != matches.end())
  {
 

    delete (data);
    data=NULL;    
    /** check if interest is for a valid content (get data from repo)
     */
    CCNxPacket* data_packet = m_transport_node->manage_interest(interest); 
    
    if ( data_packet != NULL ){
      std::stringstream source;
      source << pid();
      (*data_packet).set_source(source.str());

      if (data_packet->chunk_id() == 0)

	Sim::signal_event(i2p(ifid), data_packet, m_output_queues[ifid]->delay());
      else{
	
	m_output_queues[ifid]->enqueue(data_packet);
	m_queues_lenght[ifid]=(*m_output_queues[ifid]).avg_length();
      }
    }
    else { cout << pid () << " content not founded don't send info packet" << endl;}

    
    return 1;
  }
  
  /** Interest is not for local node  */
  
  /** 1-------Content Store Look up
    */
  if (interest.chunk_id() != 0 ) 
    data = m_transport_node->search_in_cache( interest );
  if (data != NULL)
  {
    std::stringstream source;
    source << pid();
    DATAPacket* data_packet = new DATAPacket(pid(), (*data), (*data).port(), source.str());
    delete(data);
    data=NULL;
    m_output_queues[ifid]->enqueue((CCNxPacket*)data_packet);
    m_queues_lenght[ifid]=(*m_output_queues[ifid]).avg_length();
    return 1;
  }

  /** 2------PIT Look up
    */
  if (!( m_transport_node->record_interest(ifid ,interest, Sim::clock(), NULL)  ) )	
  {
    /** 3------FIB Look up
      */
    if (matches.size() == 0)
    {
      cout << "No route to host" << endl;
      return 0;
    }
    
    i=matches.begin();
   
    /** build the matching prefix using the match length given by the "modified" Match Handler 
      */
    cname prefix = m_transport_node->content_name(interest.msg(),mh.match_len());
    unsigned int fwd_ifid = m_load_balancer->fwd_interest(prefix); 
    
    /** An interst on INFO packet, (apply to it only the propag. delay)
     */       
    if (interest.chunk_id() == 0)
    {
      Sim::signal_event(i2p(fwd_ifid), new INTERESTPacket( pid(), interest), m_output_queues[fwd_ifid]->delay());
      return  1;
    }      
    m_output_queues[fwd_ifid]->enqueue(new INTERESTPacket( pid(), interest));
    m_queues_lenght[fwd_ifid]=(*m_output_queues[fwd_ifid]).avg_length();

    
    return 1;
  }
  return 1;
}

void CBCBNode :: process_me( unsigned int ifid, const ME& me )
{
  if( ! propagate_me( me.issuer(), ifid, me.message_id(), me.message() ) )
    {
      cbnsim::DeliveryOracle::log_false_delivery( me.message_id(), pid() );
    }
}

void CBCBNode :: process_ra( unsigned int ifid, const RA& ra )
{
  NDEBUG( "received RA: " << ra );
  /** cp is the existing predicate for the src node
   */
  update_route_table(ifid, ra.predicate(), ra.dest());
  /** propagate the new predicate down the broadcast tree rooted
   *  at the issuer's node.
   */
  propagate_ra( ra.issuer(), ra.dest(), ra.path_number(),ra.predicate() );
}

bool CBCBNode :: update_route_table(unsigned int ifid, const Predicate pred, int dest)
{
  Predicate& cp( m_route_table[ifid] );

  NDEBUG( "initial predicate for process " << i2p(ifid) << ": " << cp );
  /** 1) see if new predicate is already covered by
   *  the existing predicate for the incoming interface.
   */
  if( ( siena::covers( cp, pred ) ) && (dest == -1) )
  {
    NDEBUG( cp << " covers " << pred );
    return false;
  }
  else
  {
    /** 2) update the route table entry for the incoming interface with
     *  the new predicate.
     */
    cp.add( pred );
    /** add the "Filters" included in this predicate into the load balancing's prefix table
     */    
    set<Filter> filters = pred.getFilters();
    set<Filter>::iterator i=filters.begin();
    while (i!=filters.end())
    {
      cname prefix = m_transport_node->content_name((*i),i->size());
      i++;
      m_load_balancer->add(prefix,ifid);
    }
    NDEBUG( "updated predicate for process " << i2p(ifid) << ": " << cp );
  
    NDEBUG( "marking route table dirty" );
    m_route_table_dirty = true;
    return true;
  }
}


void CBCBNode :: process_sr( unsigned int ifid, const SR& sr )
{
  /** first propagate down to children
   */
  unsigned int count = propagate_sr( sr.issuer(), sr.number() );
  /** if there were no recipients to wait for, then send a UR reply to
   *  parent.
   */
  if( !count )
  {
    NDEBUG( "effective leaf node for SR: " << sr );
    
    ProcessId parent = m_bl.getParent( sr.issuer(), pid() );
    UR* ur = new UR( pid(), sr.issuer(), sr.number(), m_route_table[0] );
    NDEBUG( "responding to parent(" << parent << ") with UR: " << *ur );
    issue_packet( parent, ur);
  }
}

void CBCBNode :: process_ur( unsigned int ifid, const UR& ur )
{
  ProcessId sender = i2p(ifid);
  CBNASSERT( sender == ur.sender(), "sender calculations do not match" );
  /** if we were the issuer of the SR that this is in response to
   * just set the predicate for the UR's sender
   */
  if( ur.sr_issuer() == pid() )
  {
    NDEBUG( "Setting predicate for " << sender << " to " << ur.predicate() );
    m_route_table[ifid] = ur.predicate();
    NDEBUG( "marking route table as dirty" );
    m_route_table_dirty = true;
  }
  /** otherwise we have to remove the sender from the respondees list
   * and propagate up to our parent if necessary
   */
  else
  {
    map<unsigned int,sr_response>::iterator i = m_sr_responses.find( ur.sr_number() );
    if( i == m_sr_responses.end() )
    {
      NDEBUG( "received unexpected UR: " << ur << ", node failed previously?" );
      return;
    }
      
    set<ProcessId>::iterator j = (*i).second.respondees.find( sender );
/*    CBNASSERT( j != (*i).second.respondees.end(), pid() << "@" << Sim::clock() << ": received UR from unexpected respondee: " << ur );   */
    /**  add this predicate to the predicate being collected
     */
    NDEBUG( "adding predicate from " << sender << " to entry for SR number " << ur.sr_number() );
    (*i).second.pred.add( ur.predicate() );
    /** erase respondee from 
     */
    (*i).second.respondees.erase( j );

    if( (*i).second.respondees.empty() )
    {
      NDEBUG( "all URs have been received for SR number " << ur.sr_number() );
      /** make sure to add my own predicate in there...
       */
      (*i).second.pred.add( m_route_table[0] );
      /** lookup parent and send
       */
      ProcessId parent = m_bl.getParent( ur.sr_issuer(), pid() );
      NDEBUG( "issuing completed UR packet for SR number " << ur.sr_number() << " to " << parent );
      issue_packet( parent, new UR( pid(), ur.sr_issuer(), ur.sr_number(), (*i).second.pred ));
      m_sr_responses.erase( i );
    }
  }
}


bool CBCBNode :: propagate_me( ProcessId issuer, unsigned int ifid, MessageId msg_id, const Message& msg )
{
  ME* me( NULL );
  bool matched_something = false;
  
  const set<ProcessId>& pids( m_bl.getChildren( issuer, pid() ) );

  if( m_route_table_dirty )
  {
    NDEBUG( "route table dirty, updating fwd table..." );
    update_fwd_table();
  }
  cbnsim::MatchHandler mh;
  m_fwd_table.match( msg, mh );

  const set<siena::if_t>& matches = mh.getIds();
  for( set<siena::if_t>::const_iterator i = matches.begin();i != matches.end(); ++i )
  {
    if( ! (*i) )
    {
      /** we matched ourself...
       */
      NDEBUG( "matched local application" );
      matched_something = true;
      cbnsim::DeliveryOracle::log_true_delivery( msg_id, pid() );
      /** record the route at the potential client
       * received routing info record it at transport layer 
       */
    } 
    else
    {
      ProcessId p( i2p( (*i) ) );
      NDEBUG( "matched process " << p << " (interface " << (*i) << ")" );
      /** we only propagate to neighbors that
       *  the broadcast layer said we could.
       *  OR IF IT MAKES PART OF THE ALTERNATIVE PATH
       */    
      if( ( pids.find( p ) != pids.end() ) && ( (*i) != ifid ) )
      {
	if( !me )
	{
	  me = new ME( pid(), issuer, msg_id, msg, 0 );
	}
	issue_packet( (*i), me);
	m_msg_sent_counts[(*i)]++;
	matched_something = true;
      }
    }
  }
  if( !matched_something )
  {
    NDEBUG("message did not match any interfaces, dropping..." );
  }
  return matched_something;
}

int CBCBNode :: propagate_ra( ProcessId issuer, ProcessId dest, unsigned int path_number, const Predicate& pred )
{
  if (dest == -1)
  {
    const set<ProcessId>& children( m_bl.getChildren( issuer, pid() ) );
    NDEBUG( children.size() << " children in broadcast tree rooted at " << issuer );
    RA* ra = NULL ;
    set<ProcessId>::const_iterator i;
    for( i = children.begin(); i != children.end(); i++ )
    {
      ra = new RA( pid(), issuer, dest, path_number,pred );
      NDEBUG( "propagating RA packet issued by " << issuer << " to " << (*i) );   
      Sim::signal_event((*i), ra, m_output_queues[p2i(*i)]->delay());
    }
    return 1;
  }
  ProcessId next_node = m_bl.getNext(issuer,dest,pid(),path_number);
  RA* ra = new RA( pid(), issuer, dest, path_number,pred );
  if ((next_node != -1) && (dest != pid()) )
    Sim::signal_event(next_node, ra, m_output_queues[p2i(next_node)]->delay());  
  return 1;
}

unsigned int CBCBNode :: propagate_sr( ProcessId issuer, unsigned int number )
{
  const set<ProcessId>& children( m_bl.getChildren( issuer, pid() ) );
  NDEBUG( children.size() << " children in broadcast tree rooted at " << issuer );
  SR* sr( NULL );
  unsigned int count = 0;
  set<ProcessId>::const_iterator i;
  for( i = children.begin(); i != children.end(); ++i )
    {
      if( m_route_table[p2i(*i)].size() == 0 ||
	  (m_options.srsp == ProtocolOptions::SRSP_TIMED_COUNT && m_msg_sent_counts[p2i(*i)] < m_options.srsp_count) )
      {
	  continue;
      }
      if( !sr )
	sr = new SR( pid(), issuer, number );
      NDEBUG( "propagating SR packet issued by " << issuer << " to " << (*i) );
      issue_packet( (*i), sr);
      /** if we are not the issuer of this SR, but we are sending
       * down to some children we need to keep track of them.
       */
      if( issuer != pid() )
      {
	NDEBUG( "adding process " << (*i) << " to the respondees list of " << number );
	m_sr_responses[number].respondees.insert( (*i) );
      }
      count++;
    }
  return count;
}

void CBCBNode :: send_message( MessageId mid, const Message& message )
{
  NDEBUG( "****************** start send_message ******************" );
  NDEBUG( "received send_message: " << message );
  //cout << "send message" << endl;
  propagate_me( pid(), 0, mid, message );
  NDEBUG( "****************** end send_message   ******************" );
}

void CBCBNode :: publish_content(const Predicate& p, unsigned int size, unsigned int bytesize, unsigned int receiver_win, unsigned int fec_perc, unsigned int popularity_class) 
{
  m_transport_node->record_content(p, size, bytesize, receiver_win, fec_perc, popularity_class);
}

void CBCBNode :: download_content(const Message& msg) 
{
  if( m_route_table_dirty )
  {
    NDEBUG( "route table dirty, updating fwd table..." );
    update_fwd_table();
  }
  
  /** Start the content download process, issuing an INTEREST packet on chunk id 0
   */
  MatchHandler mh;
  m_fwd_table.match( msg, mh );
  const set<siena::if_t>& matches = mh.getIds();
  cname content_name = m_transport_node->content_name(msg, msg.size());
  const INTERESTPacket* interest = new INTERESTPacket( pid(), content_name, 0, Sim::clock(), 1, msg, 0, 0, 0, 0);
  
  if (matches.size() != 0)
  {
     /** Send the INTEREST for the INFO packet only to one interface 
      *  (for the future, send this INTEREST to the primary path!!!)
      */
     set<siena::if_t>::const_iterator i = matches.begin();   
     NDEBUG( "matched process " << p << " (interface " << (*i) << ")" );
     Sim::signal_event(i2p(*i), interest, m_output_queues[(*i)]->delay());
     m_transport_node->record_interest(0 , *interest, Sim::clock(),NULL);
  }
  else
  {
    cout << "Node "<< pid() <<" No Route to host for : "<< interest->content_name() << endl;
    delete(interest);
  }
  
}


void CBCBNode :: set_multipath(unsigned int to, const Predicate& p)
{
  //TODO this is for automatic routing 
  NDEBUG( "****************** start set_multipath ******************" );
  if ( m_options.prp == ProtocolOptions::PRP_ALWAYS || m_options.prp == ProtocolOptions::PRP_TIMED )
  {
    NDEBUG( "received set_multipath: " << p );
    NDEBUG( "marking route table as dirty" );
     m_issue_pending = true;
    if( m_options.pip == ProtocolOptions::PIP_IMMEDIATE )
    {
      bool found = true;
      unsigned int path_number = 2;
      while (found)
      {
	if (m_bl.getNext(pid(),to,pid(),path_number)!= NULL_PROCESSID)
	{
	  issue_predicate(false,to,path_number);  
	}
	else
	  found = false;
	path_number ++;
      }
    }
  }
}

void CBCBNode :: set_predicate( const Predicate& p, unsigned int, bool )
{
  NDEBUG( "****************** start set_predicate ******************" );
  if ( m_options.prp == ProtocolOptions::PRP_ALWAYS || m_options.prp == ProtocolOptions::PRP_TIMED )
  {
    NDEBUG( "received set_predicate: " << p );
    Predicate& cp( m_route_table[0] );
    /** 1) see if new predicate is already covered by
     *  the existing predicate for the incoming interface.
     */    
    if ( siena::covers( cp, p ) ) 
    {
      NDEBUG( cp << " covers " << p );
      return;
    }
    /** 2) update the route table entry for the incoming interface with
     *  the new predicate.
     */
    else
    {
      cp.add( p );
      NDEBUG( "marking route table dirty" );
      m_route_table_dirty = true;
    }  
    m_issue_pending = true;
    if( m_options.pip == ProtocolOptions::PIP_IMMEDIATE )
    {
      issue_predicate();
    }
  }
 NDEBUG( "****************** end set_predicate ******************" );
}

void CBCBNode :: update_fwd_table()
{
  m_fwd_table.clear_recycle();
  unsigned int size = m_route_table.size();
  for( unsigned int i = 0; i < size; ++i )
    {
      NDEBUG( "ifconfig " << i << "(" << i2p( i ) << ") -> " << m_route_table[i] );
      m_fwd_table.ifconfig( i, m_route_table[i] );
    }
  m_fwd_table.consolidate();
  m_route_table_dirty = false;
}
