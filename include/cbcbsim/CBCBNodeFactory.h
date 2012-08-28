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
// $Id: CBCBNodeFactory.h,v 1.2 2008-04-01 17:00:54 carzanig Exp $
//
#ifndef CBCBSIM_CBCBNODEFACTORY_H
#define CBCBSIM_CBCBNODEFACTORY_H

#include <cbnsim/NodeFactory.h>
#include "cbcbsim/CBCBNode.h"

namespace cbcbsim
{

  class CBCBNodeFactory : public cbnsim::NodeFactory
  {
    const BroadcastLayer&	m_bl;
    ProtocolOptions&	m_options;
  public:
    ssim::Time* time_block_fac;

 
    virtual ~CBCBNodeFactory() {};
    //add the last parameter in the constructor   -liufeng
    CBCBNodeFactory( const BroadcastLayer& bl, ProtocolOptions& options, ssim::Time* time_block_FAC)
      : m_bl( bl ), m_options( options ) 
    {
     time_block_fac = time_block_FAC; 
    }

    cbnsim::Node* newNode( unsigned int idx, unsigned int x = 0, unsigned int y = 0, unsigned int cache_s = 0, char cache_repl = 'l')
    {  
      return new CBCBNode( idx, m_bl, m_options, time_block_fac,cache_s,cache_repl);  //add the last parameter in the constructor   -liufeng
    }
  };

};

#endif
