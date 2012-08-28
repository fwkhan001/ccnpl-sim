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
// $Id: DataCollector.cc,v 1.8 2005/08/04 14:19:01 hallcp Exp $
//
#include <stdexcept>
#include "cbnsim/DataCollector.h"
#include "cbnsim/DeliveryOracle.h"
#include "cbnsim/logging.h"
#include <iostream>
#include <cstdlib>

using namespace cbnsim;
using namespace ssim;
using namespace std;

void DataCollector :: finalize_polling(ssim::Time time)
{
  for( NodeList::iterator i = m_nodes->begin();i != m_nodes->end(); ++i )
    poll_node(*(*i),time);

  output_final( (*m_out),time );
}

void DataCollector :: finalize(ssim::Time time)
{
  output_final( (*m_out),time);
}

void DataCollector :: init()
{
  if( m_timeout )
    {
      process_event( NULL );
    }
}

void DataCollector :: process_event( const ssim::Event* evt )
{
  CBNASSERT( evt == NULL, "DataCollector only knows about timeouts" );

  Time cl = Sim::clock();

  // loop across nodes 
  unsigned int ctrl_count( 0 ), data_count( 0 ), num_power_nodes(0);
  power_type power_left( 0 );
  size_t mem_usage( 0 );
  for( NodeList::iterator i = m_nodes->begin();i != m_nodes->end(); ++i )
    {
      // packet counts
      ctrl_count += (*i)->control_packets();
      data_count += (*i)->data_packets();
      (*i)->reset_packet_counters();

      // memory usage
      mem_usage += (*i)->bytesize();
      if((*i)->count_power()) {
	  power_left += (*i)->power_left();
	  num_power_nodes++;
      }

      // deletage to derived class
      poll_node( *(*i), cl );
    }

  if(num_power_nodes)
      power_left = power_left / num_power_nodes;

  /*
   * packet counts
   */
  unsigned int packet_count = ctrl_count + data_count;
  
  double ctrl_percent = 0, data_percent = 0;
  if( packet_count )
    {
      ctrl_percent = 100.0 * ctrl_count / double(packet_count);
      data_percent = 100.0 * data_count / double(packet_count);
    }
  
  /*(*m_out) << "INL_CTRL_PKT_CNT\t" << cl << "\t" << ctrl_count << endl
	   << "INL_DATA_PKT_CNT\t" << cl << "\t" << data_count << endl
	   << "INL_TOTL_PKT_CNT\t" << cl << "\t" << packet_count << endl
	   << "INL_CTRL_PKT_PCT\t" << cl << "\t" << ctrl_percent << endl
	   << "INL_DATA_PKT_PCT\t" << cl << "\t" << data_percent << endl
	   << "INL_MEMORY_USAGE\t" << cl << "\t" << mem_usage << endl
	   << "INL_FP_PERCENT  \t" << cl << "\t" << DeliveryOracle::false_positive_percent() << endl
	   << "INL_FN_PERCENT  \t" << cl << "\t" << DeliveryOracle::false_negative_percent() << endl
	   << "INL_AVG_POWER   \t" << cl << "\t" << power_left << endl
    ;*/

  DeliveryOracle::update_dci_windows();

    // delegate to derived class
    output_inline( (*m_out), cl );

    // if we are in this method, timeout must be > 0
    Sim::self_signal_event( NULL, m_timeout );
}
