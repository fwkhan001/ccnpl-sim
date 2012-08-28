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
// $Id: DownloadObjectPacket.h,v 1 Max Gallo $
//
#ifndef CBNSIM_DOWNLOADCONTENTPACKET_H
#define CBNSIM_DOWNLOADCONTENTPACKET_H

#include <siena/ssimp_types.h>
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief packet used by the driver to signal a download_object
   * workload event to a Node.
   */
  class DownloadContentPacket : public ssim::Event
  {
      Message		m_msg;
      unsigned int      m_msg_rate;
  public:
    DownloadContentPacket( const Message& message, unsigned int msg_rate )
      : m_msg( message ), m_msg_rate(msg_rate) {}
    
    virtual ~DownloadContentPacket() {}
    
    const Message& msg() const
    { return m_msg; }
    
    unsigned int msg_rate() const
    { return m_msg_rate; }
    
    bool is_message_null() const
    { return m_msg.bytesize() == 0;}
  };
};
  
#endif
