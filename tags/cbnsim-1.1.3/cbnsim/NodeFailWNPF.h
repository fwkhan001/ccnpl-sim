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
// $Id: NodeFailWNPF.h,v 1.2 2004/02/02 00:23:25 hallcp Exp $
//
#ifndef CBNSIM_NODEFAILWNPF_H
#define CBNSIM_NODEFAILWNPF_H

#include "cbnsim/WorkloadNodeProcessFactory.h"

namespace cbnsim
{
  /** @brief Node process that generates node_fail workload events.
   */
  class NodeFailWNPF : public WorkloadNodeProcessFactory
    {
    public:
      /** @brief Constructor.
       */
      NodeFailWNPF();

      /** @brief Returns <code>true</code> for all node types.
       */
      bool applies_to( NodeType nt );

      /** @brief Handles <code>-ft <sec></code> arguments.
       */
      bool handle_argument( std::vector<std::string>::const_iterator& itr,
			    std::vector<std::string>::const_iterator end ) throw ( argument_exception );

      /** @brief Returns instances of <code>NodeFailWNP</code>.
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
	double          m_recover_delay;
	double          m_final_delay;
    };
};

#endif
