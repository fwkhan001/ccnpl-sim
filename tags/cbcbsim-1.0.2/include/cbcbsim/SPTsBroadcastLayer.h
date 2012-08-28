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
// $Id: SPTsBroadcastLayer.h,v 1.2 2005/03/04 13:40:37 carzanig Exp $
//
#ifndef SPTSBROADCASTLAYER_H
#define SPTSBROADCASTLAYER_H

#include <map>
#include <list>
#include <set>
#include <siena/ssim.h>
#include <string>

#include "cbcbsim/BroadcastLayer.h"

namespace cbcbsim
{
  class SPTsBroadcastLayer : public BroadcastLayer
  {
  private:
    
    std::set<ssim::ProcessId> nodes;

    struct BLink {
      ssim::ProcessId prev;
      std::set<ssim::ProcessId> next;
      int hop_count;

      BLink() : prev(ssim::NULL_PROCESSID), next() {};
    };
    
    struct B : public std::map<ssim::ProcessId, BLink> {
	
    };
    
    struct Paths : public std::map<ssim::ProcessId, B> {

    };
    
    struct BLinkOneWay {
      ssim::ProcessId next;
      int hop_count;

      BLinkOneWay() : next() {};
    };
    
    struct BOneWay : public std::map<ssim::ProcessId, BLinkOneWay> {
	
    };
    
    struct AlternativePaths : public std::map< std::pair< std::pair<ssim::ProcessId,ssim::ProcessId> , unsigned int >, BOneWay> {

    };
    Paths paths;
    AlternativePaths alternative_paths;
   public:
    struct Edge {
      ssim::ProcessId from;
      ssim::ProcessId to;
      double len;
	
      Edge(ssim::ProcessId f, ssim::ProcessId t, double l): from(f), to(t), len(l) {};
    };

    std::list<Edge> input;
     virtual ~SPTsBroadcastLayer() {};

    virtual void	addLink(ssim::ProcessId from, ssim::ProcessId to, double length);
    virtual void	addNode(ssim::ProcessId id);

    virtual void	computePaths();

    virtual const std::set<ssim::ProcessId>& 
    getChildren(ssim::ProcessId root, ssim::ProcessId node) const;
    
    virtual ssim::ProcessId	getParent(ssim::ProcessId root, ssim::ProcessId node) const;
    
    virtual ssim::ProcessId getNext(ssim::ProcessId from, ssim::ProcessId to, ssim::ProcessId current_node, unsigned int path_number) const;
    
    virtual std::string str() const { return ""; };
    
    virtual void        computeAlternativePaths(std::list<Edge> explored_paths, ssim::ProcessId i, ssim::ProcessId j,unsigned int n);

    virtual int		getHopCount(ssim::ProcessId from, ssim::ProcessId to ) const;
  };

};

#endif
