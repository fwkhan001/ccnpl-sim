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
// $Id: RA.h,v 1.2 2004/04/19 05:26:40 rutherfo Exp $
//
#ifndef CBCBSIM_RA_H
#define CBCBSIM_RA_H

#include <cbnsim/Packet.h>
#include <cbnsim/types.h>
#include <siena/ssimp_types.h>
#include <string>

namespace cbcbsim
{
  class RA : public cbnsim::Packet
  {
    ssim::ProcessId	m_issuer;
    ssim::ProcessId	m_destination;
    unsigned int 	m_path_number;
    cbnsim::Predicate	m_pred;
  
  public:
    RA( ssim::ProcessId sender, ssim::ProcessId issuer, ssim::ProcessId destination, unsigned int path_number, const cbnsim::Predicate& pred )
      : Packet( sender, true ), m_issuer( issuer ), m_destination( destination ), m_path_number( path_number ), m_pred( pred ) {}

    ssim::ProcessId issuer() const
    { return m_issuer; }
    
    ssim::ProcessId dest() const
    { return m_destination; }
    
    unsigned int path_number() const
    { return m_path_number; }

    const cbnsim::Predicate& predicate() const
    { return m_pred; }
    
    unsigned int size() const
    { return 0; } 
  };

//   inline std::ostream& operator<<( std::ostream& out, const RA& ra )
//   { return out << "[cbcbsim::RA: issuer=" << ra.issuer() << ",pred=" << ra.predicate() << "]"; }
};
#endif
