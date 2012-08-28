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
// $Id: WorkloadGenerator.cc,v 1.4 2005/08/04 14:19:01 hallcp Exp $
//
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <set>
#include <siena/ssbg.h>
#include <stdexcept>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <sstream>



#include <config.h>

#include "cbnsim/KillerProcess.h"
#include "cbnsim/WorkloadGenerator.h"

#define DEFAULT_DIST_DIR PREFIX "/share/" PACKAGE "-" VERSION "/dist"
#define DEFAULT_TOPO_DIR PREFIX "/share/" PACKAGE "-" VERSION "/topo"

using namespace cbnsim;
using namespace std;

WorkloadGenerator :: WorkloadGenerator( ostream& out )
    : m_attr_max( 10 ),
      m_attr_min( 1 ),
      m_constr_max( 5 ),
      m_constr_min( 1 ),
      m_dist_dir( DEFAULT_DIST_DIR ),
      m_filt_max( 20 ),
      m_filt_min( 0 ),
      m_out( out ),
      m_p_cmd( "0" ),
      m_reuse_types( false ),
      m_s_cmd( "0" ),
      m_sim_length( 0 ),
      m_wtu_sec( 1.0 ),
      m_file_per_class( 1 )
     
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_seed = tv.tv_usec;
}

WorkloadGenerator :: ~WorkloadGenerator()
{
    for( vector<WorkloadNodeProcessFactory*>::const_iterator i = m_npfs.begin();
	 i != m_npfs.end(); ++i )
    {
	delete (*i);
    }
}

void WorkloadGenerator :: add( WorkloadNodeProcessFactory* wnpf )
{
    m_npfs.push_back( wnpf );
}



int WorkloadGenerator :: set_times( ) 
{  
  srand( m_seed );
  time=0.0;
  /*
    * calculate the number of timesteps in the simulation
    */
  ts = m_sim_length * (uint)(1 / m_wtu_sec);
  /*
    * set some output flags.
    */
  m_out << showpoint;
  m_out << boolalpha;
  
  /*
    * output the time unit and the sim length
    */
  m_out << "time_unit " << setiosflags(ios::fixed) << setprecision(10) << m_wtu_sec << " ;" << endl;
  m_out << "sim_length " << ts << " ;" << endl;
  return 0;
}

int WorkloadGenerator :: publishing_content()
{
  string line;

  ifstream ifs_files(m_files_file.c_str());
  double old_class_prob=0;
  unsigned int n_file_class=0;
  std::string previous="";
  unsigned int file_class=0;
  string node="";
    

  if (!ifs_files.is_open()){
    m_out << "ERROR loading contents' file" << endl;
    return 1;
  }
  
  while( !ifs_files.eof() )
  {
    getline(ifs_files,line);
    
    if (line.size()==0)   /** Skip empty or comment lines*/ 
      continue;
    
    /** read input */
    char* read;
    string delim=" ";
    vector<string> line_tok;
    read = strtok((char *)line.c_str(),delim.c_str());
    while (read != NULL){
      line_tok.push_back(string(read));   
      read = strtok(NULL,delim.c_str());
    }           
    
    /** parse input */
    double class_prob = atof(line_tok[0].c_str());
    double file_prob = atof(line_tok[1].c_str());
    unsigned int size = atoi(line_tok[2].c_str());
    unsigned int chunksize = atoi(line_tok[3].c_str());
    int node = atoi(line_tok[4].c_str());
    
    /** obsolete parameters */
    unsigned int fec_perc =  atoi(line_tok[5].c_str()); //unused
    unsigned int fec_tot =  atoi(line_tok[6].c_str()); //unused
    unsigned int win_size =  atoi(line_tok[7].c_str()); //unused

    /** check parsed input */
/*    if (topo_node_coords.find(node)==topo_node_coords.end()){
      m_out << "ERROR in contents' file, node " << node << " does not exist" << endl;
      return 1;
    }    */  
    
    m_files_token.push_back(line_tok);
    
    if (class_prob != old_class_prob){
      file_class++;
    }
    old_class_prob=class_prob;
    
    m_out << "publish_content "<< node << " " <<  time << " " <<  size << " " << chunksize << " " << file_class << " ";
    for (unsigned int j=6; j < line_tok.size();j++)
      m_out << line_tok[j] << " ";
    m_out << endl;
  }
  
  return 0;
}

int WorkloadGenerator :: read_clients()
{
  ifstream ifs_clients(m_clients_file.c_str());
  string line;
  
  if (!ifs_clients.is_open()){
    m_out << "ERROR loading clients' file" << endl;
    return 1;
  }
  
  while( !ifs_clients.eof() )
  {
    getline(ifs_clients,line);
    
    if (line.size()==0)   /** Skip empty or comment lines*/ 
      continue;
    
    /** read input */
    char* read;
    string delim=" ";
    vector<string> line_tok;
    read = strtok((char *)line.c_str(),delim.c_str());
    while (read != NULL){
      line_tok.push_back(string(read));   
      read = strtok(NULL,delim.c_str());
    }
 
    /** parse input */
    if (line_tok.size() != 3){
      m_out << "ERROR in client file, malformed input" << endl;
      return 1;
    }
    
    int node = atoi(line_tok[0].c_str());
    string distrib = line_tok[1];
    double rate = atof(line_tok[2].c_str());
    
    /** check parsed input */
/*    if (topo_node_coords.find(node)==topo_node_coords.end()){
      m_out << "ERROR in client file, node " << node << " does not exist" << endl;
      return 1;
    }     */ 
    if ((distrib != "Poisson") && (distrib != "poisson")){
      m_out << "ERROR in client file,  " << distrib << " not supported" << endl;
      return 1;
    }
    
    client new_cl={distrib,rate};
    m_client_map.insert(make_pair(node,new_cl));

  }
  return 0;
}

int WorkloadGenerator :: init_routing()
{
  string line;
  
  time += (0/m_wtu_sec);
    
  ifstream ifs_prefix(m_prefix_file.c_str());
  if (!ifs_prefix.is_open()){
      m_out << "ERROR loading prefix's file" << endl;
      return 1;
  }
    
  while( !ifs_prefix.eof() )
  {
    getline(ifs_prefix,line);

    if (line.size()==0)   /** Skip empty or comment lines*/ 
      continue;
    
    /** read input */
    char* read;
    string delim=" ";
    vector<string> line_tok;
    read = strtok((char *)line.c_str(),delim.c_str());
    while (read != NULL){
      line_tok.push_back(string(read));   
      read = strtok(NULL,delim.c_str());
    }
    
    /** parse input */
    int node = atoi(line_tok[0].c_str());
    stringstream prefix_server;    


    for(unsigned int i=1;i<line_tok.size(); i+=3){
      prefix_server << line_tok[i] << " " << line_tok[i+1] << " " << line_tok[i+2] << " ;" << endl; 
    }
    
/*    if (topo_node_coords.find(node)==topo_node_coords.end()){
      m_out << "ERROR in prefix file, node " << node << " does not exist" << endl;
      return 0;
    }*/  
    m_out << "set_predicate " << node << " " << time << " 0 " << prefix_server.str();
  }
  time += (1/m_wtu_sec);

// TODO verify multipath
//   if(m_multipath.compare("y")==0)
//   {
//     //Enable multipath
//     for (std::map<int,string>::iterator it_server = m_server_map.begin(); it_server != m_server_map.end(); it_server++)
//     {
//       for (std::map<int,client>::iterator it_client = m_client_map.begin(); it_client != m_client_map.end();it_client++ )
//       {
// 	m_out << "set_multipath " << (*it_server).first << " " << (*it_client).first  << " " << time << " " << (*it_server).second << endl;
//       }
//     }
//   }
  return 0; 
}

int WorkloadGenerator :: create_input_process() 
{ 
  std::map<int,client>::iterator it;

  double lambda_input;//sum of the rate of the different poisson processes
  
  if (read_clients()!=0)
    return 1;

  for (it = m_client_map.begin(); it!=m_client_map.end(); it++){
    if (( it->second.distribution == "poisson" ) || ( it->second.distribution == "Poisson" ))
      lambda_input += it->second.rate; 
  }
  time+=(0.5/m_wtu_sec);
  
  while(true)
  {
    double poisson_interval = ssbg::rand_poisson_delta( (1/lambda_input)/m_wtu_sec ); //gnerate an exponential interval
    time+=poisson_interval;
    
    if ( time > ts)
      return 1;
    
    //random node (according to the rate of the poisson processes)
    double rnd_node = ((double)random()/(double)RAND_MAX);
    double cum = 0.0;
    int node=-1;
    it = m_client_map.begin();
    while(it!=m_client_map.end()){
      cum+=(it->second.rate/lambda_input);
      if(rnd_node < cum ){
	node = it->first;
	break;
      }
      it++;
    }
    
    /**randomize class and file */
     double rnd_class = ((double)random()/(double)RAND_MAX);
     double rnd_file = ((double)random()/(double)RAND_MAX);
     
     unsigned int r =0;
     while (rnd_class > ( atof(m_files_token[r][0].c_str())) ) {
       r++;
       if (r >= m_files_token.size())
	 break;
    }
    
    while (rnd_file > ( atof(m_files_token[r][1].c_str())) ){ 
      r++;
      if (r >= m_files_token.size())
	break;
    }
    
    /**write the download process */
    m_out <<"download_content " << node << " ";
    m_out << setiosflags(ios::fixed) << setw(10) << time << " 0 ";
    if (r < m_files_token.size())
      for (unsigned int j=8; j < m_files_token[r].size();j++)
	m_out << m_files_token[r][j]<< " ";
    m_out << endl; 
  }

  return 1;
}

int WorkloadGenerator :: main( const string& prog, const vector<string>& args ) throw ( exception )
{
    try {
      parse_args( args );
    }
    catch( argument_exception e ){
      cerr << "ERROR: INVALID ARGUMENT: " << e.what() << endl;
      print_usage();
      return 1;
    }
    if(set_times()!=0)
      return 1;
    if(publishing_content()!=0)
      return 1;
    if(!m_man_routing)
    {
      if(init_routing()!=0)
	return 1;
    }
    if(create_input_process()!=0)
      return 1;
    return 0;
}

void WorkloadGenerator :: parse_args( const vector<string>& args ) throw ( argument_exception )
{
    m_man_routing = false;
    for( vector<string>::const_iterator i = args.begin(); i != args.end(); ++i )
    {
	if( (*i) == "-seed" )
	{
	    if( ++i != args.end() )
	    {
		m_seed = atol( (*i).c_str() );
	    }
	    else
	    {
		throw argument_exception( "-seed requires an argument" );
	    }
	}
	else if( (*i) == "-l" )
	{
	    if( ++i != args.end() )
	    {
		m_sim_length = atof( (*i).c_str() );
	    }
	    else
	    {
		throw argument_exception( "-l requires an argument" );
	    }
	}
	else if( (*i) == "-wtu" )
	{
	    if( ++i != args.end() )
	    {
		m_wtu_sec = atof( (*i).c_str() );
	    }
	    else
	    {
		throw argument_exception( "-wtu requires an argument" );
	    }
	}  
	else if( (*i) == "-files" )
	{
	    if( ++i != args.end() )
	    {
		m_files_file = (*i);
	    }
	    else
	    {
		throw argument_exception( "-files requires an argument" );
	    }
	}  
	else if( (*i) == "-prefix" )
	{
	    if( ++i != args.end() )
	    {
		m_prefix_file = (*i);
	    }
	    else
	    {
		throw argument_exception( "-prefix requires an argument" );
	    }
	}    
	else if( (*i) == "-clients" )
	{
	    if( ++i != args.end() )
	    {
		m_clients_file = (*i);
	    }
	    else
	    {
		throw argument_exception( "-clients requires an argument" );
	    }
	}
/*	else if( (*i) == "-multipath" )
	{
	    if( ++i != args.end() )
	    {
		m_multipath = (*i);
	    }
	    else
	    {
		throw argument_exception( "-clients requires an argument" );
	    }
	}*/   
	else if( (*i) == "-man_routing" )
	{
	  m_man_routing = true;
	}
	else
	{

	    // unhandled argument, pass off to process factories
	    bool handled = false;
	    for( vector<WorkloadNodeProcessFactory*>::const_iterator j = m_npfs.begin();
		 j != m_npfs.end(); ++j )
	    {
		if( (*j)->handle_argument( i, args.end() ) )
		{
		    handled = true;
		}
	    }

	    if( !handled )
	    {
		throw argument_exception( (*i).c_str() );
	    }
	}

    }
    /*
     * check required arguments
     */
    if( !m_sim_length )
    {
	throw argument_exception( "'-l' is a required parameter" );
    }
    if( m_wtu_sec > 1.0 )
    {
	// the calculation of ts_per_min will screw up if 60 / wtu does not
	// result in an integer, so at this point its just easiest
	// to force it to be <= 1.0
	throw argument_exception( "'-wtu' cannot be greater than 1.0" );
    }
    if( m_files_file.size() == 0 )
    {
	throw argument_exception( "'-files' is a required parameter" );
    }
    for( vector<WorkloadNodeProcessFactory*>::const_iterator j = m_npfs.begin();
	 j != m_npfs.end(); ++j )
    {
	if( ! (*j)->satisfied() )
	{
	    throw argument_exception( (*j)->event_name() + string( " missing required argument(s)" ) );
	}
    }
    
    //standard param values
}

void WorkloadGenerator :: print_usage( const char* e )
{
    if( e != NULL )
    {
	cerr << "ERROR: " << e << endl;
    }
    cerr << "usage: [options...]\n\
required parameters:\n\
\t -l <min>		length of the simulation in minutes\n\
\t -prefix <file>=	the file name of the prefix file\n\
\t -files <file>=	the file name of the list of availables files\n";




    /*
     * print out the settings for the node process factories
     */
    for( vector<WorkloadNodeProcessFactory*>::const_iterator j = m_npfs.begin();
	 j != m_npfs.end(); ++j )
    {
	(*j)->print_required_usage( m_out );
    }

    cerr << 
	"\noptional parameters:\n\
\t -wtu <sec>		the workload time unit (s) (defaults to 1.0)\n\
\t -seed <num>		random seed (defaults to current time)\n\
" << endl;
    /*
     * print out the settings for the node process factories
     */
//     for( vector<WorkloadNodeProcessFactory*>::const_iterator j = m_npfs.begin();
// 	 j != m_npfs.end(); ++j )
//     {
// 	(*j)->print_optional_usage( m_out );
//     }

}

