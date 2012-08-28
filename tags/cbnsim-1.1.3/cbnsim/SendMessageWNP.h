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
// $Id: SendMessageWNP.h,v 1.4 2004/04/19 05:26:16 rutherfo Exp $
//
#ifndef CBNSIM_SENDMESSAGEWPN_H
#define CBNSIM_SENDMESSAGEWPN_H

#include <siena/ssbg.h>
#include "cbnsim/WorkloadNodeProcess.h"
#include "cbnsim/Crandom.h"

namespace cbnsim
{
  /** @brief Workload node process for the <code>send_message</code> event.
   */
  class SendMessageWNP : public WorkloadNodeProcess
    {
    public:
      /** @brief Constructor.
       */
	SendMessageWNP( unsigned int nid, std::ostream& out, ssbg::SSBG& ssbg, unsigned int delay,
			bool isconst, bool ispoisson, double change_interval, bool ispareto, double pareto_alpha);

      /** @brief Init method issues sets a timeout.
       */
      void init();

      /** @brief process_event method outputs a new send_message event
       * and resets its timeout.
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
      
      void print( const ssbg::notification& );

      unsigned int	   m_delay;
	ssbg::SSBG&	   m_ssbg;
	ssbg::notification m_notif;
	bool               m_const;
	bool               m_poisson;
        /** @brief for the options of 
         *  holding the content of msg during 
         *  certain duration of time interval  --liufeng 22/10/09
         */
	Crandom            m_random_num;
	double             m_change_interval;
	bool               m_pareto;
	double             m_pareto_alpha;
	double             m_change_pareto_interval;
	ssim::Time         m_time_last_change;
    };
};

#endif
