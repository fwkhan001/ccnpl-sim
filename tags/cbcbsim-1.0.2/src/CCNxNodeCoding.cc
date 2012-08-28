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

#include <cbnsim/SetPredicateEvent.h>
#include <cbnsim/SetPredicateParser.h>
#include <cbnsim/WorkloadParser.h>
#include <cbnsim/types.h>
#include <siena/types.h>
#include <siena/ssimp_types.h>
#include "cbcbsim/INTEREST.h"
#include <cbnsim/NodeDeletePacket.h>
#include <siena/ssim.h>
#include "cbcbsim/SocketManager.h"
#include "cbcbsim/SERVEDPREDICATE.h"
#include "cbcbsim/CCNxNodeCoding.h"


using namespace cbcbsim;
using namespace cbnsim;
using namespace ssim;
using namespace std;


#define NDEBUG(m) CBNDEBUG( "CCNxNodeCoding[" << pid() << "@" << Sim::clock() << "]: " << m )

void CCNxNodeCoding :: add_in_cache( const DATA& data )
{
    INTEREST* interest;
    interest =  new INTEREST( m_id, m_id, data.receiver(), data.chunk_id(), data.address(), -1 , 1, data.msg(), data.size(), data.port(), 0, 0);
    std::map<std::string,unsigned int>::iterator it_m_chunk_content; //coding declaration

    if ( m_options.cach == ProtocolOptions::CACHING_ON_LRU)
    {
      const DATA* data_searched = search_in_cache_new(*interest);

      if ( data_searched == NULL)
      {
    	 //coding modifications
    	  it_m_chunk_content= m_chunk_content.find(data.address());
    	    if ( it_m_chunk_content != m_chunk_content.end() )
				{
    	    	if ((*it_m_chunk_content).second<m_chunk_number)
    	    	{
    	    		(*it_m_chunk_content).second++;
    	    		//printf("IN %d %s %d\n", m_id, data.address().c_str(), (*it_m_chunk_content).second);
    	    	}
				else{
					//printf("NO_IN %d %s %d\n", m_id, data.address().c_str(), (*it_m_chunk_content).second);
					delete(data_searched);
					delete(interest);
					return;
					}
				}
    		    else
    		      {
    		    	m_chunk_content.insert(make_pair(data.address(),1));
    		      }
    	 //end modifications


	std::multimap<ssim::Time, DATA>::iterator   it_LRU;
	std::map<string, std::multimap<ssim::Time, DATA>::iterator>::iterator   it_LRU_search;
	while( ( m_cache_size+(data.size()/1000) ) > (m_cache_limit) )
	{
	  it_LRU=m_cache_LRU.begin();
	  unsigned int data_size=(*it_LRU).second.size();
	  m_cache_size-=(double)(data_size/1000);
	  string search = (*it_LRU).second.address();
	  //coding modification
	  it_m_chunk_content= m_chunk_content.find((*it_LRU).second.address());
	  (*it_m_chunk_content).second--;
	  //printf("OUT %d %s %d\n", m_id, (*it_LRU).second.address().c_str(), (*it_m_chunk_content).second);
	  if ((*it_m_chunk_content).second==0)
		  m_chunk_content.erase(it_m_chunk_content);
	  //end coding modification
	  search.append("/");
	  char chunk[100];
	  sprintf(chunk,"%d",(*it_LRU).second.chunk_id());
	  search.append(chunk);
	  m_cache_search.erase(search);
	  m_cache_LRU.erase(it_LRU);
	}

	m_cache_size+=(data.size()/1000);
	it_LRU = m_cache_LRU.insert( make_pair(Sim::clock(), data ));
	if (it_LRU == m_cache_LRU.end())
	  cout << " Severe error!" << endl;
	string insert;
	insert.append(data.address());
	insert.append("/");

	char chunk2[100];
	sprintf(chunk2,"%d",data.chunk_id());
	insert.append(chunk2);
	m_cache_search.insert(make_pair(insert,it_LRU ));
      }
      delete(data_searched);
    }
    delete(interest);
}

std::list<Packet*> CCNxNodeCoding :: send_data_interest( string addr, unsigned int received_chunk , unsigned int size, unsigned int next_size, ProcessId producer, Message msg, unsigned int port)
{
  std::list<int>             requesting_chunk;
  std::list<Packet*>         interest_packets;
  std::map<string,int>	     interest_address; //use it to send only a single interest, not duplicate interests
  INTEREST*                  interest;

  std::vector<std::multimap<std::string, SocketManager*>::iterator> erasing;
  std::multimap<std::string, SocketManager*>::iterator it;
  std::pair< std::multimap<std::string, SocketManager*>::iterator, std::multimap<std::string, SocketManager*>::iterator > range;

  range = m_socket_manager.equal_range(addr);//search for socket interested in this URI and try to use
  //cout << "Received: " << received_chunk << " port: " << port << endl;
  for (it = range.first;  it!=range.second && range.first!=m_socket_manager.end();it++) //search for sockets of this address
  {
    if (it != m_socket_manager.end())
    {
      if( (*it).second->port() == port )
      {
	std::multimap<unsigned int, unsigned int> map_match;
	//TODO adapt insert chunk to the Socket Manager structure
	if ( /*(*it).second->insert_chunk(received_chunk,size) ==*/ false )
	{
	  //TODO adapt to the SocketManager structure
/*	  requesting_chunk = (*it).second->get_interest_id();*/
	  for (std::list<int>::iterator i = requesting_chunk.begin(); i!=requesting_chunk.end(); i++)
	  {
	    //cout <<"Requesting chunk:" << (*i) << " " << addr << endl;
	    if ( (*i) != -1 )
	    {
	      interest = new INTEREST( m_id, m_id, producer, *i, addr, Sim::clock() , 1, msg, next_size, (*it).second->port(), (*it).second->popularity_class(),(*it).second->chunk_number());
	      //printf("INTEREST %d %d, %d, %s, %d\n", m_id, producer, interest->chunk_id(), interest->address().c_str(), (*it).second->port());fflush(stdout);
	      record_interest(0 ,*interest, interest->retransmission(),(*it).second->start_time(), (*it).second);
	      interest_packets.push_back(interest);
	    }
	  }
	}
	else
	{
	  if (m_socket_manager.count((*it).first)==1)
	    NOpenConnections_single--;
	  NOpenConnections--;
	  cout << Sim::clock() << "\tclosed socket " << m_id << "\t" << (*it).first <<  "\t" << (Sim::clock()-(*it).second->start_time())*m_options.sim_time_unit << "\t" << (*it).second->filesize()  << "\t" << (*it).second->VRTT()*m_options.sim_time_unit << "\tclass " << (*it).second->popularity_class() << " \t" << NOpenConnections_single<< "/" << NOpenConnections << " port:" << (*it).second->port() << endl;
	  erasing.push_back(it);
	}
	 //TODO adapt to the Socket manager structure
// 	if ( (*it).second->check_virtual_end(received_chunk,size) == true )
// 		{
// 		cout << Sim::clock() << "\tvirtual_closed socket " << m_id << "\t" << (*it).first <<  "\t" << (Sim::clock()-(*it).second->start_time())*m_options.sim_time_unit << "\t" << (*it).second->filesize()  << "\t" << (*it).second->VRTT()*m_options.sim_time_unit << "\tclass " << (*it).second->popularity_class() << " \t" << NOpenConnections_single<< "/" << NOpenConnections << " port:" << (*it).second->port() << endl;
// 		}

      }

    }
  }
  //delete
  for (unsigned int j =0; j < erasing.size() ; j++)
  {
    delete((*erasing[j]).second);
    std::string address = (*erasing[j]).first;
    address.append("/");
    char port_s[100];
    sprintf(port_s,"%d",port);
    address.append(port_s);
    m_socket_manager_port.erase(address);
    m_socket_manager.erase(erasing[j]);
  }
  return interest_packets;
}

