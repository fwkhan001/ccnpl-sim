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
// $Id: NodeFailWNP.h,v 1.4 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_NODEFAILWPN_H
#define CBNSIM_NODEFAILWPN_H

#include <siena/ssbg.h>
#include "cbnsim/WorkloadNodeProcess.h"

namespace cbnsim
{
  /** @brief Workload node process used to generate <code>node_fail</code> events.
   */
  class NodeFailWNP : public WorkloadNodeProcess
    {
    public:
      /** @brief Constructor.
       */
	NodeFailWNP( unsigned int nid, std::ostream& out, unsigned int delay,
		     unsigned int final_delay, unsigned int recover_delay);

      /** @brief Init method issues sets a timeout.
       */
      void init();

      /** @brief Process_event method outputs a new send_message event
       * and resets its timeout.
       */
      void process_event( const ssim::Event* evt );

    private:
      void print();

	unsigned int	m_delay;
	unsigned int    m_final_delay;
	unsigned int    m_recover_delay;
	bool            m_failed;
	unsigned int    m_final_time;
    };
};

#endif
