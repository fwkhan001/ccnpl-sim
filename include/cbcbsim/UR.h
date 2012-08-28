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
// $Id: UR.h,v 1.2 2004/04/19 05:26:40 rutherfo Exp $
//
#ifndef CBCBSIM_UR_H
#define CBCBSIM_UR_H

#include <cbnsim/Packet.h>
#include <cbnsim/types.h>
#include <string>
#include <siena/ssimp_types.h>

namespace cbcbsim
{
  class UR : public cbnsim::Packet
  {
    ssim::ProcessId m_srIssuer;
    unsigned int m_srNumber;
    cbnsim::Predicate m_pred;
  public:
    UR( ssim::ProcessId sender, ssim::ProcessId srIssuer, unsigned int srNumber,
	const cbnsim::Predicate& pred )
      : Packet( sender, true ), m_srIssuer( srIssuer ), m_srNumber( srNumber), m_pred( pred ) {}

    virtual ~UR() {}

    ssim::ProcessId sr_issuer() const
    { return m_srIssuer; }

    int sr_number() const
    { return m_srNumber; }

    const cbnsim::Predicate& predicate() const
    { return m_pred; }

    unsigned int size() const
    { return 0; } 

  };

//   inline std::ostream& operator<<( std::ostream& out, const UR& ur )
//   { return out << "[cbcbsim::UR: sr_issuer=" << ur.sr_issuer()
// 	       << ",sr_number=" << ur.sr_number()
// 	       << ",predicate=" << ur.predicate() << "]"; }
};
#endif
