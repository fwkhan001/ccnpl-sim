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
// $Id: cbnsim.cc,v 1.5 2004/04/19 05:26:16 rutherfo Exp $
//
#include <climits>
#include "cbnsim/Packet.h"
#include "cbnsim/NodeFailPacket.h"
#include "cbnsim/NodeRecoverPacket.h"
#include "cbnsim/SendMessagePacket.h"
#include "cbnsim/SetPredicatePacket.h"
#include "cbnsim/DownloadContentPacket.h"
#include "cbnsim/PublishContentPacket.h"

namespace cbnsim
{
  bool g_debug_on = false;
  bool g_info_on = true;
  bool g_error_on = true;

  unsigned int Packet::s_id_counter( 1 );
};
