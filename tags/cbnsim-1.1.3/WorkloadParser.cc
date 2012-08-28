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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
//  USA, or send email to serl@cs.colorado.edu.
//
// $Id: WorkloadParser.cc,v 1.5 2005/10/17 16:11:05 hallcp Exp $
//
#include <iostream>
#include <stdlib.h>
#include "cbnsim/WorkloadParser.h"
#include "cbnsim/types.h"

using namespace cbnsim;
using namespace siena;
using namespace std;

WorkloadParser :: WorkloadParser()
  : m_event_ptr( NULL ), m_in( NULL )
{
}

WorkloadParser :: ~WorkloadParser()
{
  if( m_event_ptr != NULL )
    {
      delete m_event_ptr;
    }
  for( map<string,WorkloadEventParser*>::iterator i = m_event_parsers.begin();
       i != m_event_parsers.end(); ++i )
    {
      delete (*i).second;
    }
}

void WorkloadParser :: advance() throw ( parse_exception )
{
  if( ! m_in )
    {
      throw parse_exception( "input stream is NULL" );
    }
  (*m_in) >> m_val;
  if( ! (*m_in) )
    {
      m_tok = END;
      m_val == "";
      return;
    }

  if( m_val == "#" )
    {
      while( (*m_in) && (*m_in).get() != '\n' ) ;
      advance();
      return;
    }

  if( m_val == ";" )
    {
      m_tok = SEMICOLON;
    }
  else if( m_val == "," )
    {
      m_tok = COMMA;
    }
  else if( m_val == "&" )
    {
      m_tok = AMP;
    }
  else if( m_val == "|" )
    {
      m_tok = PIPE;
    }
  else if( m_val == "sim_length" )
    {
      m_tok = SIM_LENGTH;
    }
  else if( m_val == "time_unit" )
    {
      m_tok = TIME_UNIT;
    }
  else if( m_val == "node" )
    {
      m_tok = NODE;
    }
  else if( m_val == "link" )
    {
      m_tok = LINK;
    }
  else if( isbool() )
    {
      m_tok = BOOL;
    }
  else if( isdouble() )
    {
      m_tok = DOUBLE;
    }
  else if( isinteger() )
    {
      m_tok = INTEGER;
    }
  else if( isstring() )
    {
      m_tok = STRING;
    }
  else if( isop() )
    {
      m_tok = OP;
    }
  else if( isid() )
    {
      m_tok = ID;
    }
  else
    {
      // assume its an event name
      throw parse_exception( m_val );
    }
}

void WorkloadParser :: eat( TOK tok ) throw ( parse_exception )
{
  if( tok == m_tok )
    {
      advance();
    }
  else
    {
      throw parse_exception( string( "cannot eat token: " ) + m_val ); 
    }
}

bool WorkloadParser :: eat_bool() throw ( parse_exception )
{
  bool out;
  if( m_val == "true" )
    {
      out = true;
    }
  else if( m_val == "false" )
    {
      out = false;
    }
  else 
    {
      throw parse_exception( m_val );
    }
  eat( BOOL );
  return out;
}

string WorkloadParser :: eat_id() throw ( parse_exception )
{
  string out = m_val;
  eat( ID );
  return out;
}

int WorkloadParser :: eat_integer() throw ( parse_exception )
{
  int out = atoi( m_val.c_str() );
  eat( INTEGER );
  return out;
}

double WorkloadParser :: eat_double() throw ( parse_exception )
{
  double out = atof( m_val.c_str() );
  eat( DOUBLE );
  return out;
}

operator_id WorkloadParser :: eat_op() throw ( parse_exception )
{
  operator_id out;
  if( m_val == "=" )
    {
      out = eq_id;
    }
  else if( m_val == "!=" )
    {
      out = ne_id;
    }
  else if( m_val == "<" )
    {
      out = lt_id;
    }
  else if( m_val == ">" )
    {
      out = gt_id;
    }
  else if( m_val == "=*" )
    {
      out = pf_id;
    }
  else if( m_val == "*=" )
    {
      out = sf_id;
    }
  else if( m_val == "**" )
    {
      out = ss_id;
    }
  else
    {
      throw parse_exception( m_val );
    }
  eat( OP );
  return out;
}

string WorkloadParser :: eat_string() throw ( parse_exception )
{
  string out( m_val, 1, m_val.length() - 2 );
  eat( STRING );
  return out;
}

bool WorkloadParser :: isbool() const
{
  return m_val == "true" || m_val == "false";
}

bool WorkloadParser :: isdouble() const
{
  bool found_dot = false;
  int length = m_val.length();
  for( int i = 0; i < length; ++i )
    {
      char c = m_val[i];
      if( isdigit( c ) || 
	  (!found_dot && c == '.') ||
	  (i == 0 && c == '-') )
	{
	  if( c == '.' )
	    {
	      found_dot = true;
	    }
	}
      else
	{
	  return false;
	}
    }
  // up to here everything is digits, a leading '-'
  // and maybe a period. This is only a double if there 
  // is a period.
  return found_dot;
}

bool WorkloadParser :: isid() const
{
  int length = m_val.length();
  for( int i = 0; i < length; ++i )
    {
      char c = m_val[i];
      if( (i == 0 && !isalpha( c ) && c != '_' ) ||
	  (i > 0 && !isalnum( c ) && c != '_') )
	{
	  return false;
	}
    }
  return true;
}

bool WorkloadParser :: isinteger() const
{
  int length = m_val.length();
  for( int i = 0; i < length; ++i )
    {
      char c = m_val[i];
      if( !(isdigit( c ) || (i == 0 && c == '-') ) )
	{
	  return false;
	}
    }
  return true;
}

bool WorkloadParser :: isop() const
{
  if( m_val == "=" ||
      m_val == "!=" ||
      m_val == "<" ||
      m_val == ">" ||
      m_val == "=*" ||
      m_val == "*=" ||
      m_val == "**" )
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool WorkloadParser :: isstring() const
{
  register int length = m_val.length();
  return length > 0 && m_val[0] == '"' && m_val[length-1] == '"';
}

// unsigned int WorkloadParser :: get_link_count() const
// {
//   return m_link_set.size();
// }
// 
// unsigned int WorkloadParser :: get_link_weight( unsigned int from, unsigned int to ) const
// {
//   LinkMap::const_iterator i;
//   i = m_link_map.find( make_pair( from, to ) );
//   if( i == m_link_map.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second;
//     }
// }
// 
// double WorkloadParser :: get_link_buffer( unsigned int from, unsigned int to ) const
// {
//   LinkMap::const_iterator i;
//   i = m_link_buffer.find( make_pair( from, to ) );
//   if( i == m_link_buffer.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second;
//     }
// }
// 
// double WorkloadParser :: get_link_capacity( unsigned int from, unsigned int to ) const
// {
//   LinkMap::const_iterator i;
//   i = m_link_capacity.find( make_pair( from, to ) );
//   if( i == m_link_capacity.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second;
//     }
// }
// 
// 
// 
// unsigned int WorkloadParser :: get_node_count() const
// {
//   return m_nodes.size();
// }
// 
// unsigned int WorkloadParser :: get_node_x( unsigned int id ) const
// {
//   typedef map<unsigned int,WorkloadParser::Node>::const_iterator ITR;
//   ITR i = m_nodes.find( id );
//   if( i == m_nodes.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second.x;
//     }
// }
// 
// unsigned int WorkloadParser :: get_node_y( unsigned int id ) const
// {
//   typedef map<unsigned int,WorkloadParser::Node>::const_iterator ITR;
//   ITR i = m_nodes.find( id );
//   if( i == m_nodes.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second.y;
//     }
// }
// 
// unsigned int WorkloadParser :: get_node_cache(unsigned int id) const
// {
//   typedef map<unsigned int,WorkloadParser::Node>::const_iterator ITR;
//   ITR i = m_nodes.find( id );
//   if( i == m_nodes.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second.cache;
//     }
// }
// 
// unsigned int WorkloadParser :: get_node_replacement(unsigned int id) const
// {
//   typedef map<unsigned int,WorkloadParser::Node>::const_iterator ITR;
//   ITR i = m_nodes.find( id );
//   if( i == m_nodes.end() )
//     {
//       return 0;
//     }
//   else
//     {
//       return (*i).second.replacement;
//     }
// }

double WorkloadParser :: get_sim_length() const
{
  return m_sim_length;
}

double WorkloadParser :: get_time_unit() const
{
  return m_time_unit;
}
// 
// void WorkloadParser :: link_list() throw ( parse_exception )
// {
//   eat( LINK );
//   unsigned int from = eat_integer();
//   unsigned int to = eat_integer();
//   double delay = eat_double();
//   double buffer = ((double)eat_integer()) * 1000.0;
//   double capacity = ((double)eat_integer()) * 1000.0;
//   m_link_map.insert( make_pair( make_pair( from, to ), delay ) );
//   m_link_buffer.insert( make_pair( make_pair( from, to ), buffer ) );
//   m_link_capacity.insert( make_pair( make_pair( from, to ), capacity ) );
//   m_link_set.insert( make_pair( from, to ) );
//   switch( m_tok )
//     {
//     case COMMA:
//       eat( COMMA ); link_list();
//       break;
//     case SEMICOLON:
//       eat( SEMICOLON );
//       break;
//     default:
//       throw parse_exception( m_val );
//     }
// }

const WorkloadEvent* WorkloadParser :: next_event() throw ( parse_exception )
{
  if( m_event_ptr != NULL )
    {
      delete m_event_ptr;
      m_event_ptr = NULL;
    }
  if( m_tok == END )
    {
      return m_event_ptr;
    }
  else if( m_tok == ID )
    {
      map<string,WorkloadEventParser*>::iterator i = m_event_parsers.find( m_val );
      if( i == m_event_parsers.end() )
	{
	  // no matching event parser, just ignore.
	    cerr << "WARNING: No mathcing parser for " << m_val << "!" << endl;
	    while( m_tok != SEMICOLON )
	    {
	      eat( m_tok );
	    }
	  eat( SEMICOLON );
	  // now that its out of the way, recurse to get the next one...
	  return next_event();
	}
      else
	{
	  m_event_ptr = (*i).second->parse_event( *this );
	  return m_event_ptr;
	}
    }
  else
    {
      throw parse_exception( m_val );
    }
}
// 
// void WorkloadParser :: node_list() throw ( parse_exception )
// {
//   eat( NODE );
//   uint32_t id = eat_integer();
//   uint32_t x = eat_integer();
//   uint32_t y = eat_integer();
//   uint32_t cache = eat_integer();
//   string replacement = eat_string();
//   Node n( id, x, y ,cache,*(replacement.c_str()));
// 
//   m_nodes.insert( make_pair( n.id, n ) );
//   switch( m_tok )
//     {
//     case COMMA:
//       eat( COMMA ); node_list();
//       break;
//     case SEMICOLON:
//       eat( SEMICOLON );
//       break;
//     default:
//       throw parse_exception( m_val );
//     }
// }

void WorkloadParser :: parse_times() throw ( parse_exception )
{
  advance();
  time_unit();
  sim_length();
}

void WorkloadParser :: sim_length() throw ( parse_exception )
{
  eat( SIM_LENGTH );
  m_sim_length = eat_double();
  eat( SEMICOLON );
}

void WorkloadParser :: time_unit() throw ( parse_exception )
{
  eat( TIME_UNIT );
  m_time_unit = eat_double();
  eat( SEMICOLON );
}
