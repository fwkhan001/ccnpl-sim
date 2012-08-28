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
// $Id: WorkloadEvent.h,v 1.1 2004/01/07 16:33:23 rutherfo Exp $
//
#ifndef CBNSIM_WORKLOADEVENT_H
#define CBNSIM_WORKLOADEVENT_H

#include <siena/ssim.h>
#include <string>

namespace cbnsim
{
  /** @brief Abstract class representing a single workload event.
   */
  class WorkloadEvent
  {
  private:
    std::string		m_event_name;
    bool		m_node_event;
    ssim::Time		m_timestep;

  protected:
    /** @brief Derived classes must provide a name.
     */
    WorkloadEvent( const std::string& event_name,
		   bool node_event,
		   ssim::Time timestep )
      : m_event_name( event_name ),
      m_node_event( node_event ),
      m_timestep( timestep ) {}

  public:
    virtual ~WorkloadEvent() {}

    /** @brief Returns the event name.
     */
    const std::string& event_name() const
      { return m_event_name; }

    /** @brief Returns true if the event is a 'Node Event', false
     * if it is a 'Link Event'.
     */
    bool node_event() const
      { return m_node_event; }

    /** @brief Returns the timestep at which the event is to
     * be issued.
     */
    ssim::Time timestep() const
      { return m_timestep; }
  };
};
#endif
