// -*- C++ -*-
//
//  This file is part of CCN-PLsim, a CCNx simulator
//
//  Authors: See the file AUTHORS for full details. 
//
//  Copyright (C) 2012 France Telecom R&D
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
//  USA, or send email to luca.muscariello@orange.com 
//
#include "cbcbsim/CBR.h"
#include <cbnsim/ReceiverCBR.h>

using namespace cbcbsim;
using namespace ssim;

void CBR :: init (){
   receiverCBR();
}

void CBR :: receiverCBR()
{
  stringstream por;  por << port();
  string  socket_address = content_name() + "/port:" + por.str();
  cout << "next CBR time " <<  (double)1.0/m_rate << endl;
  Sim::signal_event( m_id, new cbnsim::ReceiverCBR(ifid(),socket_address,start_time()), 1.0/m_rate);
}
