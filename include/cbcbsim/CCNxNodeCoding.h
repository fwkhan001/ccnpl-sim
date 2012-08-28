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


#include "cbcbsim/CCNxNode.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace cbcbsim;
using namespace cbnsim;
using namespace ssim;
using namespace std;

#ifndef CBCBSIM_CCNXNODECODING_H
#define CBCBSIM_CCNXNODECODING_H


namespace cbcbsim
{
  /** @brief Node class for CCNx Transport protocol with differentiated caching.
   */
  class CCNxNodeCoding : public cbcbsim::CCNxNode
  {

    private:
       std::map<std::string, unsigned int> m_chunk_content;
       unsigned int m_chunk_number;
    public:

       CCNxNodeCoding(unsigned int id, const ProtocolOptions& options, unsigned int cache_s, char cache_repl): CCNxNode(id, options,cache_s, cache_repl) { m_chunk_number = options.store_perc; } 
       void add_in_cache( const DATA& data );
       std::list<cbnsim::Packet*> send_data_interest( string, unsigned int, unsigned int, unsigned int, ssim::ProcessId, Message msg, unsigned int);

  };
};

#endif
