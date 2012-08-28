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

#include "cbcbsim/PITable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



using namespace cbcbsim;
using namespace std;
bool PITable :: record_interest( unsigned int ifid,const INTERESTPacket interest, Time start_time, 
				  Socket* socket)
{
  bool PIT_match = false;
  bool record_pit_entry = true; 
  ssim::Time expiration_time = Sim::clock() + m_timer;
  
  pair< store_by_name_PIT::iterator, store_by_name_PIT::iterator > range;
  range = lookup(interest.content_name(),interest.chunk_id());
  
  /** iterate on matching PIT entries and decides wether or not recording the interest in the PIT,
   *  based on filtering parameter, receiving interface id and chunk_id (0 indicates the preliminar INTEREST packet)
   */
  for (store_by_name_PIT::iterator it=range.first; it!=range.second && range.first!=m_store_by_name_PIT.end(); ++it)
  {
    if  ( ((*it).second.chunk_id() != 0 ) && 
	  ( m_filtering == ProtocolOptions::FILTERING_ON ) && 
	  (ifid != 0) )
    {
      expiration_time = (*it).second.time(); //if there is a filtering event record new pit entry 
      PIT_match = true;
      break;
    }
    
    if ((ifid == 0)&&(socket != NULL))
      if ((*it).second.port() == socket->port())
      {
	record_pit_entry = false;
	PIT_match = true;
      }
  }
  
  if (record_pit_entry)
  {
    stringstream str_chunk_id;  
    str_chunk_id << interest.chunk_id();
    string  chunk_name = interest.content_name() + "/" + str_chunk_id.str();
    store_by_name_PIT::iterator it_by_name;
    store_by_time_PIT::iterator it_by_time;
    
    it_by_name=m_store_by_name_PIT.insert(make_pair(chunk_name, PIT_entry(ifid, expiration_time, interest, it_by_time)));

    it_by_time = m_store_by_time_PIT.insert(make_pair(expiration_time,it_by_name));  
    //(*it_by_name).second = PIT_entry(ifid, expiration_time, interest, it_by_time);
    it_by_name->second.set_store_by_time_it(it_by_time);
    
  }
  return PIT_match;
}

pair< store_by_name_PIT::iterator, store_by_name_PIT::iterator > PITable :: lookup(cname content_name, unsigned int chunk_id)
{  
  stringstream str_chunk_id;  
  str_chunk_id << chunk_id;
  string  chunk_name = content_name + "/" + str_chunk_id.str();
  PIT_clean();
  return m_store_by_name_PIT.equal_range(chunk_name);
}

Time PITable::registration_time(unsigned int ifid,DATAPacket data)
{
  pair<store_by_name_PIT::iterator, store_by_name_PIT::iterator> range;
  
  range = lookup(data.content_name(),data.chunk_id());
  
  store_by_name_PIT::iterator it=range.first;
  
  Time reg_time=Sim::clock();
  
  while((it!=range.second)&&(range.first!=m_store_by_name_PIT.end()))
  { 
    if ( (*it).second.ifid() == ifid || (*it).second.ifid() == 0 )  
    {
      it++;
      continue;
    }
    
    if((*it).second.interest_issue_time()<reg_time){
      reg_time=(*it).second.interest_issue_time();
    }
    it++;
  }
  return reg_time;
}

match PITable :: process_out( unsigned int ifid,DATAPacket data)
{
  /** @brief look up in PIT used when receive data and need to fwd it
   *  consume PIT entries if needed
   */
  pair<store_by_name_PIT::iterator, store_by_name_PIT::iterator> range;
  match match_out;

  range = lookup(data.content_name(),data.chunk_id());
  
  
  store_by_name_PIT::iterator it=range.first;
  
  while((it!=range.second)&&(range.first!=m_store_by_name_PIT.end()))
  { 
    if ( (*it).second.ifid() == ifid )  
    {
      it++;
      continue;
    }
    
    /** if PIT entry not for same interf from witch data (or research) is arriving
     */
    if ((data.chunk_id() == 0) || ( m_filtering == ProtocolOptions::FILTERING_OFF )) 
    {
      if (((*it).second.interest_issue_time() == data.interest_issue_time())&&(data.port() == (*it).second.port())) 
      {
	consume_PIT_entry(it++,match_out);
	/** We will consume only a single entry. break*/
	break;
      }
      it++;
      continue;
    }
    
    if ( m_filtering == ProtocolOptions::FILTERING_ON ) 
    {
      if ( ((*it).second.ifid() == 0) ) 	
      { 
	/** received DATA is for the local process
	 */
	if ((*it).second.port() == data.port()) 
	{   
	  consume_PIT_entry(it++, match_out);
	  /** We will consume only a single entry. break*/
	  break;
	}
	it++;
	continue;
      }
      consume_PIT_entry(it++,match_out);
      continue;
    }
  }
  return match_out;
}

void PITable :: consume_PIT_entry(store_by_name_PIT::iterator it,match &match_out )
{
  matching_if match = {(*it).second.ifid(),(*it).second.port()};
  match_out.push_back(match);
  m_store_by_time_PIT.erase((*it).second.store_by_time_it());
  //delete((*it).second);
  m_store_by_name_PIT.erase(it);
}

int PITable :: PIT_clean( )
{
  store_by_time_PIT::iterator it = m_store_by_time_PIT.begin();
/*  cout << " PIT size :"<<  m_store_by_name_PIT.size()  << "," << m_store_by_time_PIT.size() << endl; */
  while((it != m_store_by_time_PIT.end())&&(Sim::clock()>=(*it).first))
  {
    store_by_name_PIT::iterator it_name = it->second;
    //delete(it_name->second);
    m_store_by_name_PIT.erase(it_name);
    m_store_by_time_PIT.erase(it++);
  }
  return 1;
}