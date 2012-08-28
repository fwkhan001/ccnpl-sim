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
// $Id: test_CBNSimulation.cc,v 1.14 2004/08/27 01:16:16 rutherfo Exp $
//
#include <cassert>
#include <iostream>
#include "cbnsim/CBNSimulation.h"
#include "cbnsim/DeliveryOracle.h"
#include "cbnsim/NodeFailParser.h"
#include "cbnsim/SendMessageParser.h"
#include "cbnsim/SetPredicateParser.h"
#include "cbnsim/logging.h"

using namespace cbnsim;
using namespace ssim;
using namespace std;

class SampleMessagePacket : public Packet
{
  MessageId	m_mid;
  Message	m_message;
public:
  SampleMessagePacket( ProcessId sender, MessageId mid, const Message& message )
    : Packet( sender, false ), m_mid( mid ), m_message( message ) {}

  virtual ~SampleMessagePacket()
  {
  }

  const Message& message() const
  { return m_message; }

  MessageId mid() const
  { return m_mid; }
};

class SamplePredicatePacket : public Packet
{
  Predicate	m_predicate;
public:
  SamplePredicatePacket( ProcessId sender, const Predicate& predicate )
    : Packet( sender, true ), m_predicate( predicate ) {}

  const Predicate& predicate() const
  { return m_predicate; }
};

class SampleNode : public Node
{
  size_t	m_bytesize;
public:
  SampleNode( unsigned int id )
    : Node( id ), m_bytesize( 0 ) {}

  size_t bytesize() const
  { return m_bytesize; }

  bool has_failed() const
  {
    return false;
  }

  bool count_power() const
  {
    return false;
  }

  void node_fail()
  {
    CBNDEBUG( "Node[" << id() << "] received node_fail at " << ssim::Sim::clock() );
  }

  void process_packet( unsigned int ifid, const Packet& pkt )
  {
    CBNDEBUG( "Node[" << id() << "] received packet at " << ssim::Sim::clock()
	      << " from interface " << ifid );

    const SampleMessagePacket* smp;
    if( (smp = dynamic_cast<const SampleMessagePacket*>( &pkt )) )
      {
	// as an example: sometimes the message will be false negative,
	// sometimes false positive.
	if( rand() < (RAND_MAX/4) )
	  {
	    DeliveryOracle::log_false_delivery( smp->mid(), pid() );
	  }
	else
	  {
	    DeliveryOracle::log_true_delivery( smp->mid(), pid() );
	  }
      }
  }

  void send_message( MessageId mid, const Message& message )
  {
    CBNDEBUG( "Node[" << id() << "] received send_message " << ssim::Sim::clock()
	      << "    " << message.str() );
    m_bytesize += message.bytesize();

    SampleMessagePacket *p = new SampleMessagePacket( pid(), mid, message );
    for( unsigned int i = 1; i < neighbor_count(); ++i )
      {
	issue_packet( i, p );
      }
  }

  void set_predicate( const Predicate& predicate )
  {
    CBNDEBUG( "Node[" << id() << "] received set_predicate " << ssim::Sim::clock() 
	      << "    " << predicate.str() );
    m_bytesize += predicate.bytesize();

    SamplePredicatePacket *p = new SamplePredicatePacket( pid(), predicate );
    for( unsigned int i = 1; i < neighbor_count(); ++i )
      {
	issue_packet( i, p );
      }
  }
  
  void special_event()
  {
    // this is just here to be called by SampleDriver
  }

  int special_metric() const
  {
    // this is just here to be called by SampleDataCollector
    return rand();
  }
};

class SampleNodeFactory : public NodeFactory
{
public:
  Node* newNode( unsigned int i )
  {
    return new SampleNode( i );
  }
};

class SampleDriver : public Driver
{
public:
  void handle_event( Node& node, const NodeEvent& node_event )
  {
    SampleNode& sn = static_cast<SampleNode&>( node );
    // there is no such event in the workload, but you get the picture...
    if( node_event.event_name() == "special_event" )
      {
	sn.special_event();
      }
  }
};

// You can subclass DataCollector to collect data that are specific to
// the the Node implementation.
class SampleDataCollector : public DataCollector
{
  int  m_special_total;
public:
  SampleDataCollector() : m_special_total( 0 ) {}

  // poll_node is called for each node in the system when the
  // DataCollector timeout is tripped.
  void poll_node( Node& node )
  {
    const SampleNode& sn = static_cast<const SampleNode&>( node );
    m_special_total += sn.special_metric();
  }

  // output_inline is called once after all the nodes have been polled
  // and any special values that have been accumulated in the
  // poll_node method can be output here. NB: one should take care to
  // reset the variables used to accumulate values in once they have
  // been printed out.
  void output_inline( ostream& out, Time time )
  {
    out << "INL_SPECIAL_METRIC\t" << time << "\t" << m_special_total << endl;
    m_special_total = 0;
  }

  // output_final is called at the end of the simulation so that any 
  // final values can be printed out.
  void output_final( ostream& out )
  {
    out << "FINAL_SPECIAL_METRIC\tNan" << endl;
  }
};

int main( int argc, char** argv )
{
  assert( getenv( "srcdir" ) );
  string srcdir( getenv( "srcdir" ) );
  try
    {
      CBNSimulation sim;
      sim.add_event_parser( new NodeFailParser() );
      sim.add_event_parser( new SendMessageParser() );
      sim.add_event_parser( new SetPredicateParser() );

      // setting a custom data collector is optional
      sim.set_data_collector( new SampleDataCollector() );

      // setting a custom driver is optional
      sim.set_driver( new SampleDriver() );

      // a node factory must be supplied.
      sim.set_factory( new SampleNodeFactory() );

      sim.debug_messages( false );
      sim.dci( 60 );
      sim.input_fname( "test_CBNSimulation.wl" );
      sim.output_fname( "test_CBNSimulation.out" );
      sim.run();
    }
  catch( exception& e )
    {
      CBNERROR( "EXCEPTION: " << e.what() );
      return 1;
    }
  return 0;
}
