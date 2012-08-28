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
// $Id: BroadcastLayer.h,v 1.2 2008-04-01 17:02:12 carzanig Exp $
//
#ifndef CBCBSIM_BROADCASTLAYER_H
#define CBCBSIM_BROADCASTLAYER_H

#include <cbnsim/WorkloadParser.h>
#include <map>
#include <set>
#include <string>
#include <siena/ssim.h>

namespace cbcbsim
{

  class BroadcastLayer
  {
  public:
    virtual ~BroadcastLayer() {};
    virtual void addLink( ssim::ProcessId from, ssim::ProcessId to, double length ) = 0;

    virtual void addNode( ssim::ProcessId id ) = 0;

    virtual void computePaths() = 0;

    virtual const std::set<ssim::ProcessId>& getChildren( ssim::ProcessId root,
							  ssim::ProcessId node ) const = 0;

    virtual int getHopCount( ssim::ProcessId from, ssim::ProcessId to ) const = 0;

    virtual ssim::ProcessId getParent( ssim::ProcessId root, ssim::ProcessId node ) const = 0;
    
    virtual ssim::ProcessId getNext(ssim::ProcessId from, ssim::ProcessId to, ssim::ProcessId current_node, unsigned int path_number) const = 0;

    virtual std::string str() const = 0;
  };

};

#endif
