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


#include "cbcbsim/FlowController.h"

#ifndef CBCBSIM_CBR_H
#define CBCBSIM_CBR_H


namespace cbcbsim
{

  /** @brief Node class for socket that implements a window protocol.
   */
  class CBR : public FlowController
  { 
    
    protected:
      
      unsigned int 	m_win;
      double		m_rate;
    
    public:
    
      CBR(unsigned int m_id, std::string content_name, unsigned int m_port, const ProtocolOptions& options, 
	 unsigned int ifid, ssim::Time start_time,unsigned int win, double rate):FlowController(m_id, content_name, 
	  m_port, options, ifid, start_time),m_win(win),m_rate(rate*m_options.sim_time_unit){init();}

      ~CBR(){}


    void init();
    
    double average_win(){ return (double)m_win; }
    
    int free_slot_win(){return (int)m_win;}
    
    int request_chunk(unsigned int,double){return 1;}
    
    int received_chunk( int, bool, string){return 1;}

    void receiverCBR();
 
  };

};

#endif
