// -*- C++ -*-
//
//  This file is part of CBCBSIM, the Combined Broadcast Content-Based
//  (CBCB) routing simulation.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
//  USA, or send email to serl@cs.colorado.edu.
//
// $Id: MSTBroadcastLayer.cc,v 1.1 2004/03/19 17:04:47 carzanig Exp $
//
#include <sstream>
#include <stdexcept>
#include <cbnsim/logging.h>
#include "cbcbsim/MSTBroadcastLayer.h"

using namespace cbcbsim;
using namespace ssim;
using namespace std;

MSTBroadcastLayer :: MSTBroadcastLayer()
{
}

MSTBroadcastLayer :: ~MSTBroadcastLayer()
{
}

void MSTBroadcastLayer :: addLink( ProcessId from, ProcessId to, int length )
{
  m_links.insert( make_pair( length, make_pair( from, to ) ) );
}

void MSTBroadcastLayer :: addNode( ProcessId id )
{
  m_nodes.insert( id );
}

void MSTBroadcastLayer :: computePaths()
{
  map<ProcessId,ProcessId> roots;

  set<ProcessId>::const_iterator i;
  for( i = m_nodes.begin(); i != m_nodes.end(); ++i )
    {
      roots[(*i)] = (*i);
      m_mst[(*i)] = set<ProcessId>();
    }
  multimap<int,pair<ProcessId,ProcessId> >::const_iterator j;
  for( j = m_links.begin(); j != m_links.end(); ++j )
    {
      ProcessId n1 = (*j).second.first;
      ProcessId n2 = (*j).second.second;
      CBNDEBUG( "computePaths: looking at link: |" << n1 << "," << n2 << "|=" << (*j).first );
      CBNDEBUG( "computePaths: root of " << n1 << " is " << roots[n1] );
      CBNDEBUG( "computePaths: root of " << n2 << " is " << roots[n2] );
      if( roots[n1] != roots[n2] )
	{
	  CBNDEBUG( "computePaths: adding (" << n1 << "," << n2 << ") to MST" );
	  m_mst[n1].insert( n2 );
	  m_mst[n2].insert( n1 );
	  
	  map<ProcessId,ProcessId>::iterator k;
	  ProcessId from = roots[n2], to = roots[n1];
	  for( k = roots.begin(); k != roots.end(); ++k )
	    {
	      if( (*k).second == from )
		{
		  CBNDEBUG( "Setting root of " << (*k).first << " to " << to );
		  (*k).second = to;
		}
	    }
	}
    }
  // now we just need to use the MST to create the tree for each
  // node.
  for( i = m_nodes.begin(); i != m_nodes.end(); ++i )
    {
      set<ProcessId> visited;
      map<ProcessId,set<ProcessId> > children;
      map<ProcessId,ProcessId> parents;
      parents[(*i)] = (*i);
      visit( (*i), visited, children, parents );
      m_children[(*i)] = children;
      m_parents[(*i)] = parents;
    }

  /*
   * clear out the nodes and links data structures
   * since they are not used again.
   */
  m_nodes.clear();
  m_links.clear();
}

const set<ProcessId>& MSTBroadcastLayer :: getChildren( ProcessId root,
						     ProcessId node ) const
{
  const static set<ProcessId> empty;

  map<ProcessId, map<ProcessId, set<ProcessId> > >::const_iterator i;
  i = m_children.find( root );
  if( i == m_children.end() )
    {
      throw logic_error( "Unknown root node" );
    }

  map<ProcessId, set<ProcessId> >::const_iterator j;
  j = (*i).second.find( node );
  if( j == (*i).second.end() )
    {
      return empty;
    }
  return (*j).second;
}

int MSTBroadcastLayer :: getHopCount( ProcessId from, ProcessId to ) const
{
  set<ProcessId> visited;
  visited.insert( from );
  return getHopCount( from, to, visited );
}

int MSTBroadcastLayer :: getHopCount( ProcessId from, ProcessId to, set<ProcessId>& visited ) const
{
  if( from == to )
    {
      CBNDEBUG( "MSTBroadcastLayer::getHopCount: from (" << from << ") == to (" << to << ") returning 0" );
      return 0;
    }
  map<ProcessId, set<ProcessId> >::const_iterator i;
  i = m_mst.find( from );
  if( i == m_mst.end() )
    {
      throw logic_error( "Unknown from node" );
    }

  set<ProcessId>::const_iterator j;
  j = (*i).second.find( to );
  if( j == (*i).second.end() )
    {
      for( j = (*i).second.begin(); j != (*i).second.end(); ++j )
	{
	  if( visited.find( (*j) ) == visited.end() )
	    {
	      visited.insert( (*j) );
	      int ret = getHopCount( (*j), to, visited );
	      if( ret != -1 )
		{
		  return 1 + ret;
		}
	    }
	}
      return -1;
    }
  else 
    {
      return 1;
    }
}

ProcessId MSTBroadcastLayer :: getParent( ProcessId root, ProcessId node ) const
{
  map<ProcessId, map<ProcessId, ProcessId> >::const_iterator i;
  i = m_parents.find( root );
  if( i == m_parents.end() )
    {
      throw logic_error( "Unknown root node" );
    }

  map<ProcessId,ProcessId>::const_iterator j;
  j = (*i).second.find( node );
  if( j == (*i).second.end() )
    {
      return NULL_PROCESSID;
    }
  return (*j).second;
}

string MSTBroadcastLayer :: str() const
{
  ostringstream oss;
  oss << "[MSTBroadcastLayer: mst={";
  {
    map<ProcessId,set<ProcessId> >::const_iterator l;
    for( l = m_mst.begin(); l != m_mst.end(); ++l )
      {
	if( l != m_mst.begin() )
	  {
	    oss << ",";
	  }
	oss << (*l).first << ":";
	set<ProcessId>::const_iterator m;
	for( m = (*l).second.begin(); m != (*l).second.end(); ++m )
	  {
	    if( m != (*l).second.begin() )
	      {
		oss << "->";
	      }
	    oss << (*m);
	  }
      }
  }

  oss << "},children={";
  {
    map<ProcessId,map<ProcessId,set<ProcessId> > >::const_iterator k;
    for( k = m_children.begin(); k != m_children.end(); ++k )
      {
	if( k != m_children.begin() )
	  {
	    oss << ",";
	  }
	oss << (*k).first << ":";
	map<ProcessId,set<ProcessId> >::const_iterator l;
	for( l = (*k).second.begin(); l != (*k).second.end(); ++l )
	  {
	    if( l != (*k).second.begin() )
	      {
		oss << ",";
	      }
	    oss << "[" << (*l).first << "={";
	    set<ProcessId>::const_iterator m;
	    for( m = (*l).second.begin(); m != (*l).second.end(); ++m )
	      {
		if( m != (*l).second.begin() )
		  {
		    oss << ",";
		  }
		oss << (*m);
	      }
	    oss << "}]";
	  }
      }
  }

  oss << "},parents={";
  {
    map<ProcessId,map<ProcessId,ProcessId> >::const_iterator k;
    for( k = m_parents.begin(); k != m_parents.end(); ++k )
      {
	if( k != m_parents.begin() )
	  {
	    oss << ",";
	  }
	oss << (*k).first << ":";
	map<ProcessId,ProcessId>::const_iterator l;
	for( l = (*k).second.begin(); l != (*k).second.end(); ++l )
	  {
	    if( l != (*k).second.begin() )
	      {
		oss << ",";
	      }
	    oss << "(" << (*l).first << "," << (*l).second << ")";
	  }
      }
  }
  oss << "}]";
  return oss.str();
}

void MSTBroadcastLayer :: visit( ProcessId node,
			      set<ProcessId>& visited,
			      map<ProcessId,set<ProcessId> >& children,
			      map<ProcessId,ProcessId>& parents )
{
  visited.insert( node );
  const set<ProcessId>& neighbors = (*m_mst.find( node )).second;
  set<ProcessId>::const_iterator i;
  for( i = neighbors.begin(); i != neighbors.end(); ++i )
    {
      if( visited.find( (*i) ) == visited.end() )
	{
	  parents[(*i)] = node;
	  children[node].insert( (*i) );
	  visit( (*i), visited, children, parents );
	}
    }
}
