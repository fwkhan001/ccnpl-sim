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
#include <cbcbsim/CacheLRU.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


using namespace cbcbsim;
using namespace ssim;
using namespace std;


DATAPacket* CacheLRU::search_by_name( const INTEREST interest )
{
  store_by_time_LRU::iterator  it_by_time;
  store_by_name_LRU::iterator  it_by_name;
  DATAPacket*  cached_chunk=NULL; 
  string search = interest.content_name();
  content_bitmap_search = m_store_by_name_LRU.end();

  it_by_name=m_store_by_name_LRU.find(interest.content_name());
  if (it_by_name != m_store_by_name_LRU.end())
  {
    it_by_time = (*it_by_name).second.chunk_bitmap[interest.chunk_id()];
    if( it_by_time != m_store_by_time_LRU.end() )
    {
      cached_chunk = new DATAPacket( m_id,interest);
      m_store_by_time_LRU.erase(it_by_time);
      time_LRU_record new_time_LRU_record = time_LRU_record(Sim::clock(),(*cached_chunk),it_by_name);
      m_store_by_time_LRU.push_back(new_time_LRU_record);
      (*it_by_name).second.chunk_bitmap[interest.chunk_id()] = --m_store_by_time_LRU.end();
    }
  }   
  content_bitmap_search = it_by_name;
  return cached_chunk;
}

int CacheLRU :: add_in_cache( const DATAPacket data )
{
  if (m_cache_limit==0)
     return 0;
  DATAPacket* data_searched;
  const INTEREST aux_interest = INTEREST(0, data.content_name(), data.chunk_id(),0,0,data.msg(),data.size(),0,0,0);
  data_searched = search_by_name(aux_interest);
  if(data_searched!=NULL)
  {
    delete(data_searched);
    data_searched=NULL;
    return 0;
  }
  
  while( ( m_cache_size+(data.size()/1000) ) > (m_cache_limit) )
  { 
    store_by_time_LRU::iterator  it_by_time;
    it_by_time=m_store_by_time_LRU.begin();
    unsigned int data_size = (*it_by_time).data_packet.size();
    unsigned int chunk_id= (*it_by_time).data_packet.chunk_id();
    m_cache_size-=(data_size/1000);
    store_by_name_LRU::iterator it_search_new_del=(*it_by_time).store_by_name_LRU_it; 
    unsigned int recorded_chunks = (*it_search_new_del).second.stored_chunk;
    /** if the number of recorded chunks under this given name is one, delete the entry in the structure
      */
    if((recorded_chunks == 1) && ((*it_by_time).data_packet.content_name() != data.content_name())) 
      m_store_by_name_LRU.erase(it_search_new_del);
    else 
    {
      /**if not, simply delete the iterator to the DATA from the structure
	*/
      (*it_search_new_del).second.stored_chunk--;
      (*it_search_new_del).second.chunk_bitmap[chunk_id] = m_store_by_time_LRU.end();
    }
    m_store_by_time_LRU.erase(it_by_time);
  }

  if(content_bitmap_search == m_store_by_name_LRU.end())
  {
    /** insert new record for this file with all data null and the data to be inserted 
      */
    std::vector< store_by_time_LRU::iterator > vect(data.content_size()+1,m_store_by_time_LRU.end());     
    content_record<store_by_time_LRU::iterator>::type new_content_record;
    new_content_record.stored_chunk = 0;
    new_content_record.chunk_bitmap = vect;
    std::pair<store_by_name_LRU::iterator, bool> inserted_by_name;
    inserted_by_name = m_store_by_name_LRU.insert(make_pair(data.content_name(),new_content_record));
    if (inserted_by_name.second)
      content_bitmap_search = inserted_by_name.first;	    
    else
      cout << "Error in writing in memory" << std::endl;
  }
  time_LRU_record new_time_LRU_record = time_LRU_record(Sim::clock(), data, content_bitmap_search);
  m_store_by_time_LRU.push_back(new_time_LRU_record);
  store_by_time_LRU::iterator it_LRU = --m_store_by_time_LRU.end();
  m_cache_size+=(data.size()/1000);
  (*content_bitmap_search).second.stored_chunk++;
  (*content_bitmap_search).second.chunk_bitmap[data.chunk_id()] = it_LRU;  
  return 1;
}