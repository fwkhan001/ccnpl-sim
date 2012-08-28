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

#ifndef CBCBSIM_TYPES_H
#define CBCBSIM_TYPES_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <list>
#include <vector>
#include "cbcbsim/ServedContent.h"
#include <boost/unordered_map.hpp>
#include <cbnsim/Packet.h>


using namespace std;
using namespace boost;
using namespace cbnsim;

namespace cbcbsim
{
  typedef string cname; 

  /** @brief pair that represent the LRU cache bitmap
   *         stored_chunk:  number of chunks stored in the bitmap
   *         chunk_bitmap: vector that contains pointers (iterators) to the LRU/RND map. 
   *                       If chunk not cached, iterator is an iterator to the end of the LRU map.
   */
  template <class T>
     struct content_record {
         typedef struct{ 
	   unsigned int stored_chunk; 
	   vector< T > chunk_bitmap;
	} type;
   };
  
  /** @brief Map that stores routing information loaded using an input file.
   *         first:  pair of nodes source->dest
   *         second: vector of predicates associated to source->dest
   */
  typedef map< pair<unsigned int, unsigned int>, vector<Predicate > >           routing_map;
	    
  typedef struct matching_if {
    unsigned int	ifid;   
    unsigned int	port;
  } matching_if;
  
  /** @brief type for collecting output of a PIT lookup.
   *         first:  interface
   *         second: port (2 socket for the same content has different port numbers)
   */
  typedef list<matching_if>   match;
    
  /** @brief list of pairs that contain interface to which propagate the packet and the Packet itself
   *         first:  interface
   *         second: Packet
   */
  typedef list<pair<unsigned int,Packet*> >  outgoing_packets;

  /** @brief maps that contains all the contents served by this node
   *         first:  content name
   *         second: Information about the content
   */  
  typedef unordered_map<string, ServedContent*> served_contents;
     
  
  /** @brief Structure to indicate intersts that need to be sent out
   * 	       first:  interface to which send interest
   * 	       second: chunk id of the data interested
   */ 
  typedef struct requesting_chunk {
    unsigned int	ifid;   
    unsigned int	chunk_id;
  } requesting_chunk;
  
  /** @brief Structure to keep statistics while forwarding interest 
   * 	     pred:  predicate forwarded
   * 	     fwd_interest:interest pending in a given interface 
   * 	     mov_win_fwd_interest: moving average associated to a given interface 
   * 	     if_weight: weight associated to a give interface 
   * 	     tot_fwd_interest:number of total interest forwarded with the fiven predicate
   * 	     tot_weight:
   */ 
  typedef struct fwd_info{
    Predicate 					pred;
    map<unsigned int, unsigned int> 		fwd_interest;
    map<unsigned int,double> 			if_weight; 
    unsigned int 				tot_fwd_interest; 
    double 					tot_weight; 
    } fwd_info;
    
    

};

#endif