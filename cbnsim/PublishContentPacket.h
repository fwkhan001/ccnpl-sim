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
// $Id: PublishContentPacket.h,v 1 Max Gallo $
//
#ifndef CBNSIM_PUBLISHCONTENTPACKET_H
#define CBNSIM_PUBLISHCONTENTPACKET_H

#include <siena/ssimp_types.h>
#include "cbnsim/types.h"

namespace cbnsim
{
  /** @brief packet used by the driver to signal a download_object
   * workload event to a Node.
   */
  class PublishContentPacket : public ssim::Event
  {
      Predicate		m_pred;
      unsigned int      m_size; //# of chunk of the content
      unsigned int      m_bytesize; // size in byte of the content
      unsigned int 	m_receiver_win;
      unsigned int 	m_fec_perc;
      unsigned int	m_popularity_class;

  public:
    PublishContentPacket( const Predicate& pred, unsigned int size , unsigned int bytesize, unsigned int receiver_win, unsigned int fec_perc, unsigned int popularity_class)
      : m_pred( pred ), m_size( size ), m_bytesize( bytesize ), m_receiver_win( receiver_win ), m_fec_perc( fec_perc ), m_popularity_class ( popularity_class ) {}
    
    virtual ~PublishContentPacket() {}
    
    const Predicate& predicate() const
    { return m_pred; }

    unsigned int size() const
    { return m_size; }

    unsigned int bytesize() const
    { return m_bytesize; }
	    
    unsigned int receiver_win() const
    { return m_receiver_win; }
	    
    unsigned int fec_perc() const
    { return m_fec_perc; }
	    
    unsigned int popularity_class() const
    { return m_popularity_class; }

    bool is_predicate_null() const
	  {
	      return m_pred.bytesize() == 0;
	  }
  };
};
  
#endif
