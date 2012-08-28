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
// $Id: CBNSimulation.h,v 1.6 2004/03/06 22:05:19 rutherfo Exp $
//
#ifndef CBNSIM_CBNSIMULATION_H
#define CBNSIM_CBNSIMULATION_H

#include <string>
#include <vector>
#include "cbnsim/DataCollector.h"
#include "cbnsim/Driver.h"
#include "cbnsim/NodeFactory.h"
#include "cbnsim/WorkloadParser.h"
#include "cbnsim/TopologyParser.h"

namespace cbnsim
{
  /** @brief Main entry point for running a CBN simulation.
   */
  class CBNSimulation
    {
    public:
      /** @brief Default constructor.
       */
      CBNSimulation();

      virtual ~CBNSimulation();

      /** @brief Adds an event parser to the internal <code>WorkloadParser</code>.
       */
      void add_event_parser( WorkloadEventParser* wep )
      { m_wl_parser.add( wep ); }

      /** @brief Set the flag that controls the printing of debug
       * messages.
       */
      void debug_messages( bool debug_on );

      /** @brief Get the debug messages flag.
       */
      bool debug_messages() const;

      /** @brief Sets the data collection interval in seconds.
       */
      void dci( double dci )
      { m_dci = dci; }
      
      /** @brief Sets the simulation length in seconds.
       */
      void sim_length( double sim_length )
      { m_sim_length = sim_length; }

      /** @brief Returns the data collection interval.
       */
      double dci() const
      { return m_dci; }

      /** @brief Set the flag that controls the printing of error
       * messages.
       */
      void error_messages( bool error_on );

      /** @brief Get the error messages flag.
       */

      bool error_messages() const;

      /** @brief Set the flag that controls the printing of info
       * messages.
       */
      void info_messages( bool info_on );

      /** @brief Get the info messages flag.
       */
      bool info_messages() const;

      /** @brief Set the workload file name.
       *
       */
      void wl_fname( const std::string& ifname );

      /** @brief Get the workload file name.
       *
       */
      const std::string& wl_fname() const;
      
      /** @brief Set the routing file name for the manual routing 
       */
      void routing_fname( std::string routing_fname );
      
      /** @brief Get the routing file name for the manual routing 
       */
      std::string routing_fname( );
      
      /** @brief Set the topology file name for the manual routing 
       */
      void topo_fname( std::string topo_fname );
      
      /** @brief Get the topology file name for the manual routing 
       */
      std::string topo_fname( );

      /** @brief Set the output file name.
       *
       * Empty string corresponds to <code>stdout</code>.
       */
      void output_fname( const std::string& ofname );

      /** @brief Get the output file name.
       *
       * Empty string corresponds to <code>stdout</code>.
       */
      const std::string& output_fname() const;
      
      /** @brief Set the output file name for intermediate data.
       *
       * Empty string corresponds to <code>stdout</code>.
       */
      void output_int_fname( const std::string& ofname );

      /** @brief Get the output file name for intermediate data.
       *
       * Empty string corresponds to <code>stdout</code>.
       */
      const std::string& output_int_fname() const;

      /** @brief Main entry method for the simulation.
       */
      void run() throw ( std::exception );

      /** @brief Sets the <code>DataCollector</code> associated
       * with the simulation.
       *
       * The CBNSimulation object manages memory for the DataCollector.
       */
      void set_data_collector( DataCollector* data_collector )
      { delete m_data_collector; m_data_collector = data_collector; }

      /** @brief Sets the <code>Driver</code> associated
       * with the simulation.
       *
       * The CBNSimulation object manages memory for the Driver.
       */
      void set_driver( Driver* driver )
      { delete m_driver; m_driver = driver; }

      /** @brief Sets the <code>NodeFactory</code> associated
       * with the simulation.
       *
       * The CBNSimulation object manages memory for the NodeFactory.
       */
      void set_factory( NodeFactory* node_factory )
      { delete m_node_factory; m_node_factory = node_factory; }

      /** @brief Returns the time unit of the workload.
       */
      double time_unit() const
      { return m_wl_parser.get_time_unit(); }
      
      /** @brief Returns the length of simulation
       */
      unsigned int sim_length() const
      { return m_wl_parser.get_sim_length(); }

      siena::operator_id parse_operator(std::string str) 
      {
	siena::operator_id out;
	if( str == "=" )
	{
	  out = siena::eq_id;
	}
	else if( str == "!=" )
	{
	  out = siena::ne_id;
	}
	else if( str == "<" )
	{
	  out = siena::lt_id;
	}
	else if( str == ">" )
	{
	  out = siena::gt_id;
	}
	else if( str == "=*" )
	{
	  out = siena::pf_id;
	}
	else if( str == "*=" )
	{
	  out = siena::sf_id;
	}
	else if( str == "**" )
	{
	  out = siena::ss_id;
	}
	else
	{
	  std::cout << "Invalid operator in routing informations" << std::endl;
	}
	return out;
      }

    protected:
      /** @brief Hook method invoked just before
       * ssim::Sim::run_simulation() is invoked.  All setup is
       * completed at this point.
       */
      virtual void		pre_run_simulation_hook() throw () {}

      /** @brief Nook method invoked after a link is established between
       * two node processes.
       *
       * These links are directed in the sense that this method is invoked
       * twice for each pair of nodes that are connected.
       */
      virtual void		setup_link_hook( ssim::ProcessId from,
						 ssim::ProcessId to,
						 double length ) throw () {}

      /** @brief Hook method invoked after a new Node is instantiated
       * and activated.
       */
      virtual void		setup_node_hook( Node* node ) throw () {}

      /** @brief Hook method invoked after the topology has been set
       * up.
       */
      virtual void		setup_topology_hook() throw () {}
      
    private:
      void			setup_link( unsigned int from,
					    unsigned int to,
					    unsigned int length,
                                            double buffer, 
					    double capacity);
      void			setup_node(unsigned int i);
      void			setup_topology();
      int 			setup_routing();


      DataCollector*		m_data_collector;
      double			m_dci;
      Driver*			m_driver;
      std::string		m_wl_fname;
      NodeFactory*		m_node_factory;     
      std::string		m_routing_fname;
      std::string		m_topo_fname;
      double			m_sim_length;
      std::vector<Node*>	m_nodes;
      std::string		m_output_fname;
      std::string		m_output_int_fname;
      TopologyParser		m_topo_parser;
      WorkloadParser		m_wl_parser;
    };
};

#endif
