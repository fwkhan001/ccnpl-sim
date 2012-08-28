// -*- C++ -*-
//
//  This file is part of CBNSIM, the Content-Based Networking
//  Simulation Library.
//
//  Authors: See the file AUTHORS for full details.
//
//  Copyright (C) 2002-2004 University of Colorado
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
//
// $Id: TopologyParser.h,v 1 2012/06/18 16:00:00 gallo Exp $

#ifndef CBNSIM_TOPOLOGYPARSER_H
#define CBNSIM_TOPOLOGYPARSER_H

#include <map>
#include <set>
#include <istream>
#include <string>
#include "cbnsim/parse_exception.h"


namespace cbnsim
{
  /** @brief Class to parse the .brite topology
   */
  class TopologyParser
  {
  private:

    class Node{
      public:
	unsigned int id, x, y,cache;
	char replacement;
	Node( unsigned int p_id, unsigned int p_x, unsigned int p_y, unsigned int p_cache, char p_replacement )
	  : id( p_id ), x( p_x ), y( p_y ), cache( p_cache ), replacement( p_replacement ) {}
    };
      
    class TLink{
      public:
	unsigned int from, to;
	double	prop_delay,buffer_size,capacity;

	TLink( unsigned int id_from, unsigned int id_to, double pdelay, double bsize, double c )
	  : from( id_from ), to( id_to ), prop_delay ( pdelay ), buffer_size( bsize ), capacity( c ) {}
    };
    
    typedef std::pair<unsigned int,unsigned int> Link;
    typedef std::map<Link,TLink> LinkMap;
    typedef std::set<Link> LinkSet;
    typedef std::map<unsigned int,Node> NodeMap;
    
    
    std::string			m_topo_fname;
    LinkMap			m_link_map;
    LinkSet			m_link_set;
    NodeMap			m_nodes;
      
  public:
    TopologyParser(){};
          
    int 			parse( std::string input );
    
    void			node_list() throw ( parse_exception );
    
    void			link_list() throw ( parse_exception );
    
    const std::set< std::pair<unsigned int, unsigned int> >& get_links() const
    { return m_link_set; }
    
    double			get_link_delay( unsigned int from, unsigned int to ) const;
  
    double			get_link_buffer( unsigned int from, unsigned int to ) const;
    
    double			get_link_capacity( unsigned int from, unsigned int to ) const;
       
    unsigned int		get_link_count() const;

    unsigned int		get_node_count() const;

    unsigned int		get_node_x( unsigned int id ) const;

    unsigned int		get_node_y( unsigned int id ) const;
    
    unsigned int		get_node_cache( unsigned int id ) const;
    
    unsigned int		get_node_replacement( unsigned int id ) const;
    
  };
};
#endif