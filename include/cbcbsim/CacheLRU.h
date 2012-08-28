// -*- C++ -*-
//
//  This file is part of CBCBSIM, the Combined Broadcast Content-Based
//  (CBCB) routing simulation.
//
//  Authors: See the file AUTHORS for full details. 
//
//  Copyright (C) 2002-2004 University of Colorado
//            (C) 2009-2010 France Telecom R&D
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
// $Id: SocketManager.h, 2010-02 10:55:50 Gallo Exp $

#include <stdio.h>
#include <cbcbsim/cbcbsim_types.h>
#include <cbcbsim/ProtocolOptions.h>
#include <cbcbsim/Cache.h>
#include <iostream>


#ifndef CBCBSIM_CACHE_LRU_H
#define CBCBSIM_CACHE_LRU_H


namespace cbcbsim
{
  class time_LRU_record;

  /** @brief cache structure ordered by time, used to evict from cache Least Recently Used chunk/s
   *  NOTICE that the order is maintained because a new element is always inserted at the end of the list,
   *         so that the list has an increasing order w.r.t. the insertion time (see class time_LRU_record )
   */
  typedef std::list<time_LRU_record> store_by_time_LRU;
 
  /** @brief cache structure ordered by content (not chunk) name in order to perform a faster research
   * 	     first:  content name
   * 	     second: cache_bitmap, containing iterators to the store_by_time_LRU structure
   *                 if the chunk is stored, it has a valid iterator to the store_by_time_LRU structure
   *                 if not it has an iterator to the end of the store_by_time_LRU structure
   */
  typedef boost::unordered_map<std::string,content_record<store_by_time_LRU::iterator>::type > store_by_name_LRU;
  
    
  /** @brief record of the store_by_time_LRU structure
   */
  class time_LRU_record{
  public:
    /** @brief time at which the stored chuk was most recently used
     */
    ssim::Time			insertion_time;
    /** @brief data packet
     */
    DATAPacket			data_packet;
    /** @brief iterator to the store_by_name_LRU structure
     */
    store_by_name_LRU::iterator store_by_name_LRU_it;
    
    time_LRU_record(ssim::Time time,DATAPacket data,store_by_name_LRU::iterator it)
    : insertion_time(time),data_packet(data),store_by_name_LRU_it(it){};
  };
  
  /** @brief Class for caching with Least Recently Used replacement policy. 
   *  the LRU cache has 2 data structures one ordered by chunk insertion time and an hash table with 
   *  key = content name. Each data structure points to the other one for fast searching and deleting.
   */ 
  class CacheLRU : public Cache
  {
  
  private:
    store_by_time_LRU 		m_store_by_time_LRU;
    store_by_name_LRU		m_store_by_name_LRU;
    /** @brief iterator that points the record of the most recenly searched content name
     *  used to avoid multiple search in  the store_by_name_LRU structure
     */
    store_by_name_LRU::iterator content_bitmap_search; 

   
  public:
    CacheLRU(ssim::ProcessId id, unsigned int cache_limit, unsigned int minimum_chunk_size):Cache(id,cache_limit)
    {
      m_cache_size=0;
      m_store_by_name_LRU.rehash(cache_limit/minimum_chunk_size);
    }

    int add_in_cache( const DATAPacket data );

    DATAPacket* search_by_name( const INTERESTPacket interest );

  };
};
#endif