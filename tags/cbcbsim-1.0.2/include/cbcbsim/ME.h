// -*- C++ -*-
//
//  This file is part of CBCBSIM, the Combined Broadcast Content-Based
//  (CBCB) routing simulation.
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
// $Id: ME.h,v 1.2 2004/04/19 05:26:40 rutherfo Exp $
//
#ifndef CBCBSIM_ME_H
#define CBCBSIM_ME_H

#include <cbnsim/Packet.h>
#include <cbnsim/types.h>
#include <ostream>
#include <string>
#include <siena/ssimp_types.h>

using namespace ssimp;

namespace cbcbsim
{
  class ME : public cbnsim::Packet
  {
    ssim::ProcessId	m_issuer;
    Message		m_msg;
    cbnsim::MessageId	m_msg_id;
    unsigned int	m_t_used; //for LFU  --liufeng
  public:
    ME( ssim::ProcessId sender, ssim::ProcessId issuer, cbnsim::MessageId msg_id, const Message& msg, unsigned int t_used )
      : Packet( sender, false ), m_issuer( issuer ), m_msg( msg ), m_msg_id( msg_id ), m_t_used( t_used ) {}
    
    virtual ~ME() {}

    ssim::ProcessId issuer() const
    { return m_issuer; }
    
    const Message& message() const
    { return m_msg; }

    cbnsim::MessageId message_id() const
    { return m_msg_id; }

    unsigned int time_used() const
    { return m_t_used; }

    void time_used_plusone() 
    { m_t_used++ ; }

    //for the used_time sorting
    bool operator> ( const ME& m_me ) const
    { 
     return m_t_used > m_me.m_t_used; 
    }

    //for the msg_id sorting
    bool operator< ( const ME& m_me ) const
    {
     return m_msg_id < m_me.m_msg_id;
    }

    unsigned int size() const
    { return 0; } 

  };
/*
  inline std::ostream& operator<<( std::ostream& out, const ME& me )
  { return out << "[cbcbsim::ME: msg=" << me.message() << "]"; }*/

};
  
#endif
