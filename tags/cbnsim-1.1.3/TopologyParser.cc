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
// $Id: TopologyParser.cc,v 1 2012/06/18 16:00:00 gallo Exp $

#include "cbnsim/TopologyParser.h"
#include <fstream>
#include <stdlib.h>
#include <list>
#include <stdio.h>
#include <iostream>

using namespace cbnsim;
using namespace std;

extern FILE* yyin;
int yyparse( void );


typedef struct {
    int		x,y,type,cache_size;
    char	cache_replacement;
} bnode;

typedef struct {
    double	buffer_size,prop_delay,capacity;
} blink;  

extern map<int,bnode>topo_node;
extern map<pair<int,int>,blink > topo_link;
extern map<int, set<int> > topo_adjacency_list;



int TopologyParser :: parse( std::string input) 
{  
    /*
     * read in the topology info
     */
    
    m_topo_fname = input;
    
    uint topo_node_count = 0;
    {
	yyin = fopen( m_topo_fname.c_str(), "r" );
	if( !yyin )
		std::cout << "ERROR: unable to open topology file '" << m_topo_fname << "'" << endl;
	do{
	  yyparse();
	}
	while( !feof( yyin ) );
	fclose( yyin );
	topo_node_count = topo_adjacency_list.size();
    }
     
    //srand( m_seed );
    node_list();
    link_list();
    return 0;
}

void TopologyParser :: node_list() throw ( parse_exception )
{
  map<int,bnode >::const_iterator ci;
  for( ci = topo_node.begin(); ci != topo_node.end(); ++ci ){
    int id =(*ci).first;
    int x = (*ci).second.x;
    int y = (*ci).second.y;
    int cache = (*ci).second.cache_size;
    char replacement = (*ci).second.cache_replacement;
    Node n( id, x, y ,cache,replacement);
    m_nodes.insert( make_pair( n.id, n ) );
  }
}

void TopologyParser :: link_list() throw ( parse_exception )
{
   /*
    * calculate the multiplier needed to bring topology timings into
    * the same time unit as the workload. This is used to scale the
    * link delays.
    */
  map<pair<int,int>,blink >::const_iterator li;
  list<double>::const_iterator it_list;
  
  for( li = topo_link.begin(); li != topo_link.end(); ++li ){
    unsigned int from = (*li).first.first;
    unsigned int to = (*li).first.second;
    double delay = (*li).second.prop_delay; //TODO(convert to the correct scale)
    double buffer = ((*li).second.buffer_size) * 1000.0;
    double capacity = ((*li).second.capacity) * 1000.0;
    TLink l(from,to,delay,buffer,capacity);
    m_link_map.insert( make_pair( make_pair( from, to ), l ) );
    m_link_set.insert( make_pair( from, to ) );
  }
}

unsigned int TopologyParser :: get_link_count() const
{
  return m_link_set.size();
}

double TopologyParser :: get_link_delay( unsigned int from, unsigned int to ) const
{
  LinkMap::const_iterator i;
  i = m_link_map.find( make_pair( from, to ) );
  if( i == m_link_map.end() )
      return 0;
  return (*i).second.prop_delay;
}

double TopologyParser :: get_link_buffer( unsigned int from, unsigned int to ) const
{
  LinkMap::const_iterator i;
  i = m_link_map.find( make_pair( from, to ) );
  if( i == m_link_map.end() )
      return 0;
  return (*i).second.buffer_size;
}

double TopologyParser :: get_link_capacity( unsigned int from, unsigned int to ) const
{
  LinkMap::const_iterator i;
  i = m_link_map.find( make_pair( from, to ) );
  if( i == m_link_map.end() )
      return 0;
  return (*i).second.capacity;
}



unsigned int TopologyParser :: get_node_count() const
{
  return m_nodes.size();
}

unsigned int TopologyParser :: get_node_x( unsigned int id ) const
{
  typedef map<unsigned int,TopologyParser::Node>::const_iterator ITR;
  ITR i = m_nodes.find( id );
  if( i == m_nodes.end() )
    {
      return 0;
    }
  else
    {
      return (*i).second.x;
    }
}

unsigned int TopologyParser :: get_node_y( unsigned int id ) const
{
  typedef map<unsigned int,TopologyParser::Node>::const_iterator ITR;
  ITR i = m_nodes.find( id );
  if( i == m_nodes.end() )
    {
      return 0;
    }
  else
    {
      return (*i).second.y;
    }
}

unsigned int TopologyParser :: get_node_cache(unsigned int id) const
{
  typedef map<unsigned int,TopologyParser::Node>::const_iterator ITR;
  ITR i = m_nodes.find( id );
  if( i == m_nodes.end() )
    {
      return 0;
    }
  else
    {
      return (*i).second.cache;
    }
}

unsigned int TopologyParser :: get_node_replacement(unsigned int id) const
{
  typedef map<unsigned int,TopologyParser::Node>::const_iterator ITR;
  ITR i = m_nodes.find( id );
  if( i == m_nodes.end() )
    {
      return 0;
    }
  else
    {
      return (*i).second.replacement;
    }
}