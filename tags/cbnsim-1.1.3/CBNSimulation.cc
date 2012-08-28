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
// $Id: CBNSimulation.cc,v 1.10 2005/10/17 16:11:05 hallcp Exp $
//
#include <cmath> // for rint()

#include <fstream>
#include <iostream>
#include "cbnsim/CBNSimulation.h"
#include "cbnsim/KillerProcess.h"
#include "cbnsim/io_exception.h"
#include "cbnsim/logging.h"
#include <stdlib.h>
#include <string.h>


using namespace cbnsim;
using namespace ssimp;
using namespace std;

CBNSimulation :: CBNSimulation()
  : m_data_collector( NULL ), m_driver( NULL ), m_node_factory( NULL ), m_routing_fname (""), m_sim_length ( 0 ){}

CBNSimulation :: ~CBNSimulation(){
    delete m_data_collector;
    delete m_driver;
    delete m_node_factory;
}

void CBNSimulation :: debug_messages( bool debug_on )
{
  cbnsim::g_debug_on = debug_on;
}

bool CBNSimulation :: debug_messages() const
{
  return cbnsim::g_debug_on;
}

void CBNSimulation :: error_messages( bool error_on )
{
  cbnsim::g_error_on = error_on;
}

bool CBNSimulation :: error_messages() const
{
  return cbnsim::g_error_on;
}

bool CBNSimulation :: info_messages() const
{
  return cbnsim::g_info_on;
}

void CBNSimulation :: wl_fname( const string& ifname )
{
  m_wl_fname = ifname;
}

const string& CBNSimulation :: wl_fname() const
{
  return m_wl_fname;
}

void CBNSimulation :: routing_fname( std::string routing_fname )
{
  
  m_routing_fname = routing_fname;
}

string CBNSimulation :: routing_fname( )
{
  return m_routing_fname;
}

void CBNSimulation :: topo_fname( std::string topo_fname )
{
  
  m_topo_fname = topo_fname;
}

string CBNSimulation :: topo_fname( )
{
  return m_topo_fname;
}


void CBNSimulation :: output_fname( const string& ofname )
{
  m_output_fname = ofname;
}

const string& CBNSimulation :: output_fname() const
{
  return m_output_fname;
}

void CBNSimulation :: output_int_fname( const string& ofname )
{
  m_output_int_fname = ofname;
}

const string& CBNSimulation :: output_int_fname() const
{
  return m_output_int_fname;
}

void CBNSimulation :: run() throw ( exception )
{
  CBNASSERT( m_node_factory,
	     "node_factory cannot be NULL" );
  /*
   * deal with the workload file.
   */
  ifstream ifs;
  if( m_wl_fname.size() == 0 )
    {
      m_wl_parser.set_istream( cin );
    }
  else
    {
      ifs.open( m_wl_fname.c_str() );
      if( ! ifs )
	{
	  throw io_exception( string( "unable to open: " ) + m_wl_fname );
	}
      m_wl_parser.set_istream( ifs );
    }
  /*
   * deal with the topology file.
   */
  m_topo_parser.parse(m_topo_fname);
  /*
   * now deal with the output file
   */
  ostream* p_out = &cout;
  ofstream ofs;
  if( m_output_fname.size() )
    {
      ofs.open( m_output_fname.c_str() );
      if( ! ofs )
	{
	  throw io_exception( string( "unable to open: " ) + m_output_fname );
	}
      p_out = &ofs;
    }
    ostream* p_out_int = &cout;
    ofstream ofs_int;
    if( m_output_int_fname.size() )
    {
      ofs_int.open( m_output_int_fname.c_str() );
      if( ! ofs_int )
	{
	  throw io_exception( string( "unable to open: " ) + m_output_int_fname );
	}
      p_out_int = &ofs_int;
    }
  /*
   * parse times written in the wl file
   */
  m_wl_parser.parse_times();
  /*
   * set up the topology
   */
  setup_topology();

  /*
   * activate the killer.
   */
  KillerProcess killer( m_wl_parser.get_sim_length() );
  killer.activate();

  /*
   * activate the driver.
   */
  if( ! m_driver )
    {
      m_driver = new Driver();
      CBNASSERT( m_driver,
		 "unable to allocate cbnsim::Driver" );
    }
  m_driver->setup( m_nodes, m_wl_parser );
  m_driver->activate();

  //TODO activate the manual routing
  
  /*
   * activate the data collector
   */
  if( ! m_data_collector )
    {
      m_data_collector = new DataCollector();
      CBNASSERT( m_data_collector,
		 "unable to allocate cbnsim::DataCollector" );
    }
  // I'm using rint instead of round because free fucking BSD doesn't
  // have it... -A
  //
  ssim::Time sim_tu = m_wl_parser.get_time_unit();
  ssim::Time dci_sim =m_dci / sim_tu; //the Data Collection Interval expressed in sim time unit
  
  m_data_collector->setup( m_nodes, (ssim::Time)rint( dci_sim ), (*p_out), (*p_out_int) );
  m_data_collector->activate();
  m_data_collector->init_structures();

  pre_run_simulation_hook();
  /*
   * if sim_length is forced by cbcbsim command, ignore natural end written in the workload file and schedule the end of the sim. 
   */ 
  ssim::Time sim_end_time=0;//sim _end time, expressed in sim time unit
  if (m_sim_length!=0)
  {
    sim_end_time = m_sim_length/sim_tu;
    ssim::Sim::set_stop_time(sim_end_time);    
  }
  else
    sim_end_time = m_wl_parser.get_sim_length();
  /*
   * if specified by command line, read the input routing file and write routing tables
   */
  
  setup_routing();

  /*
   * run the simulation.
   */
  ssim::Sim::run_simulation();

  /*
   * finalize the data collector passing the simulation length time (forced by cbcbsim or written in the workload)
   */
  if ((m_dci == 0)|| (fmod(sim_end_time, dci_sim) != 0))
  {
   /*
    * if ( not collecting stats. during sim. ) or ( the dci is not multiple of the sim. time )
    * poll nodes and finalize sim. printing final statistics
    */
   m_data_collector->finalize_polling(sim_end_time);    
  }
  else
  {
   /*
    * if collecting stats. during simulation, finalize sim. printing final statistics
    */
    m_data_collector->finalize(sim_end_time);
  }
    
}
int CBNSimulation :: setup_routing()
{
  /** @brief Read from file the routing configurationv and teporarily store it in an
   *  appropriate data structure
   */
  typedef multimap< unsigned int, pair<unsigned int, Predicate > > routes_map; 
  routes_map routes;
  if (m_routing_fname.size() !=0)
  {
      /** @brief If the filename has been inserted
       */
      ifstream ifs_routing;
      ifs_routing.open( m_routing_fname.c_str() );
      if( !ifs_routing )
	throw io_exception( string( "unable to open: " ) +  m_routing_fname );
      unsigned int from, to; 
      /** @brief Parse each line of the file
       */
      while( !ifs_routing.eof() )
      {
	std::string line;
	getline(ifs_routing,line);
	if (line.size()>0)
	{
	  /** @brief Routing entry parsing (each entry is: from to Predicate) 
	   *  (notice that predicate is: A="Orange" B="video" and means in CCNx /Orange/Video)
           */
	  char* linet;
          vector<string> line_tok;
	  string delim=" ";
          linet = strtok((char *)line.c_str(),delim.c_str());
          while (linet != NULL)
	  {
	    line_tok.push_back(linet);	
	    linet = strtok(NULL,delim.c_str());
	  }
	  Predicate pred;
	  from =  atoi(line_tok[0].c_str());
	  to =  atoi(line_tok[1].c_str());
	  unsigned int i = 2;
	  Filter filt;
	  while (line_tok[i] != ";")
	  {
	    std::string name = line_tok[i];
	    std::string op = line_tok[i+1];
	    std::string value( line_tok[i+2],1,line_tok[i+2].length() - 2);
	    filt.add( Constraint( name, parse_operator(op), value ) );
	    i += 3;
	  }
	  pred.add(filt);	      
	  routes.insert(make_pair(from,make_pair(to,pred)));
	}
      }
      /** @brief Write each routing entry, in the FIB 
       */
      for(unsigned int j=0;j<m_nodes.size();j++)
      {
	pair<routes_map::iterator,routes_map::iterator> range;
	range = routes.equal_range(j);
	for(routes_map::iterator it = range.first; ((it != range.second) && (it != routes.end()));it++)
	{
	  m_nodes[j]->setup_FIB_entry((*it).second.first,(*it).second.second);
	}
      }
  }
  return 1;
}

void CBNSimulation :: setup_link( unsigned int from,
				  unsigned int to,
				  unsigned int length,
                                  double buffer,  
                                  double capacity )
{

  CBNASSERT( from < m_nodes.size(),
	     "from node id out of range: " << from );
  CBNASSERT( to < m_nodes.size(),
	     "to node id out of range: " << to );

  Node* fn = m_nodes[from];
  Node* tn = m_nodes[to];

  fn->add_neighbor( tn->pid(), length , buffer, capacity);
  tn->add_neighbor( fn->pid(), length , buffer, capacity);
  setup_link_hook( fn->pid(), tn->pid(), 1/capacity);   
  setup_link_hook( tn->pid(), fn->pid(), 1/capacity);
}

void CBNSimulation :: setup_node(unsigned int i)
{
    Node* node = m_node_factory->newNode(i, m_topo_parser.get_node_x(i),
					 m_topo_parser.get_node_y(i),m_topo_parser.get_node_cache(i),m_topo_parser.get_node_replacement(i));
    node->activate();
    m_nodes.push_back( node );
    setup_node_hook( node );
}

void CBNSimulation :: setup_topology()
{
  setup_topology_hook();
  for( unsigned int i = 0; i < m_topo_parser.get_node_count(); ++i )
    setup_node(i);

  set< pair<unsigned int,unsigned int> >::const_iterator i;
  for( i = m_topo_parser.get_links().begin(); i != m_topo_parser.get_links().end(); ++i ){
    unsigned int from = (*i).first;
    unsigned int to = (*i).second;
    setup_link( from, to, m_topo_parser.get_link_delay( from, to )/m_wl_parser.get_time_unit(),  m_topo_parser.get_link_buffer( from, to ), m_topo_parser.get_link_capacity( from, to ) );
  }
}
