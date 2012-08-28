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
// $Id: SetPredicatePacket.h,v 1.4 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_SETPREDICATEPACKET_H
#define CBNSIM_SETPREDICATEPACKET_H

#include <siena/ssimp_types.h>
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief packet used by the driver to signal a set_predicate
   * workload event to a Node.
   */
  class SetPredicatePacket : public ssim::Event
  {
      Predicate		m_pred;
      unsigned int      m_msg_rate;
  public:
    SetPredicatePacket( const Predicate& pred, unsigned int msg_rate )
      : m_pred( pred ), m_msg_rate(msg_rate) {}
    
    virtual ~SetPredicatePacket() {}
    
    const Predicate& predicate() const
	  { return m_pred; }

      unsigned int msg_rate() const
	  { return m_msg_rate; }

      bool is_predicate_null() const
	  {
	      return m_pred.bytesize() == 0;
	  }
  };
};
  
#endif
