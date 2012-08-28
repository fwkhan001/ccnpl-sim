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
#include <cbcbsim/cbcbsim_types.h>
#include <cbcbsim/ProtocolOptions.h>
#include <cbcbsim/Cache.h>
#include <iostream>


#ifndef CBCBSIM_CACHE_RND_H
#define CBCBSIM_CACHE_RND_H

using namespace cbcbsim;

namespace cbcbsim
{  
  class chunk_RND_record;
  
  /** @brief cache structure that contains an entry for each stored chunk 
   *  (see chunk_RND_record)
   */
  typedef list<chunk_RND_record> store_by_chunk_RND;

  /** @brief cache structure ordered by content (not chunk) name in order to perform a faster research
   * 	     first:  content name
   * 	     second: cache_bitmap, containing indications on each chunk of the given content
   */
  typedef boost::unordered_map<std::string,content_record<store_by_chunk_RND::iterator>::type > store_by_name_RND;
  
   /** @brief record of the store_by_chunk_RND structure
   */
  class chunk_RND_record{
  public:
    /** @brief data packet
     */
    DATAPacket			data_packet;
    /** @brief iterator to the store_by_name_RND structure
     */
    store_by_name_RND::iterator store_by_name_RND_it;
    
    chunk_RND_record(DATAPacket data,store_by_name_RND::iterator it)
    : data_packet(data),store_by_name_RND_it(it){};
  };
  
  /** @brief Class for caching with RaNDom (RND) replacement policy.
   *  the RND cache has 2 data structures one hash table with key = content_name and one vector
   *  that contains an entry for each stored chunk (used for randomize the deletion of a chunk).
   *  Each data structure points to the other one for fast searching and deleting.
   */ 
  class CacheRND : public Cache
  { 
  private:
    store_by_chunk_RND 		m_store_by_chunk_RND;
    store_by_name_RND		m_store_by_name_RND;
    /** @brief iterator that points the record of the most recenly searched content name
     */
    store_by_name_RND::iterator content_bitmap_search; 

  public:
    CacheRND(ssim::ProcessId id,unsigned int cache_limit,unsigned int minimum_chunk_size):Cache(id,cache_limit)
    {
      m_cache_size=0;
      m_store_by_name_RND.rehash(cache_limit/minimum_chunk_size);
    }
    
    int add_in_cache( const DATAPacket data );

    DATAPacket* search_by_name( const INTERESTPacket interest );
  
  };
};
#endif
