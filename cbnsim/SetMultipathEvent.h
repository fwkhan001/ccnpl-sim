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
// $Id: SetMultipathEvent.h,v 1.2 2004/02/03 06:16:21 hallcp Exp $
//
#ifndef CBNSIM_SETMULTIPATHEVENT_H
#define CBNSIM_SETMULTIPATHEVENT_H

#include "cbnsim/NodeEvent.h"
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief Abstract base class for workload events that pertain to nodes.
   */
  class SetMultipathEvent : public NodeEvent
    {
    private:
      	unsigned int 		m_to_node_id;
	Predicate		m_predicate;
	unsigned int            m_inte;
	
    public:
      SetMultipathEvent( unsigned int from_node_id, unsigned int to_node_id, double timestamp, const Predicate& predicate, unsigned int inte )
	: NodeEvent( "set_multipath", from_node_id, timestamp ),
	m_to_node_id(to_node_id), m_predicate( predicate ), m_inte(inte) {}

      /** @brief Returns the predicate associated with this event.
       */
      const Predicate& predicate() const
	    { return m_predicate; }
      
      unsigned int inte() const
	    { return m_inte; }
	   
      unsigned int to() const
	    { return m_to_node_id; }
    };
};

#endif