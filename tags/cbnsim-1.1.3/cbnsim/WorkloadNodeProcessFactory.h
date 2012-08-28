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
// $Id: WorkloadNodeProcessFactory.h,v 1.2 2004/01/28 02:18:58 hallcp Exp $
//
#ifndef CBNSIM_WORKLOADNODEPROCESSFACTORY_H
#define CBNSIM_WORKLOADNODEPROCESSFACTORY_H

#include <siena/ssbg.h>
#include <vector>
#include "cbnsim/WorkloadNodeProcess.h"
#include "cbnsim/argument_exception.h"

namespace cbnsim
{
    /** @brief Factory class for WorkloadNodeProcess instances.
     */
    class WorkloadNodeProcessFactory
    {
    private:
	std::string	m_event_name;

    protected:
	/** @brief Derived classes must provide an event name during
	 * construction.
	 */
	WorkloadNodeProcessFactory( const std::string& event_name )
	    : m_event_name( event_name ) {}

    public:
	/** @brief Enumeration of different node types.
	 */
	enum NodeType { Publisher_Node, Subscriber_Node, Any_Node };

	virtual ~WorkloadNodeProcessFactory() {}

	/** @brief Abstract method that allows the factory to determine
	 * what kinds of node types its generated WorkloadNodeProcess
	 * applies to.
	 */
	virtual bool applies_to( NodeType nt ) = 0;

	/** @brief Returns the event name supplied to the constructor.
	 */
	const std::string& event_name() const
	    { return m_event_name; }

	/** @brief Abstract method to allow the factory to handle a
	 * command line argument. The implementation should return
	 * true if it handled the argument, false otherwise.
	 */
	virtual bool handle_argument( std::vector<std::string>::const_iterator& itr,
				      std::vector<std::string>::const_iterator end ) throw ( argument_exception ) = 0;

	/** @brief Abstract factory method to return a newly allocated 
	 * WorkloadNodeProcess.
	 *
	 * The node id of the node to be associated with the new
	 * process.
	 */
	virtual WorkloadNodeProcess* newWorkloadNodeProcess( unsigned int nid, ostream& out, ssbg::SSBG& ssbg, double wtu ) throw () = 0;

	/** @brief Abstract method to print out usage for optional arguments
	 */
	virtual void print_optional_usage( std::ostream& out ) = 0;

	/** @brief Abstract method to print out usage for required arguments
	 */
	virtual void print_required_usage( std::ostream& out ) = 0;

	/** @brief Abstract method to print out a summary of the factories settings.
	 *
	 * Lines should begin with a '#' character.
	 */
	virtual void print_setting_summary( std::ostream& out ) = 0;

	/** @brief Abstract method useds to determine if a factory's 
	 * required settings have been satisfied.
	 */
	virtual bool satisfied() const = 0;
    };
};

#endif
