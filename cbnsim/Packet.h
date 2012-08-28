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
// $Id: Packet.h,v 1.4 2005/10/03 21:27:17 hallcp Exp $
//
#ifndef CBNSIM_PACKET_H
#define CBNSIM_PACKET_H

#include <siena/ssim.h>
#include <string>

namespace cbnsim
{
  /** @brief Abstract base classe for messages exchanged between
   * nodes.
   */
  class Packet : public ssim::Event
  {
    static unsigned int s_id_counter;

    bool		m_is_control;
    unsigned int	m_id;
    ssim::ProcessId     m_sender;
    
  protected:
    Packet( ssim::ProcessId sender, bool is_control )
      : m_is_control( is_control ),
	m_id( s_id_counter++ ),
	m_sender( sender ) {}
    
  public:
    virtual ~Packet() {}

    /** @brief Returns this packets size in bytes.  Default returns 0.
     */
    virtual unsigned int size() const { return 0; }
      
    /** @brief Returns this packets unique id.
     */
    unsigned int id() const
    { return m_id; }

    /** @brief Returns the is_control flag.
     */
    bool is_control() const
    { return m_is_control; }

    /** @brief Returns the sending process.
     */
    ssim::ProcessId sender() const
    { return m_sender; }
  };
};

#endif
