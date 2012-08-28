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
// $Id: KillerProcess.h,v 1.2 2004/04/19 05:26:16 rutherfo Exp $
//

#ifndef CBNSIM_KILLERPROCESS_H
#define CBNSIM_KILLERPROCESS_H

#include <siena/ssim.h>

namespace cbnsim
{
  /** @brief Simple simulation process that kills the simulation after
   * a specified number of virtual time units.
   */
  class KillerProcess : public ssim::ProcessWithPId
    {
    private:
      double m_delay;
    public:
      KillerProcess( double delay )
	: m_delay( delay ) {}
      
      void init()
	{
	  ssim::Sim::self_signal_event( NULL, m_delay );
	}
      
      void process_event( const ssim::Event* evt )
	{
	  ssim::Sim::stop_simulation();
	}
    };
};

#endif
