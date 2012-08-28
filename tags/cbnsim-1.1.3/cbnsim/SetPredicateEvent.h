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
#ifndef CBNSIM_SETPREDICATEEVENT_H
#define CBNSIM_SETPREDICATEEVENT_H

#include "cbnsim/NodeEvent.h"
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief Abstract base class for workload events that pertain to nodes.
   */
  class SetPredicateEvent : public NodeEvent
    {
    private:
	Predicate		m_predicate;
	unsigned int            m_msg_rate;
    public:
      SetPredicateEvent( unsigned int node_id, double timestep, const Predicate& predicate, unsigned int msg_rate )
	: NodeEvent( "set_predicate", node_id, timestep ),
	m_predicate( predicate ), m_msg_rate(msg_rate) {}

      /** @brief Returns the predicate associated with this event.
       */
      const Predicate& predicate() const
	    { return m_predicate; }

	unsigned int msg_rate() const
	    { return m_msg_rate; }
    };
};

#endif
