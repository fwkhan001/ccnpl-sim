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
// $Id: NodeFactory.h,v 1.5 2005/10/17 08:37:13 hallcp Exp $
//
#ifndef CBNSIM_NODEFACTORY_H
#define CBNSIM_NODEFACTORY_H

#include "cbnsim/Node.h"

namespace cbnsim
{
  /** @brief Interface of a node factory needed by <code>CBNSimulation</code>.
   */
  class NodeFactory
    {
    public:
	virtual ~NodeFactory() { };

      /** @brief Abstract factory method.
       */
	virtual Node* newNode(unsigned int node_num,
			      unsigned int x, unsigned int y, unsigned int cache, char replacement) = 0;
    };
};

#endif
