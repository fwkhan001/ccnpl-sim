// -*- C++ -*-
//
//  This file is part of CBNSIM, the Content-Based Networking
//  Simulation Library.
//
//  Authors: See the file AUTHORS for full details.
//
//  Copyright (C) 2002-2004 University of Colorado
//            (C) 2009-2012 France Telecom R&D
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
// $Id: DeQueuePacket.h,v 1 2012/02/14 03:48:16 Gallo Exp $
//
#ifndef CBNSIM_DEQUEUEPACKET_H
#define CBNSIM_DEQUEUEPACKET_H

#include <siena/ssimp_types.h>
#include "cbnsim/Packet.h"


namespace cbnsim
{
  /** @brief event used to signal the packet transmission on a specific interface 
   * workload event to a Node.
   */ 
  class DeQueuePacket : public ssim::Event
  {
    void* m_q;
  public:
    DeQueuePacket(void* q) :  m_q( q){}
    
    virtual ~DeQueuePacket() {}
    
    void* get_queue() const { return m_q; }
  }; 
};
  
#endif
