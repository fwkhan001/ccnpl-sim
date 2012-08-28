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
// $Id: WorkloadNodeProcess.h,v 1.1 2004/01/07 16:33:23 rutherfo Exp $
//
#ifndef CBNSIM_WORKLOADNODEPROCESS_H
#define CBNSIM_WORKLOADNODEPROCESS_H

#include <ostream>
#include <siena/ssim.h>

namespace cbnsim
{
  /** @brief Abstract class defines the interface for all workload
   * generation processes that are associated with a node.
   */
  class WorkloadNodeProcess : public ssim::ProcessWithPId
    {
    protected:
      /** @brief Protected constructor must be supplied with the node
       * id associated with this process.
       */
      WorkloadNodeProcess( unsigned int nid, std::ostream& out )
	: m_out( out ), m_nid( nid ) {}

      /** @brief Output stream should be used directly by all derived
       * classes for printing output.
       */
      std::ostream&		m_out;

    public:
      /** @brief Returns the node id associated with this process.
       */
      unsigned int node_id() const
	{ return m_nid; }

    private:
      unsigned int	m_nid;
    };
};

#endif
