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
// $Id: SRSendTimer.h,v 1.2 2004/04/19 05:26:40 rutherfo Exp $
//
#ifndef CBCBSIM_SR_SEND_TIMER_H
#define CBCBSIM_SR_SEND_TIMER_H

#include <siena/ssim.h>
#include "cbcbsim/CBCBNode.h"

namespace cbcbsim
{
  class SRSendTimer : public ssim::ProcessWithPId
  {
    const ssim::Time	m_delay;
    CBCBNode&		m_node;
    
  public:
    SRSendTimer( CBCBNode& node, const ssim::Time delay )
      : m_delay( delay ), m_node( node ) {}
    
    void init();
    
    void process_event( const ssim::Event* evt );
  };
};

#endif
