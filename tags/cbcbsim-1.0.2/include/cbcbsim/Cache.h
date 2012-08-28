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

#include <stdio.h>
#include <cbcbsim/cbcbsim_types.h>
#include <cbcbsim/ProtocolOptions.h>
#include <cbcbsim/CCNxPackets.h>

#ifndef CBCBSIM_CACHE_H
#define CBCBSIM_CACHE_H

using namespace cbcbsim;

namespace cbcbsim
{
  /** @brief General class for caching.
   */ 
  class Cache
  { 
  protected:
    ssim::ProcessId		m_id;
    unsigned int		m_cache_size; //current size of the cache
    unsigned int 		m_cache_limit; //maximum size of the cache
   
  public:
    Cache(unsigned int id,unsigned int cache_limit) :
    m_id(id), m_cache_limit(cache_limit)
    {}
    
    /** @brief search a chunk given its chunk name (content_name+chunk_id)
     */ 
    virtual DATAPacket* search_by_name( const INTERESTPacket interest ){printf("cache father's search\n"); return NULL;}
    
    /** @brief add a chunk to the cache, evicting according to a given POLICY
     */
    virtual int add_in_cache( const DATAPacket data ){printf("cache father's search\n");return 0;}
  };
};
#endif
