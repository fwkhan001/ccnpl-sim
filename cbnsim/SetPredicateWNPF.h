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
// $Id: SetPredicateWNPF.h,v 1.2 2004/02/03 06:16:21 hallcp Exp $
//
#ifndef CBNSIM_SETPREDICATEWNPF_H
#define CBNSIM_SETPREDICATEWNPF_H

#include "cbnsim/WorkloadNodeProcessFactory.h"

namespace cbnsim
{
  /** @brief Node process that generates set_predicate workload events.
   */
  class SetPredicateWNPF : public WorkloadNodeProcessFactory
    {
    public:
      /** @brief Constructor.
       */
      SetPredicateWNPF();

      /** @brief Returns <code>true</code> for <code>Subscriber_Node</code>
       * false otherwise.
       */
      bool applies_to( NodeType nt );

      /** @brief Handles <code>-t0-pred</code> and <code>-st <sec></code> arguments.
       */
      bool handle_argument( std::vector<std::string>::const_iterator& itr,
			    std::vector<std::string>::const_iterator end ) throw ( argument_exception );

      /** @brief Returns instances of <code>SetPredicateWNP</code>.
       */
      WorkloadNodeProcess* newWorkloadNodeProcess( unsigned int nid,
						   std::ostream& out,
						   ssbg::SSBG& ssbg,
						   double wtu ) throw ();

      /** @brief Abstract method to print out usage for optional arguments
       */
      virtual void print_optional_usage( std::ostream& out );

      /** @brief Abstract method to print out usage for required arguments
       */
      virtual void print_required_usage( std::ostream& out );

      /** @brief prints out the settings.
       */
      void print_setting_summary( std::ostream& out );

      /** @brief Abstract method useds to determine if a factory's 
       * required settings have been satisfied.
       */
      virtual bool satisfied() const;

    private:
	double		m_delay;
	bool		m_t0_pred;
	double          m_msg_rate;
	bool            m_poisson;
    };
};

#endif
