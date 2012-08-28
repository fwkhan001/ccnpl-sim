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
// $Id: PublishContentEvent.h,v 1 Max $
//
#ifndef CBNSIM_PUBLISHCONTENTEVENT_H
#define CBNSIM_PUBLISHCONTENTEVENT_H

#include "cbnsim/NodeEvent.h"
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief Abstract base class for workload events that pertain to nodes.
   */
  class PublishContentEvent : public NodeEvent
    {
    private:
	Predicate		m_predicate;
	unsigned int            m_size; //# of chunk of the content
        unsigned int            m_bytesize; // size in byte of the single chunk 
	unsigned int		m_receiver_win;
	unsigned int 		m_fec_perc;
	unsigned int 		m_popularity_class;
	
    public:
      PublishContentEvent( unsigned int node_id, double timestep, const Predicate& predicate, unsigned int size, unsigned int bytesize, unsigned int receiver_win, unsigned int fec_perc, unsigned int popularity_class)
	: NodeEvent( "publish_content", node_id, timestep ), m_predicate( predicate ), m_size( size ), m_bytesize( bytesize ), m_receiver_win( receiver_win ), m_fec_perc( fec_perc ) , m_popularity_class( popularity_class ) {}

      /** @brief Returns the predicate associated with this event.
       */
      const Predicate& predicate() const
      { return m_predicate; }

      unsigned int size() const
      { return m_size; }

      unsigned int bytesize() const
      { return m_bytesize; }
	    
      unsigned int receiver_win() const
      { return m_receiver_win; }
	    
      unsigned int fec_perc() const
      { return m_fec_perc; }
	    
      unsigned int popularity_class() const
      { return m_popularity_class; }

    };
};

#endif
