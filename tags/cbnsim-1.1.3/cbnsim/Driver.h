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
// $Id: Driver.h,v 1.3 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_DRIVER_H
#define CBNSIM_DRIVER_H

#include <siena/ssim.h>
#include <vector>
#include "cbnsim/Node.h"
#include "cbnsim/WorkloadParser.h"

namespace cbnsim
{
  /** @brief Simulation process that pulls the workload events and
   * issues them to nodes, thereby driving the simulation.
   */
  class Driver : public ssim::ProcessWithPId
  {
    typedef std::vector<Node*>		NodeList;

    const WorkloadEvent			*m_next_event;
    const NodeList*			m_nodes;
    WorkloadParser*			m_parser;

  public:
    Driver()
      : m_next_event( NULL ), m_nodes( NULL ), m_parser( NULL ) {}

    virtual ~Driver() {}

    /** @brief Virtual method to allow handling of custom events.
     *
     * Default implementation is empty.
     */
    virtual void handle_event( Node& node, const NodeEvent& node_event ) {}

    void init();

    void process_event( const ssim::Event* evt );

    void setup( const NodeList& nodes, WorkloadParser& parser )
    { m_nodes = &nodes; m_parser = &parser; }
  };
};
#endif
