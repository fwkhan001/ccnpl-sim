// -*- C++ -*-
//
//  This file is part of CCN-PLsim, a CCNx simulator
//
//  Authors: See the file AUTHORS for full details. 
//
//  Copyright (C) 2012 France Telecom R&D
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
//  USA, or send email to luca.muscariello@orange.com 
//
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cbnsim/argument_exception.h>
#include "cbcbsim/CBCBSimulation.h"
#include <signal.h>

using namespace cbcbsim;
using namespace cbnsim;
using namespace ssim;
using namespace std;

void parse_args( CBCBSimulation& sim, int argc, char** argv ) throw ( argument_exception );
int parse_input(string input_file);
void print_usage();
void print_version();
void print_example();


CBCBSimulation sim;


int main( int argc, char** argv )
{
  try
  {
    parse_args( sim, argc, argv );
    sim.run();
  }
  catch( argument_exception& e )
  {
    cerr << "INVALID ARGUMENT: " << e.what() << endl;
    print_usage();
    return 1;
  }
  catch( std::exception& e )
  {
    cerr << "EXCEPTION: " << e.what() << endl;
    return 1;
  }
  return 0;
}

int parse_input(string input_file)
{
  ifstream ifs_files(input_file.c_str());
  if (!ifs_files.is_open())
    return 0;
  unsigned int line_num=0;
  string line;

  while( !ifs_files.eof() ){
    line_num++;
    std::stringstream ss;
    std::string s;
    ss << line_num;
    s = ss.str();
    string line_num_str = input_file+", line "+s+" :" ;

    getline(ifs_files,line);    
    if (line.size()==0)
      continue;
    
    char* argument;
    vector<string> line_tok;
    string delim=" ";
    argument = strtok((char *)line.c_str(),delim.c_str());

    if ((line.size()==0) || ( argument[0] == '#') || (argument == NULL) )
      continue;
/*----------------------------------------------DCI----------------------------------------------------------*/	
    if(!strcmp(argument,"dci")){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + string (": parameter 'dci' expects an argument" ));  
	return 0;
      }
      sim.dci( atof( argument ) );
      continue;
    }
/*------------------------------------------sim length--------------------------------------------------------*/	
    if(!strcmp(argument,"sim_length")){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + string ("parameter 'sim_length' expects an argument" ));
	return 0;
      }
      sim.sim_length( atof( argument ) );
      continue;
    }
/*--------------------------------------------output---------------------------------------------------------*/	
    if(!strcmp(argument,"output")){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + string ("parameter 'output' expects an argument" ));
	return 0;
      }
      sim.output_fname( argument );
      continue;
    }
/*---------------------------------------queue discipline-----------------------------------------------------*/	
    if(!strcmp(argument,"queue_discipline")){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + string ("parameter 'queue_discipline' expects an argument" ) );
	return 0;
      }
      if(!strcmp(argument,"FIFO")){
	sim.qd( ProtocolOptions::QD_FIFO );
	continue;
      }
      if(!strcmp(argument,"DRR")){
	sim.qd( ProtocolOptions::QD_DRR );
	continue;
      }
      throw argument_exception( line_num_str + string("unrecognized 'queue_discipline' flag: ") + string(argument) );
      return 0;
    }
/*---------------------------------------cache policy-------------------------------------------------------*/	
    if(!strcmp(argument,"cache")){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'cache' expects an argument spcifying the policy" );
	return 0;
      }
      if(!strcmp(argument,"LRU")){
	sim.cache( ProtocolOptions::CACHING_ON_LRU );
	goto cache_size;
      }
      if(!strcmp(argument,"RND")){
	sim.cache( ProtocolOptions::CACHING_ON_RND );
	goto cache_size;
      }
      if(!strcmp(argument,"AUTO")){
	sim.cache( ProtocolOptions::CACHING_ON_AUTO );  
	continue;
      }
      if(!strcmp(argument,"NO")){
	sim.cache( ProtocolOptions::CACHING_OFF );
	sim.cache_limit( 0 );
	continue;
      }
      throw argument_exception( line_num_str + string("unrecognized 'cache' flag: ") + string(argument) + string("([LRU/RND/AUTO/NO] accepted)" ));
      return 0;
	
    cache_size:
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'cache LRU/RND' expects an argument spcifying the cache size" );
	return 0;
      }
      sim.cache_limit( atof( argument ) );  
      continue;
    }
/*---------------------------------------startup time-------------------------------------------------------*/	
    if(!strcmp(argument,"startup") ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'startup' expects an argument spcifying the startup time in " );
	return 0;
      }
      sim.startup( atof( argument ) );
      continue;
    }
/*---------------------------------------request order------------------------------------------------------*/	
    if(!strcmp(argument,"req_order")){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'reqest_order' expects an argument spcifying the req. order [SEQ/RND]" );
	return 0;
      }
      if (!strcmp(argument,"SEQ")){
	sim.request_order( ProtocolOptions::INSEQUENCE );
	continue;
      }
      if (!strcmp(argument,"RND")){
	sim.request_order( ProtocolOptions::RANDOM );
	continue;
      }
      throw argument_exception( string( "unrecognized req_order flag: " + string( argument ) + string("([SEQ/RND] accepted)") ) );
      return 0;
    }
/*---------------------------------------flow controller----------------------------------------------------*/	
      if( !strcmp( argument, "flow_controller" ) ){
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller' expects an argument spcifying the controller type" );
	  return 0;
	}
	if (!strcmp(argument,"FIX")){
	  sim.rbfcp( ProtocolOptions::RBFCP_ON );
	  goto icp;
	}
	if (!strcmp(argument,"ICP")){
	  sim.rbfcp( ProtocolOptions::RBFCP_AIMD );
	  goto icp;
	}
	if (!strcmp(argument,"RAQM")){
	  sim.rbfcp( ProtocolOptions::RBFCP_AIMD );
	  goto raqm;
	}
	if (!strcmp(argument,"CBR")){
	  sim.rbfcp( ProtocolOptions::RBFCP_ON );
	  sim.fc_type(ProtocolOptions::fc_CBR);
	  goto cbr;
	}
      
      icp: 
	sim.fc_type(ProtocolOptions::fc_ICP);
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller FIX/ICP' expects three arguments spcifying W size, Rx timer (type and value) and max req. re-expression" );
	  return 0;
	}
	sim.rbfcp_win( atoi( argument ) );
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller FIX/ICP' expects three arguments spcifying W size, Rx timer (type and value) and max req. re-expression" );
	  return 0;
	}
	if (!strcmp(argument,"var")){
	  sim.to( ProtocolOptions::TO_V );
	  goto timers_reqlimit_icp;
	}
	if (!strcmp(argument,"fix")){
	  sim.to( ProtocolOptions::TO_ON );
	  goto timers_reqlimit_icp;
	}
	throw argument_exception( string( "unrecognized flow_controller FIX/ICP #size flag: " + string( argument ) + string("([var/fix] accepted)") ) );
	return 0;
      timers_reqlimit_icp:
	 if ((argument = strtok(NULL,delim.c_str())) == NULL){
	   throw argument_exception( line_num_str + "parameter 'flow_controller FIX/ICP' expects three arguments spcifying W size, Rx timer (type and value) and max req. re-expression" );
	   return 0;
	 }
	 sim.to_sender( atof( argument ) );
	 if ((argument = strtok(NULL,delim.c_str())) == NULL){
	   throw argument_exception( line_num_str + "parameter 'flow_controller FIX/ICP' expects three arguments spcifying W size, Rx timer (type and value) and max req. re-expression" );
	   return 0;
	 }
	  sim.to_limit( atoi( argument ) );
	  continue;
      raqm:
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller RAQM' expects five arguments specifying pmax, beta, W size, max req. re-expression and 'source/no_source' to specify which protocol you want to use" );
	  return 0;
	}       
	sim.pmax(atof(argument));
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller RAQM' expects five arguments specifying pmax, beta, W size, max req. re-expression and 'source/no_source' to specify which protocol you want to use" );
	  return 0;
	}
	sim.beta(atof(argument));
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller RAQM' expects five arguments specifying pmax, beta, W size, max req. re-expression and 'source/no_source' to specify which protocol you want to use" );
	  return 0;
	}
	sim.rbfcp_win( atoi( argument ) );
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller RAQM' expects five arguments specifying pmax, beta, W size, max req. re-expression and 'source/no_source' to specify which protocol you want to use" );
	  return 0;
	}
	sim.to_limit(atof(argument));
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception(line_num_str + "parameter 'flow_controller RAQM' expects five arguments specifying pmax, beta, W size, max req. re-expression and 'source/no_source' to specify which protocol you want to use" );
	  return 0;
	}
	if (!strcmp(argument,"source")){
	  sim.fc_type( ProtocolOptions::fc_RAQMsource );
	  continue;
	}
	if (!strcmp(argument,"no_source")){
	  sim.fc_type( ProtocolOptions::fc_RAQM );
	  continue;
	}
	throw argument_exception( string( "unrecognized RAQM type #size flag: " + string( argument ) + string("([source/no_source] accepted)") ) );
	return 0;
      cbr:
	sim.to_limit(1);
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller CBR' expects two arguments specifying W size and request rate (times per second)" );
	  return 0;
	}
	sim.rbfcp_win( atoi( argument ) );
	if ((argument = strtok(NULL,delim.c_str())) == NULL){
	  throw argument_exception( line_num_str + "parameter 'flow_controller CBR' expects two arguments specifying W size and request rate (times per second)" );
	  return 0;
	}
	sim.cbr_rate( atoi( argument ) );
	continue;
    }
/*---------------------------------------PIT timeout--------------------------------------------------------*/	
    if( !strcmp( argument, "PIT_timer" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'PIT_timer' expects an argument spcifying the timer value" );
	return 0;
      }
      sim.to_router( atof( argument ) );
      continue;
    }
/*---------------------------------------filtering----------------------------------------------------------*/	
    if( !strcmp( argument, "filtering" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'filtering' expects an argument [YES/NO]" );
	return 0;
      }
      if (!strcmp(argument,"YES")){
	sim.filtering( ProtocolOptions::FILTERING_ON );
	continue;
      }
      if (!strcmp(argument,"NO")){
	sim.filtering( ProtocolOptions::FILTERING_OFF );
	continue;
      }
      throw argument_exception( string( "unrecognized filtering flag: " + string( argument ) + string("([YES/NO] accepted)") ) );
      return 0;
    }
/*---------------------------------------workload file------------------------------------------------------*/	
    if( !strcmp( argument, "workload" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'workload' expects an argument specifying the file name" );
	return 0;
      }
      sim.wl_fname( argument );
      continue;
    }
/*---------------------------------------classes stat--------------------------------------------------------*/	
    if( !strcmp( argument, "classes_stat" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'classes_stat' expects an argument specifying the max class number for wich making stats" );
	return 0;
      }
      sim.n_classes( atoi(argument) );
      continue;
    }
/*---------------------------------------classes stat--------------------------------------------------------*/	
    if( !strcmp( argument, "routing" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'routing' expects an argument specifying the input file name for manual routing" );
	return 0;
      }
      sim.routing_fname( argument );
      continue;
    }
/*--------------------------------------forwarding----------------------------------------------------------*/	
    if( !strcmp( argument, "forwarding" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'forwarding' expects an argument specifying interest forwarding type [RND/BALANCED]" );
	return 0;
      }
      if (!strcmp(argument,"RND")){
	sim.forwarding( ProtocolOptions::FWD_RND );
	continue;
      }
      if (!strcmp(argument,"BALANCE")){
	sim.forwarding( ProtocolOptions::FWD_BALANCE );
	continue;
      }
      //TODO modify specifying balanced pending interest or balanced delay 
      throw argument_exception( line_num_str + string( "unrecognized forwarding flag: " + string( argument ) + string("([RND/BALANCE] accepted)") ) );
      return 0;
    }
    /*---------------------------------------classes stat--------------------------------------------------------*/	
    if( !strcmp( argument, "topology" ) ){
      if ((argument = strtok(NULL,delim.c_str())) == NULL){
	throw argument_exception( line_num_str + "parameter 'topology' expects an argument specifying the input file name for the topology" );
	return 0;
      }
      sim.topo_fname( argument );
      continue;
    }
    
   
    
/*--------------------------------------unrecognised----------------------------------------------------------*/   
    throw argument_exception( line_num_str + string( "unrecognized option: " + string( argument )  ) );
    return 0;
  }
}


void parse_args( CBCBSimulation& sim, int argc, char** argv ) throw ( argument_exception )
{
  string input_file;
  for( int i = 1; i < argc; ++i )
  {
    if( !strcmp( argv[i], "-h" ) ){
      print_usage();
      exit( 0 );
    }
    if( !strcmp( argv[i], "-e" ) ){
      print_example();
      exit( 0 );
    }
    if( !strcmp( argv[i], "-input" ) ){
      if( ++i == argc )
	throw argument_exception( "command-line argument '-input' expects an argument specifying the input file name" );  
      parse_input( argv[i] );
    }
    else
      throw argument_exception( string( "unrecognized command-line argument: " ) + string( argv[i] ) );
  }
}

void print_example()
{
  cout  << "dci                  time                               	collect and print some stats in outputfile every [time] seconds" << endl
        << "sim_length           time                               	sim length expressed in sec (override the length written in the workload)" << endl
        << "output               outputfile                         	output file " << endl
        << "queue_discipline     FIFO/DRR                           	queuing discipline, FIFO or DRR (applied at each node)" << endl
        << "cache                AUTO/LRU/RND/NO [cache_size]       	caching policy, auto means take cache policy from topology file; [cache size] in kbit" << endl
	<< "startup              time                               	does not collect stats for [time] seconds" << endl
	<< "req_order            SEQ/RND                            	chunk request order (SEQ = 1,2,3,...N or RND)" << endl
	<< "flow_controller      ICP/FIX win var/fix timer limit    	flow controller (applied at each download)," << endl
	<< "                                                        	Interest window ICP(AIMD) or fix, with size [win], var/fix receivertime "<< endl
	<< "                                                        	out of [timer] seconds, limit indicates the max number of interest"<< endl
	<< "                                                        	re-expressions allowed " << endl
	<< "		     RAQM pmax beta win limit source/no_source	Interest window RAQM(AIMD) with max probability decrese [pmax] and decrease"<<endl 
	<< "								factor [beta]. [win] is the initial window size and [limit] indicates the max"<<endl 
	<< "								number of interest re-expressions allowed. [source/no_source] indicate if RAQM"<<endl
	<< "  								collects global RTT stats or if it divides the samples per source"<<endl                                                       	  
	<< "		     CBR win rate				Controller with fix window size [win]. Sends an entire window of interests"<<endl
	<< "								[rate] times per second. No interest re-expression is allowed"<<endl
	<< "PIT_timer            timer                              	Pending Interest Table timer of  [timer] seconds" << endl
	<< "filtering            YES/NO                             	enable filtering in the  PIT table (applied at each node)" << endl
	<< "workload             fileneame.wl                       	workload of the simulation" << endl
	<< "classes_stat         class                              	print out statistics for the fisrt [class] popularity classes" << endl
	<< "routing              routing.dist                       	manual routing file" << endl
	<< "forwarding           RND/BALANCE                        	forwarding policy RND or balanced"<< endl
	<< "topology             filename.brite                     	topology filename" << endl;
}

void print_usage()
{
  cerr << "cbcbsim [options]" << endl << endl
       << " -h                                                print this help message." << endl  
       << " -e	                                              print the input file syntax." << endl  
       << " -input        inputfile                           inputfile.dat specifying the simulator's options" << endl
       << endl;
}

void print_version()
{
  cout << "cbcbsim v" << VERSION
       << " (compiled: " << __DATE__
       << " " << __TIME__
       << ")"
       << endl;
}

