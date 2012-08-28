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
// $Id: NodeDeletePacket.h,v 1 2010/01/21 03:48:16 Max Exp $
//
#ifndef CBNSIIM_RECEIVERTO_H
#define CBNSIM_RECEIVERTO_H

#include <cbnsim/types.h>
#include <siena/ssim.h>



namespace cbnsim
{
  /** @brief signal a Receiver timeout
   * workload event to a Node.
   */
  class ReceiverCBR : public ssim::Event
  {
    unsigned int 		m_ifid_sent;
    std::string 		m_socket_address;
    ssim::Time			m_start_time;
    
  public:
    ReceiverCBR(unsigned int ifid_sent,std::string socket_addr, ssim::Time start_time) 
      : m_ifid_sent( ifid_sent ), m_socket_address( socket_addr ), m_start_time(start_time) {}

  virtual ~ReceiverCBR() {}
  
  std::string socket_address() const
  { return m_socket_address; }
  
  unsigned int ifid_sent() const
  { return m_ifid_sent; }
  
  ssim::Time start_time() const
  { return m_start_time; }
  
  };
};

#endif