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
// $Id: SetPredicateWNP.h,v 1.4 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_SETPREDICATEWPN_H
#define CBNSIM_SETPREDICATEWPN_H

#include <siena/ssbg.h>
#include "cbnsim/WorkloadNodeProcess.h"

namespace cbnsim
{
  /** @brief Workload node process for generating the <code>set_predicate</code> event.
   */
  class SetPredicateWNP : public WorkloadNodeProcess
    {
    public:
      /** @brief Constructor.
       */
	SetPredicateWNP( unsigned int nid, std::ostream& out,
			 ssbg::SSBG& ssbg, unsigned int delay, bool t0_pred,
			 unsigned int msg_rate, bool ispoisson );

      /** @brief Init method issues a set_predicate event if the 
       * <code>t0_pred</code> is set.
       */
      void init();

      /** @brief process_event method outputs a new set_predicate
       * event and resets its timeout.
       */
      void process_event( const ssim::Event* evt );

    private:
      std::ostream & output_attr( std::ostream& out, 
					  const std::string& x, 
				     const std::string& o, 
				     const std::string& v)
	{
	  return out << x << ' ' << o << ' ' << '"' << v << '"';
	}

      template<class T>
	std::ostream & output_attr(std::ostream & out, 
					   const std::string & x, 
					   const std::string & o, 
					   T v )
	{
	  return out << x << ' ' << o << ' ' << v;
	}
      
      void print( const ssbg::predicate& );

	unsigned int	m_delay;
	ssbg::SSBG&	m_ssbg;
	bool		m_t0_pred;
	unsigned int    m_msg_rate;
	bool            m_poisson;
    };
};

#endif
