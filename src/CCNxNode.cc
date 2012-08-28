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
#include <siena/types.h>
#include <siena/ssimp_types.h>
#include "cbcbsim/CCNxNode.h"
#include "cbcbsim/ProtocolOptions.h"
#include <siena/ssim.h>

using namespace cbcbsim;
using namespace cbnsim;
using namespace ssim;
using namespace std;


#define NDEBUG(m) CBNDEBUG( "CCNxNode[" << pid() << "@" << Sim::clock() << "]: " << m )


void CCNxNode :: record_content( const Predicate& p, unsigned int size, unsigned int bytesize, unsigned int receiver_win, 
				 unsigned int fec_perc, unsigned int populariy_class)
{
  Predicate pred = p;
  ServedContent* record;
  record = new ServedContent(pred,size,bytesize,receiver_win,fec_perc,populariy_class);
  m_served_contents.insert(make_pair(record->address(),record));
}

bool CCNxNode :: record_interest( unsigned int ifid,const INTERESTPacket& interest, Time start_time, 
				  Socket* socket)
{
  bool PIT_match=false;
  
  PIT_match = m_PIT_table->record_interest(ifid,interest,start_time,socket);
  
  if (Sim::clock() > m_options.sim_startup)
    if ((PIT_match)&&(m_options.m_filtering == ProtocolOptions::FILTERING_ON)&&(ifid!=0))
      filter_rate_update(interest.popularity_class());
    
  return PIT_match;
}

Time CCNxNode :: registration_time(unsigned int ifid,DATAPacket data){
  return m_PIT_table->registration_time(ifid,data);
}

match CCNxNode :: process_out( unsigned int ifid, DATAPacket data)
{
  /** @brief look up in PIT used when receive data and need to fwd it
   *  consume PIT entries if needed
   */
  return m_PIT_table->process_out(ifid,data);
}

CCNxPacket* CCNxNode :: manage_interest( const INTERESTPacket& interest )
{
  CCNxPacket* sending_packet=NULL;
  served_contents::iterator it;
  if ( interest.chunk_id() == 0 )
  {
    it = m_served_contents.find(interest.content_name());
    if (it != m_served_contents.end())
      sending_packet = new INFOPacket( m_id, interest.content_name(), interest.msg(),interest.issue_time(),(*(*it).second));
    else 
    {
      cout << "ERROR: Predicate not served" << endl;
    }
  }
  else 
    sending_packet = new DATAPacket( m_id, interest);
  return sending_packet;
}

unsigned int CCNxNode :: open_socket( const INFOPacket& info , const set<siena::if_t>& matches)
{
  unsigned int port = 0;
  string address;
  stringstream port_s; 
  bool found = false;
  while ( !found )
  {
    stringstream port_s; 
    port_s << port;
    address = info.content_name() + "/port:" + port_s.str();
    if (m_socket_manager.find(address) == m_socket_manager.end())
      found = true;
    else
      port++;
  }
  
  
    SocketManager* new_socket_man = new SocketManager(m_options, m_id, info, port, matches);
    m_socket_manager.insert( make_pair(address, new_socket_man));

  NOpenConnections++;

  return port;
}

cname CCNxNode ::content_name( Message msg , unsigned int an )
{
  string address;
  const map<string,Message::Value>& attrs = msg.getAttributes();
  map<string,Message::Value>::const_iterator k;
  unsigned int attributes = 0;
  for( k = attrs.begin(); ((k != attrs.end())&&(an > attributes)); ++k )
  {
    address += "/";
    address += (*k).second.s_value;
    attributes++;
  }
  return address;
}


cname CCNxNode :: content_name( const Filter f, unsigned int an )
{
  set<Constraint> c = f.getConstraints();
  set<Constraint>::iterator i = c.begin();
  cname content_name;
  while(i!=c.end())
  {
    siena::string_t string_val;
    int int_val;
    bool bool_val;
    double double_val;
    string attribute;
    switch (i->type()){
      case siena::string_id: string_val = i->string_value();
			     attribute = string( string_val.begin, string_val.length() );  
			     content_name.append("/");
			     content_name.append(attribute); 
			     break;
      case siena::int_id:    int_val = i->int_value(); 
			     //cout << "int = " << it->int_value(); 
			     break;

      case siena::bool_id:   bool_val = i->bool_value();
			     //cout << "bool = " << it->bool_value(); 
			     break;

      case siena::double_id: double_val = i->double_value(); 
			     //cout << "double = " << it->double_value(); 
			     break;
      default :              cout << "error route"<< endl ; break;  
    }

    i++;
  }
  return content_name;
}


outgoing_packets CCNxNode :: manage_data(const DATAPacket& data, unsigned int ifid)
{ 
  outgoing_packets interest_packets;  //pair(ifid,interest to send)
  stringstream port;  port << data.port();
  string  address = data.content_name() + "/port:" + port.str();

  /** @brief search for socket interested in this URI/port_id 
   */
  
  socket_map::iterator it = m_socket_manager.find(address);
  if (it == m_socket_manager.end())
    return interest_packets;
 
  it->second->received_chunk(data.chunk_id(),data.size(),ifid,data.source());

  if (data.chunk_id()==0)
    return set_outgoing_interests(it->second, ifid);
  
  SocketManager* socket = dynamic_cast<SocketManager*> (it->second);
  if( (socket->FlowControllerType()==ProtocolOptions::fc_CBR )){
    	return interest_packets;
  }
  
  if ( !socket->completed() ){
    return set_outgoing_interests( socket, ifid);
  }
  
  std::map<string,int> stat = socket->get_paths_stat();
  std::map<string,int>::iterator iter_path;
  int source_tot=0;
  double source_avg=0.0;
  int used_packets=0;
  double square_sum=0;
  for(iter_path=stat.begin();iter_path!=stat.end();iter_path++){
      source_tot++;
      used_packets+=(*iter_path).second;
      square_sum+=pow((*iter_path).second,2);
  }
  if (square_sum>0){
    source_avg=pow(used_packets,2)/square_sum;
  }

    NOpenConnections--;
    cout << Sim::clock() << "\tclosed socket " << m_id << "\t" << (*it).first <<  "\t" 
	    << (Sim::clock()-(*it).second->start_time())*m_options.sim_time_unit << "\t" 
	    << socket->filesize()  << "\t" << socket->VRTT()*m_options.sim_time_unit 
	    << "\t" << socket->delay()*m_options.sim_time_unit << "\tclass " 
	    << socket->popularity_class() << "\t" << NOpenConnections << "\t" 
	    << socket->avg_win() << "\t" << source_tot <<"\t" << source_avg <<  endl;
    socket->delete_receiver();
    delete(socket);
    m_socket_manager.erase(it);
    return interest_packets;

}

outgoing_packets CCNxNode :: receiver_CBR(unsigned int ifid_sent, std::string socket_addr, ssim::Time start_time)
{
  outgoing_packets send_interest;
  socket_map::iterator it_socket;
  SocketManager* socket;
  it_socket=m_socket_manager.find(socket_addr);
  
  if ((socket = dynamic_cast<SocketManager*>( it_socket->second )))
  {
    send_interest = set_outgoing_interests(socket, ifid_sent);
    if(send_interest.size() == 0)
    {
      NOpenConnections--;
      cout << Sim::clock() << "\tclosed socket " << m_id << "\t" << (*it_socket).first <<  "\t" 
	    << (Sim::clock()-(*it_socket).second->start_time())*m_options.sim_time_unit << "\t" 
	    << socket->filesize()  << "\t" << socket->VRTT()*m_options.sim_time_unit 
	    << "\t" << socket->delay()*m_options.sim_time_unit << "\tclass " 
	    << socket->popularity_class() << "\t" << NOpenConnections << "\t" 
	    << socket->avg_win() << "\t" << 0 << "\t" << 0 << endl;
      delete(socket);
      m_socket_manager.erase(it_socket);
      return send_interest;
    }
    unsigned int i=0;
    while (i< socket->m_window_controllers.size()){
      if(socket->m_window_controllers[i]->ifid() == ifid_sent){
	CBR* fc_cbr = (CBR*) socket->m_window_controllers[i];
	fc_cbr->receiverCBR();
	return send_interest;
      }
      i++;
    }
  }
  return send_interest;
}

outgoing_packets CCNxNode :: set_outgoing_interests(Socket* socket,unsigned int ifid)
{
  outgoing_packets interest_packets;
  list< requesting_chunk > chunk_list;
  /**request to the socket which pkts he is interested in
   */
  chunk_list = socket->get_interested_chunk();

  
  /**phisically build packets and check in the PIT
   */
  for (list< requesting_chunk >::iterator i = chunk_list.begin(); i!=chunk_list.end(); i++)
  {   
    INTERESTPacket* interest = new INTERESTPacket( m_id, socket->content_name(), (*i).chunk_id, Sim::clock() , 1, socket->msg(), socket->chunk_size(), 
			       socket->port(), socket->popularity_class(), socket->tot_filesize());
    if (!(record_interest(0 ,*interest, socket->start_time(), socket)))
    {
      socket->requested_chunk(interest->chunk_id()); 
      interest_packets.push_back(make_pair((*i).ifid,interest));
      continue;
    }
    if(interest!=NULL)
    {
      delete(interest);
      interest=NULL;
    }
  }
  return interest_packets;
}


void CCNxNode :: add_in_cache( const DATAPacket& data )
{
  if(m_replacement != 'o')
     m_cache->add_in_cache(data);
}


const DATAPacket*  CCNxNode :: search_in_cache( const INTERESTPacket& interest )
{
  const DATAPacket* cached_chunk = NULL;
  if(m_replacement != 'o')
  {
    cached_chunk = m_cache->search_by_name(interest);

    if (Sim::clock() > m_options.sim_startup)
    {
      if (cached_chunk == NULL)
	miss_rate_update(interest.popularity_class()); 
      else   
	hit_rate_update(interest.popularity_class());
    }
  }  
  return cached_chunk;
}


outgoing_packets CCNxNode :: receiver_TO(unsigned int ifid_sent, string socket_addr, 
					 unsigned int chunk_id,ssim::Time start_time)
{
  socket_map::iterator it_socket;
  outgoing_packets send_interest;
  it_socket=m_socket_manager.find(socket_addr);
  SocketManager* socket;
  if ( (it_socket != m_socket_manager.end()) 
    &&  (socket = dynamic_cast<SocketManager*>( it_socket->second ))) 
  {
    if ( (start_time == socket->start_time())&&
	   !socket->check_received(chunk_id))
    {
       socket->receiver_TO(ifid_sent,chunk_id);
    }
    if (!socket->aborted())
      send_interest = set_outgoing_interests( (Socket*)socket, ifid_sent);
  }

  return send_interest;
}