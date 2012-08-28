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
// $Id: SR.h,v 1.2 2004/04/19 05:26:40 rutherfo Exp $
//
#ifndef CBCBSIM_SR_H
#define CBCBSIM_SR_H

#include <cbnsim/Packet.h>
#include <iostream>
#include <string>

namespace cbcbsim
{
  class SR : public cbnsim::Packet
  {
    ssim::ProcessId	m_issuer;
    unsigned int	m_number;
    static unsigned int m_serial;

  public:
    SR( ssim::ProcessId sender, ssim::ProcessId issuer, unsigned int number )
      : Packet( sender, true ), m_issuer( issuer ), m_number( number ) {}

    virtual ~SR() {}

    ssim::ProcessId issuer() const
    { return m_issuer; }

    static unsigned int next_number()
    { return ++m_serial; }

    unsigned int number()  const
    { return m_number; }

    unsigned int size() const
    { return 0; } 

  };

//   inline std::ostream& operator<<( std::ostream& out, const SR& sr )
//   { return out << 
//       "[cbcbsim::SR: issuer=" << 
//       sr.issuer() << 
//       ",number=" << 
//       sr.number() << 
//       "]"; }
};
#endif
