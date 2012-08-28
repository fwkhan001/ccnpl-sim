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
// $Id: MSTBroadcastLayer.h,v 1.1 2004/03/19 17:04:48 carzanig Exp $
//
#ifndef CBCBSIM_MSTBROADCASTLAYER_H
#define CBCBSIM_MSTBROADCASTLAYER_H

#include <map>
#include <set>
#include <siena/ssim.h>
#include <string>
#include "cbcbsim/BroadcastLayer.h"

namespace cbcbsim
{

  class MSTBroadcastLayer : public BroadcastLayer
  {
    std::multimap<int,std::pair<ssim::ProcessId,ssim::ProcessId> > m_links;
    std::map<ssim::ProcessId,std::set<ssim::ProcessId> > m_mst;
    std::set<ssim::ProcessId> m_nodes;
    std::map<ssim::ProcessId,std::map<ssim::ProcessId,std::set<ssim::ProcessId> > > m_children;
    std::map<ssim::ProcessId,std::map<ssim::ProcessId,ssim::ProcessId> > m_parents;

    int getHopCount( ssim::ProcessId from, ssim::ProcessId to, std::set<ssim::ProcessId>& visited ) const;

    void visit( ssim::ProcessId node,
		std::set<ssim::ProcessId>& visited,
		std::map<ssim::ProcessId,std::set<ssim::ProcessId> >& children,
		std::map<ssim::ProcessId,ssim::ProcessId>& parents );

  public:
    MSTBroadcastLayer();

    virtual ~MSTBroadcastLayer();

    virtual void addLink( ssim::ProcessId from, ssim::ProcessId to, int length );

    virtual void addNode( ssim::ProcessId id );

    virtual void computePaths();

    virtual const std::set<ssim::ProcessId>& getChildren( ssim::ProcessId root,
							  ssim::ProcessId node ) const;

    virtual int getHopCount( ssim::ProcessId from, ssim::ProcessId to ) const;

    virtual ssim::ProcessId getParent( ssim::ProcessId root, ssim::ProcessId node ) const;

    virtual std::string str() const;
  };

};
#endif
