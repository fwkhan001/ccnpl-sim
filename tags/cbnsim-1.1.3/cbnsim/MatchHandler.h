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
// $Id: MatchHandler.h,v 1.2 2004/01/08 07:01:50 rutherfo Exp $
//
#ifndef CBNSIM_MATCHHANDLER_H
#define CBNSIM_MATCHHANDLER_H

#include <set>
#include <siena/forwarding.h>

namespace cbnsim
{
  /** @brief Concrete implementation of <code>siena::MatchHandler</code>.
   *
   * This implementation accumulates the matching ids in its internal data
   * structure.
   */
  class MatchHandler : public siena::MatchHandler
  {
    std::set<siena::if_t>	m_ids;
    unsigned int		m_match_len;

  public:
    MatchHandler();

    MatchHandler( const MatchHandler& mh );

    virtual ~MatchHandler();

    const MatchHandler& operator= ( const MatchHandler& mh );

    bool empty() const;

    const std::set<siena::if_t>& getIds() const;
    
    unsigned int match_len(){return(m_match_len);}
    
    virtual void match_len(unsigned int len){m_match_len=len;};

    virtual bool output( siena::if_t id ) throw ();
  };
};

#endif
