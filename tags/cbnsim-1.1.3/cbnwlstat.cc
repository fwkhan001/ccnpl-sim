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
// $Id: cbnwlstat.cc,v 1.3 2004/01/09 04:45:41 rutherfo Exp $
//
#include <iostream>
#include <fstream>
#include <siena/ssim.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "cbnsim/MatchHandler.h"
#include "cbnsim/NodeFailParser.h"
#include "cbnsim/SendMessageEvent.h"
#include "cbnsim/SendMessageParser.h"
#include "cbnsim/SetPredicateEvent.h"
#include "cbnsim/SetPredicateParser.h"
#include "cbnsim/DownloadContentEvent.h"
#include "cbnsim/DownloadContentParser.h"
#include "cbnsim/PublishContentEvent.h"
#include "cbnsim/PublishContentParser.h"
#include "cbnsim/WorkloadParser.h"
#include "cbnsim/logging.h"

using namespace cbnsim;
using namespace siena;
using namespace std;

void print_usage( const char* pname, int exit_val )
{
  if( exit_val )
    {
      cerr << "ERROR: invalid command line arguments" << endl;
    }
  cerr << "usage: " << pname << " -i <file> [options]" << endl << endl
       << "\t--help    print this help message" << endl << endl
       << "\t-i <file> input file file. this is required" << endl
       << "\t-m <file> output message distribution file." << endl
       << "\t          default is stdout." << endl
       << "\t-s <file> output workload statistics file." << endl
       << "\t          default is stdout." << endl
    ;
  exit( exit_val );
}

int main( int argc, char** argv )
{
  FILE *mfp = stdout, *sfp = stdout;
  string in_f;
  ifstream in;
  
  for( int i = 1; i < argc; ++i )
    {
      if( !strcmp( argv[i], "--help" ) )
	{
	  print_usage( argv[0], 0 );
	}
      else if( !strcmp( argv[i], "-i" ) )
	{
	  if( ++i >= argc )
	    {
	      print_usage( argv[0], 1 );
	    }
	  in.open( argv[i] );
	  if( !in )
	    {
	      cerr << "ERROR: unable to open '" << argv[i] << "' for reading" << endl;
	      return 1;
	    }
	  in_f = argv[i];
	}
      else if( !strcmp( argv[i], "-m" ) )
	{
	  if( ++i >= argc )
	    {
	      print_usage( argv[0], 1 );
	    }
	  mfp = fopen( argv[i], "w" );
	  if( !mfp )
	    {
	      cerr << "ERROR: unable to open '" << argv[i] << "' for writing" << endl;
	      return 1;
	    }
	}
      else if( !strcmp( argv[i], "-s" ) )
	{
	  if( ++i >= argc )
	    {
	      print_usage( argv[0], 1 );
	    }
	  sfp = fopen( argv[i], "w" );
	  if( !sfp )
	    {
	      cerr << "ERROR: unable to open '" << argv[i] << "' for writing" << endl;
	      return 1;
	    }
	}
      else
	{
	  print_usage( argv[0], 1 );
	}
    }
  if( ! in.is_open() )
    {
      cerr << "ERROR: input file must be specified" << endl;
      print_usage( argv[0], 1 );
    }

  try
    {
      WorkloadParser parser1;
      parser1.add( new SendMessageParser() );
      parser1.add( new SetPredicateParser() );
      parser1.add( new DownloadContentParser() );
      parser1.add( new PublishContentParser() );		
      parser1.set_istream( in );
//       parser1.parse_topology();

      /*
       * summary statistics variables
       */
      map<int, set<ssim::Time> > summary_pt;
      map<int, set<ssim::Time> > summary_st;

      /*
       * set up the forwarding table
       */
      FwdTable fwdTable;
      map<if_t,int> if_map;
      if_t next_if = 0;
      unsigned int pred_count = 0;
      const WorkloadEvent* event_ptr;
      while( (event_ptr = parser1.next_event()) )
	{
	  if( event_ptr->event_name() != "set_predicate" )
	    {
	      continue;
	    }
	  const SetPredicateEvent* spe = static_cast<const SetPredicateEvent*>( event_ptr );

	  fwdTable.ifconfig( next_if, spe->predicate() );
	  if_map[next_if++] = spe->node_id();
	  summary_st[spe->node_id()].insert( spe->timestep() );
	  pred_count++;
	}
      fwdTable.consolidate();

      in.close();
      
      ifstream in2( in_f.c_str() );

      // open a second parser to run through again.  do it this way
      // because trying to maintain a list of all predicates in memory
      // might get prohibitive.
      WorkloadParser parser2;
      parser2.add( new SendMessageParser() );
      parser2.add( new SetPredicateParser() );
      parser2.add( new DownloadContentParser() );	
      parser2.add( new PublishContentParser() );		
      parser2.set_istream( in2 );
//       parser2.parse_topology();

      /*
       * fwdTable is set up with each of the predicates associated with
       * interface. we just need to match each message and track how many
       * each one matches.
       */
      map<int,int> mdist_map;	// number of matches -> count

      unsigned int msg_count = 0;
      while( (event_ptr = parser2.next_event()) )
	{
	  if( event_ptr->event_name() != "send_message" )
	    {
	      continue;
	    }
	  const SendMessageEvent *sme = static_cast<const SendMessageEvent*>( event_ptr );
	  summary_pt[sme->node_id()].insert( sme->timestep() );

	  cbnsim::MatchHandler mh;
	  fwdTable.match( sme->message(), mh );
      
	  const set<if_t>& ids = mh.getIds();
	  mdist_map[ids.size()]++;
	  msg_count++;
	}
      /*
       * message fanout distribution
       */
      fprintf( mfp, "# message distribution data\n" );
      fprintf( mfp, "# %d predicates, %d publications\n",
	       pred_count, msg_count );
      fprintf( mfp, "# <percentage of matching predicates> <number of publications>\n" );
      map<int,int>::const_iterator j;
      for( j = mdist_map.begin(); j != mdist_map.end(); ++j )
	{
	  double f1 = 100.0 * (*j).first / double(pred_count);
	  fprintf( mfp, "%f\t%d\n", f1, (*j).second );
	}
      /*
       * summary statistics
       */
      double summary_st_total = 0;
      long int summary_st_count = 0;
      map<int,set<ssim::Time> >::const_iterator i;
      for( i = summary_st.begin(); i != summary_st.end(); ++i )
	{
	  set<ssim::Time>::const_iterator j;
	  ssim::Time last = 0, accum = 0;
	  for( j = (*i).second.begin(); j != (*i).second.end(); ++j )
	    {
	      accum += (*j) - last;
	      last = (*j);
	    }
	  summary_st_total += double(accum) / (*i).second.size();
	  summary_st_count += (*i).second.size();
	}
      double summary_pt_total = 0;
      long int summary_pt_count = 0;
      for( i = summary_pt.begin(); i != summary_pt.end(); ++i )
	{
	  set<ssim::Time>::const_iterator j;
	  ssim::Time last = 0, accum = 0;
	  for( j = (*i).second.begin(); j != (*i).second.end(); ++j )
	    {
	      accum += (*j) - last;
	      last = (*j);
	    }
	  summary_pt_total += double(accum) / (*i).second.size();
	  summary_pt_count += (*i).second.size();
	}
      fprintf( sfp, "publisher count : %d\n", summary_pt.size() );
      fprintf( sfp, "subscriber count : %d\n", summary_st.size() );
      fprintf( sfp, "pt count : %f\n", (summary_pt.size() ? double(summary_pt_count) / summary_pt.size() : 0.0) );
      fprintf( sfp, "st count : %f\n", (summary_st.size() ? double(summary_st_count) / summary_st.size() : 0.0) );
      fprintf( sfp, "pt : %f\n", (summary_pt.size() ? summary_pt_total / summary_pt.size() : 0.0) );
      fprintf( sfp, "st : %f\n", (summary_st.size() ? summary_st_total / summary_st.size() : 0.0) );
    }
  catch( parse_exception e )
    {
      cerr << "PARSE ERROR: " << e.what() << endl;
      return 1;
    }

  /*
   * clean up files (if necessary)
   */
  if( mfp != stdout )
    {
      fclose( mfp );
    }
  if( sfp != stdout )
    {
      fclose( sfp );
    }
  return 0;
}
