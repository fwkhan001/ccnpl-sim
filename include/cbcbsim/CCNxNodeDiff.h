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


#include "cbcbsim/CCNxNode.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace cbcbsim;
using namespace cbnsim;
using namespace ssim;
using namespace std;

#ifndef CBCBSIM_CCNXNODEDIFF_H
#define CBCBSIM_CCNXNODEDIFF_H


namespace cbcbsim
{
  /** @brief Node class for CCNx Transport protocol with differentiated caching.
   */



  class CCNxNodeDiff : public cbcbsim::CCNxNode
  {

	private:
	unsigned int m_num_classes;
	unsigned int m_diff_mode; //0 static; 1 dynamic weighted; 2 dynamic strict priority
	bool m_ttl_active; //0 no TTL; 1 TTL; 
	std::map<std::string, double> m_cache_partitioning;
	std::map<std::string, double> m_cache_weights;
	std::map<std::string, double> m_cache_TTL;
	std::map<std::string, double> m_cache_class_size;
	std::map<std::string, int> m_cache_replacement; // 0 LRU
	std::multimap<double, std::string> m_cache_priority;
	double tot_weights;

	std::multimap<ssim::Time, DATA>  m_cache;
	std::map<std::string,  std::multimap<double, std::multimap<ssim::Time, DATA>::iterator >::iterator >  m_cache_search;
	std::map<std::string,  std::multimap<double, std::multimap<ssim::Time, DATA>::iterator > > m_cache_class;


    public:




    CCNxNodeDiff(unsigned int id, const ProtocolOptions& options, unsigned int cache_s, char cache_repl):CCNxNode(id, options,cache_s,cache_repl)
    {
    	m_num_classes=0;
    	tot_weights=0;
    	m_diff_mode=0;
	m_ttl_active=0;
    	std::vector<string> line_tok;
    	string line;
	std::ifstream file(m_options.cache_manag_file.c_str(), std::ifstream::in);
	//FILE FIELDS
	//-1 Class weight -2 class TTL (negative value means that the chunk will always be valid) -3 Class Name -4 Class replacement policy (0 LRU) -5 Cache partitioning (0 static 1 WMA 2 priority) -6 Node id
	if ( file.is_open() )
	{
		getline(file,line);
		line_tok=m_options.tokenize(line," "); 
	        while( !file.eof() )
		{
		 	  if (m_id==(unsigned int)atoi(line_tok[5].c_str()))
			  {
		    		  m_num_classes++;
		    		  m_cache_weights.insert(make_pair(line_tok[2],atof(line_tok[0].c_str())));
		    		  tot_weights+=atof(line_tok[0].c_str());
		    		  m_cache_priority.insert(make_pair(atof(line_tok[0].c_str()),line_tok[2]));
		    		  m_cache_TTL.insert(make_pair(line_tok[2],atof(line_tok[1].c_str())));
					if (atof(line_tok[1].c_str())>=0)
						m_ttl_active=1;
		    		  m_cache_class_size.insert(make_pair(line_tok[2],0.0));
		    		  std::multimap<double, multimap<ssim::Time, DATA> :: iterator > app;
		    		  m_cache_class.insert(make_pair(line_tok[2], app));
		    		  m_cache_replacement.insert(make_pair(line_tok[2],atoi(line_tok[3].c_str())));
		    		  m_diff_mode=atoi(line_tok[4].c_str());
		    	  }
		    	  getline(file,line);
		    	  line_tok=m_options.tokenize(line," ");
		}
	}
        else
	    	printf("ERROR\n");
	file.close();
	for (  std::map<std::string,double>::iterator it=m_cache_weights.begin() ; it != m_cache_weights.end(); it++ )
	{
	       
	       double app=(*it).second/tot_weights;
	       m_cache_partitioning.insert(make_pair((*it).first,app));

	}
	
    }
    void add_in_cache( const DATA& data );
    string find_class_to_remove_WMA( string data_class, double data_size );
    string find_class_to_remove_strict_priority(string data_class);
    void remove_data_LRU(string data_class_to_remove);
    void add_data_LRU (string data_class, const DATA& data );
    const DATA* search_in_cache( const INTEREST& interest );
    void remove_obsolete();
  };
};

#endif
