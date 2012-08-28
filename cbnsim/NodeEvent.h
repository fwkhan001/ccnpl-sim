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
// $Id: NodeEvent.h,v 1.1 2004/01/07 16:33:23 rutherfo Exp $
//
#ifndef CBNSIM_NODEEVENT_H
#define CBNSIM_NODEEVENT_H

#include "cbnsim/WorkloadEvent.h"

namespace cbnsim
{
  /** @brief Abstract base class for workload events that pertain to nodes.
   */
  class NodeEvent : public WorkloadEvent
    {
    private:
      unsigned int	m_node_id;
      
    protected:
      NodeEvent( const std::string& event_name,
		 unsigned int node_id, ssim::Time timestep )
	: WorkloadEvent( event_name, true, timestep ),
	m_node_id( node_id ) {}
      
    public:
      /** @brief Returns the node id for this event.
       */
      unsigned int node_id() const
	{ return m_node_id; }
    };
};

#endif
