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
#include <stdlib.h>
#include "cbcbsim/Socket.h"
#include <vector>


using namespace cbcbsim;
using namespace ssim;
using namespace std;

Socket::Socket(const ProtocolOptions& options, unsigned int id, const INFOPacket &info, unsigned int port, 
       const std::set<siena::if_t>& matches) : 
       m_options(options), m_id(id), m_content_name(info.content_name()), m_msg( info.msg() ), 
       m_tot_filesize(info.content_size()), m_chunk_size(info.data_chunk_size()), m_port( port ), 
       m_popularity_class( info.popularity_class() ) 
{
  m_start_time = Sim::clock();
  received = 0;
  m_VRTT = 0;
  m_delay = 0;
  m_filesize = 0;
  /** @brief initialization process for the structures that identify a content download
  */ 
  for (unsigned int i=0;i<=m_tot_filesize; i++)
  {
    chunk_info new_chunk;
    new_chunk.received = false;
    new_chunk.chunk_request = 0;
    new_chunk.chunk_timeout = 0;
    new_chunk.chunk_RTT = 0;
    new_chunk.chunk_retrieving_time = 0;
    m_file_chunks.push_back(new_chunk);
    m_notreq_chunks.insert(i);
    m_missing_chunks.insert(i);
  }
}

void Socket :: remove_notreq(unsigned int chunk_id)
{
  m_notreq_chunks.erase(chunk_id);
}

int Socket :: select_chunk()
{
  int selected_chunk = -1;
  int pos = 0;
  if(m_notreq_chunks.size() == 0)
    return selected_chunk;
  while(m_notreq_chunks.size()!=0)
  {
    set<int>::iterator it = m_notreq_chunks.begin();
    if (m_options.request_order == ProtocolOptions::RANDOM) 
    {
      pos=rand() % m_notreq_chunks.size();
      advance(it,pos);
    }
    else
      it = m_notreq_chunks.begin();
      
    if ((m_file_chunks[(*it)].received == false) && 
        (m_file_chunks[(*it)].chunk_request<m_options.to_limit))
    {
      selected_chunk = (*it);
/*      m_notreq_chunks.erase(it);*/
      return selected_chunk;
    }
    /** @brief in the missing chunks there is a chunk already received, remove it!
      */
    m_notreq_chunks.erase(it);
  }
  return selected_chunk;
}

void Socket :: requested_chunk(unsigned int chunk_id)
{
  if(m_file_chunks[chunk_id].chunk_request == 0)
    m_file_chunks[chunk_id].chunk_retrieving_time = Sim::clock();
  m_file_chunks[chunk_id].chunk_RTT = Sim::clock();
  m_file_chunks[chunk_id].chunk_request++;  
}