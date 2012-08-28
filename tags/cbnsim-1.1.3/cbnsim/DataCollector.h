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
// $Id: DataCollector.h,v 1.3 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_DATACOLLECTOR_H
#define CBNSIM_DATACOLLECTOR_H

#include <iostream>
#include <map>
#include <siena/ssim.h>
#include "cbnsim/Node.h"


namespace cbnsim
{
  /** Base class for data collection processes.
   */
  class DataCollector : public ssim::ProcessWithPId
    {
      std::ostream*			m_out;
      std::ostream*			m_int_out;
      ssim::Time			m_timeout;

    public:
      DataCollector():  m_out( NULL ), m_timeout( 0 ), m_nodes( NULL ) {}

      NodeList*				m_nodes;
      
      /** @brief Method called by <code>CBNSimulation</code> to print
       * out any final data items.
       */
      void finalize(ssim::Time time);
      
      /** @brief Method called by <code>CBNSimulation</code> to print
       * out any final data items.
       */
      void finalize_polling(ssim::Time time);

      void init();

      /** @brief Method that can be overridden to output a data values
       * at the end of the simulation.
       *
       * The default implementation is empty.
       */
      virtual void output_final( std::ostream& out, ssim::Time time) {}

      /** @brief Method that can be overridden to output data values inline.
       *
       * The default implementation is empty.
       */
      virtual void output_inline( std::ostream& out, ssim::Time time ) {}

      /** @brief Method that can be overridden to poll a node instance
       * for a particular value.
       *
       * The default implementation is empty.
       */
      virtual void poll_node( Node& node, ssim::Time time  ) {}
      
     /** @brief Method that initialize the structures used to save simulation's statistics 
       */
      virtual void init_structures() {}

      void process_event( const ssim::Event* evt );

      void setup( NodeList& nodes, ssim::Time timeout, std::ostream& out, std::ostream& out_int )
	{ m_nodes = &nodes; m_timeout = timeout;  m_out = &out; m_int_out = &out_int;  }
    };
};
#endif
