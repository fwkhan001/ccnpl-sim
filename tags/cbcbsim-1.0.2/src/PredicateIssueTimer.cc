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
// $Id: PredicateIssueTimer.cc,v 1.2 2004/04/19 05:26:39 rutherfo Exp $
//
#include <cbnsim/logging.h>
#include <siena/ssbg.h>
#include "cbcbsim/PredicateIssueTimer.h"

using namespace cbcbsim;
using namespace ssim;
using namespace std;

void PredicateIssueTimer :: init()
{
  // the poisson distribution is used to represent the varying start
  // times of the different process-internal timers
  Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
}

void PredicateIssueTimer :: process_event( const Event* evt )
{
  CBNASSERT( !evt, "PredicateIssueTimer can only handle NULL events" );
  m_node.issue_predicate();
  Sim::self_signal_event( NULL, ssbg::rand_poisson_delta( m_delay ) );
}
