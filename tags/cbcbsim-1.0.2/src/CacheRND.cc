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
#include "cbcbsim/CacheRND.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>


using namespace cbcbsim;
using namespace ssim;
using namespace std;

DATAPacket* CacheRND::search_by_name( const INTERESTPacket interest )
{
  store_by_chunk_RND::iterator  it_by_chunk;
  store_by_name_RND::iterator it_by_name;
  DATAPacket* cached_chunk=NULL; 
  content_bitmap_search = m_store_by_name_RND.end();
  it_by_name=m_store_by_name_RND.find(interest.content_name());
  if (it_by_name != m_store_by_name_RND.end())
  {
    it_by_chunk = it_by_name->second.chunk_bitmap[interest.chunk_id()];
    if( it_by_chunk != m_store_by_chunk_RND.end() )
    {
      cached_chunk = new DATAPacket( m_id, interest);
    }
  }
  content_bitmap_search = it_by_name;
  return cached_chunk;
}

int CacheRND :: add_in_cache( const DATAPacket data )
{
  if (m_cache_limit==0)
     return 0;
  DATAPacket* data_searched;
  const INTERESTPacket aux_interest = INTERESTPacket(0, data.content_name(), data.chunk_id(),0,0,data.msg(),data.size(),0,0,0);
  data_searched = search_by_name(aux_interest);
  
  if(data_searched!=NULL)
  {
    delete(data_searched);
    data_searched=NULL;
    return 0;
  }
  
  while( ( m_cache_size+(data.size()/1000) ) > (m_cache_limit) )
  { 
    store_by_chunk_RND::iterator it_chunk_del = m_store_by_chunk_RND.begin();
    advance(it_chunk_del,rand() % m_store_by_chunk_RND.size()); 
    store_by_name_RND::iterator it_name_del = it_chunk_del->store_by_name_RND_it;
    m_cache_size-=it_chunk_del->data_packet.size()/1000;
    //if the number of recorded chunks under this given name is one, delete the entry in the structure
    if((it_name_del->second.stored_chunk == 1) && 
       (it_chunk_del->data_packet.content_name() != data.content_name())) 
      m_store_by_name_RND.erase(it_name_del);
    else //if not, simply delete the iterator to the DATA from the structure
    {
      it_name_del->second.stored_chunk--;
      it_name_del->second.chunk_bitmap[it_chunk_del->data_packet.chunk_id()] = m_store_by_chunk_RND.end();
    }
    m_store_by_chunk_RND.erase(it_chunk_del);
  }
  if(content_bitmap_search == m_store_by_name_RND.end())
  {
    //insert new record for this file with all data null and the data to be inserted 
    vector< store_by_chunk_RND::iterator > vect(data.content_size()+1,m_store_by_chunk_RND.end());
    content_record<store_by_chunk_RND::iterator>::type new_name_record;
    new_name_record.stored_chunk = 0;
    new_name_record.chunk_bitmap = vect;
    pair<store_by_name_RND::iterator, bool> inserted;
    inserted = m_store_by_name_RND.insert(make_pair(data.content_name(),new_name_record));
    if (inserted.second)
      content_bitmap_search = inserted.first;
    else
      std::cout << "Error in writing in memory" << std::endl;
  }
  chunk_RND_record new_chunk_record = chunk_RND_record(data,content_bitmap_search);
  m_store_by_chunk_RND.push_back(new_chunk_record);
  store_by_chunk_RND::iterator it_by_chunk = --m_store_by_chunk_RND.end();
  m_cache_size+=(data.size()/1000);
  content_bitmap_search->second.stored_chunk++;
  content_bitmap_search->second.chunk_bitmap[data.chunk_id()] = it_by_chunk;	
  return 1;
}