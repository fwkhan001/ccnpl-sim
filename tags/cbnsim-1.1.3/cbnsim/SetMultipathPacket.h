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
// $Id: SetMultipathPacket.h,v 1.4 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_SETMULTIPATHPACKET_H
#define CBNSIM_SETMULTIPATHPACKET_H

#include <siena/ssimp_types.h>
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief packet used by the driver to signal a set_multipath
   * workload event to a Node.
   */
  class SetMultipathPacket : public ssim::Event
  {
      unsigned int      m_to_nid;
      Predicate		m_pred;
  public:
    SetMultipathPacket( unsigned int  to_nid, const Predicate& pred )
      : m_to_nid( to_nid ),  m_pred( pred ) {}
    
    virtual ~SetMultipathPacket() {}

    unsigned int to() const
	  { return m_to_nid; }

    const Predicate& predicate() const
	  { return m_pred; }

    bool is_predicate_null() const
	  {
	      return m_pred.bytesize() == 0;
	  }
  };
};
  
#endif