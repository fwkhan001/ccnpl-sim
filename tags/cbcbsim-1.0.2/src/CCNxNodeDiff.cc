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

#include <cbnsim/SetPredicateEvent.h>
#include <cbnsim/SetPredicateParser.h>
#include <cbnsim/WorkloadParser.h>
#include <cbnsim/types.h>
#include <siena/types.h>
#include <siena/ssimp_types.h>
#include "cbcbsim/INTEREST.h"
//#include "cbcbsim/CCNxNode.h"
#include <cbnsim/NodeDeletePacket.h>
#include <siena/ssim.h>
#include "cbcbsim/ReceiverSocket.h"
#include "cbcbsim/SERVEDPREDICATE.h"
#include "cbcbsim/CCNxNodeDiff.h"
#include "cbcbsim/DATA.h"

using namespace cbcbsim;
using namespace cbnsim;
using namespace ssim;
using namespace std;


#define NDEBUG(m) CBNDEBUG( "CCNxNodeDiff[" << pid() << "@" << Sim::clock() << "]: " << m )



void CCNxNodeDiff::add_in_cache( const DATA& data )
    {
    //We suppose the first filter of the predicate denotes the class
	//Find the class to which data belong to
	//TODO modify and use instead the message in order to identify the class
//     std::set< TFilter<true> > filt;
//     std::set< TFilter<true> >:: iterator it;
//     filt = data.predicate().getFilters();
//     it=filt.begin();
    string data_class;
    const map<string,Message::Value>& attrs = data.msg().getAttributes();
    map<string,Message::Value>::const_iterator k;
    k = attrs.begin();
    data_class = (*k).second.s_value;
//     std::set<Constraint> constr;
//     std::set<Constraint>::iterator i;
//     constr = (*it).getConstraints();
//     i = constr.begin();
//     data_class=string((*i).string_value().begin,(*i).string_value().length());
    if (m_ttl_active==1)
    	remove_obsolete();

    INTEREST* interest;
    interest =  new INTEREST( m_id, m_id, data.receiver(), data.chunk_id(), data.address(), -1 , 1,data.msg(), data.size(), data.port(),0, data.chunk_number());
    const DATA* data_searched=search_in_cache(*interest);
    	if (data_searched==NULL){


    string data_class_to_remove;
           	if (m_diff_mode==0)
        	{        		//Static memory allocation

        		data_class_to_remove=data_class;

        		while( (*(m_cache_class_size.find(data_class))).second+double (data.size()/1000) > (*(m_cache_partitioning.find(data_class))).second*m_cache_limit)
        		 {
        			switch((*m_cache_replacement.find(data_class_to_remove)).second){
        			case 0: remove_data_LRU(data_class_to_remove); break;//LRU replacement
        			}
        		 }


        	}
        	else
        	{

        		while( ( m_cache_size+(data.size()/1000) ) > (m_cache_limit) )
        		     {
        			string data_class_to_remove;
        			switch(m_diff_mode){
        			case 1: data_class_to_remove=find_class_to_remove_WMA(data_class,data.size());break;
        			case 2: data_class_to_remove=find_class_to_remove_strict_priority(data_class);break;
        			}

        			if (data_class_to_remove.compare("")==0)
        			{
        				delete(interest);
        				delete(data_searched);
        				return;
        			}

        			switch((*m_cache_replacement.find(data_class_to_remove)).second){
        			        			case 0: remove_data_LRU(data_class_to_remove);break; //LRU replacement
        			        			}
        			   }


        	}

					switch((*m_cache_replacement.find(data_class)).second){
							case 0:add_data_LRU(data_class, data);break;
						}
    	}
  		delete(interest);
  		delete(data_searched);
    }

string CCNxNodeDiff::find_class_to_remove_WMA( string data_class, double data_size )
{

std::vector<string> class_to_remove;
double max_rat=0.0;
double aux,aux2;

for (  std::map<std::string,double>::iterator it=m_cache_class_size.begin() ; it != m_cache_class_size.end(); it++ )
		    {
				if (data_class.compare((*it).first)==0)
					{aux=(*it).second+(data_size/1000);}
				else
				{
					aux=(*it).second;
				}
				aux2=(aux/(*m_cache_weights.find((*it).first)).second);

				if ( aux2 > max_rat)
				{
				max_rat=aux2;
				class_to_remove.clear();
				class_to_remove.push_back((*it).first);
				}
				else if (aux2==max_rat)
				{
					class_to_remove.push_back((*it).first);
				}
		   }


return class_to_remove[rand() % class_to_remove.size()];
}

string CCNxNodeDiff::find_class_to_remove_strict_priority(string data_class)
{
//Attention right now double entries are not handled
string class_to_remove="";

std::map<std::string,double>::iterator it;

for ( std::map<double,std::string>::iterator it_pri=m_cache_priority.begin() ; it_pri != m_cache_priority.end(); it_pri++ )
		    {
				it=m_cache_class_size.find((*it_pri).second);
				if ((*it).second>0){
					class_to_remove=(*it).first;
					break;
				}
				if (data_class.compare((*it).first)==0)
					break;

		   }


return class_to_remove;
}


void CCNxNodeDiff :: remove_data_LRU(string data_class_to_remove)
{
	std::multimap<ssim::Time, DATA>::iterator   it;
	std::multimap<double, multimap<ssim::Time, DATA>::iterator >::iterator it_class_sort;
	std::map<std::string,  std::multimap<double, multimap<ssim::Time, DATA>::iterator > >::iterator   it_class;

	it_class=m_cache_class.find(data_class_to_remove);
	it_class_sort=(*it_class).second.begin();
	//unsigned int data_size=(*(*it_class_sort).second).second.size();
	double data_size=(*(*it_class_sort).second).second.size();
	m_cache_size-=(double)(data_size/1000);
	(*(m_cache_class_size.find(data_class_to_remove))).second-=double (data_size/1000);
	it=(*it_class_sort).second;
	string search_str ;
	search_str.append((*it).second.address());
	search_str.append("/");
	char chunk[100];
	sprintf(chunk,"%d",(*it).second.chunk_id());
	search_str.append(chunk);
	(*it_class).second.erase(it_class_sort);
	m_cache_search.erase(search_str);
	//printf("START REMOVED n_id %d mine %s %d original %s %d \n",m_id,(*it).second.address().c_str(), (*it).second.chunk_id(),(*m_cache_LRU.find((*it_class_sort).first)).second.address().c_str(),(*m_cache_LRU.find((*it_class_sort).first)).second.chunk_id());fflush(stdout);
	m_cache.erase(it);
}

void CCNxNodeDiff :: add_data_LRU (string data_class, const DATA& data )
{
	std::multimap<ssim::Time, DATA>::iterator   it;
	std::multimap<double, multimap<ssim::Time, DATA>::iterator >::iterator it_class_sort;
	std::map<std::string,  std::multimap<double, multimap<ssim::Time, DATA>::iterator > >::iterator   it_class;

	it_class=m_cache_class.find(data_class);
	m_cache_size+=double (data.size()/1000);
	(*(m_cache_class_size.find(data_class))).second+=double (data.size()/1000);
	if ((*m_cache_TTL.find(data_class)).second >=0)
		it = m_cache.insert( make_pair(Sim::clock()+(*m_cache_TTL.find(data_class)).second/m_options.sim_time_unit, data));
	else
		it = m_cache.insert( make_pair(Sim::clock()+m_options.sim_length, data));
	if (it == m_cache.end())
	cout << " Severe error!" << endl;
	string insert;
	insert.append(data.address());
	insert.append("/");
	char chunk2[100];
	sprintf(chunk2,"%d",data.chunk_id());
	insert.append(chunk2);
	it_class_sort=(*it_class).second.insert(make_pair((double) Sim::clock(),it));
	m_cache_search.insert(make_pair(insert,it_class_sort));
	//printf("ADD %d %f %f %s\n",m_id, Sim::clock(),Sim::clock()+(*m_cache_TTL.find(data_class)).second,insert.c_str());
}



const DATA* CCNxNodeDiff :: search_in_cache( const INTEREST& interest )
{
  const DATA*                                 cached_chunk=NULL;
  string search = interest.address();
  search.append("/");
  char chunk[100];
  sprintf(chunk,"%d",interest.chunk_id());
  search.append(chunk);
  //if ( m_options.cach == ProtocolOptions::CACHING_ON_LRU)
  //{

    std::multimap<ssim::Time, DATA>::iterator   it;
    std::multimap<ssim::Time, DATA>::iterator   it_new;
    std::map<std::string,  std::multimap<double, std::multimap<ssim::Time, DATA>::iterator >::iterator >::iterator   it_search;
    std::set< TFilter<true> > filt;
    std::set< TFilter<true> >:: iterator it_filt;
    std::multimap<double, multimap<ssim::Time, DATA>::iterator >::iterator it_class_sort;
    std::map<std::string,  std::multimap<double, multimap<ssim::Time, DATA>::iterator > >::iterator   it_class;

if (m_ttl_active==1)
    remove_obsolete();

    it_search=m_cache_search.find(search);

    if (it_search != m_cache_search.end())
    {


      it = (*(*it_search).second).second;



      cached_chunk = new DATA( m_id, m_id, interest.issuer(), (*it).second.chunk_id(), (*it).second.msg(), (*it).second.address(), (*it).second.size() , interest.interest_issue_time(),interest.port(), interest.chunk_number());


      //Differentiated caching modifications
      string data_class;
      const map<string,Message::Value>& attrs = (*it).second.msg().getAttributes();
      map<string,Message::Value>::const_iterator k;
      k = attrs.begin();
      data_class = (*k).second.s_value;
            it_class=m_cache_class.find(data_class);

            switch((*m_cache_replacement.find(data_class)).second){
        			case 0:{

									(*it_class).second.erase((*it_search).second);

									it_new = m_cache.insert( make_pair((*it).first, *cached_chunk ));
									m_cache.erase(it);
									it_class_sort=(*it_class).second.insert(make_pair(Sim::clock(),it_new));
									m_cache_search[search]=it_class_sort;
      //End Differentiated caching modifications
							}break;
						}
    }
  // }

    if (interest.interest_issue_time()<=0)
  	  return cached_chunk;

  if (cached_chunk == NULL){
      miss_rate_update(search,interest.popularity_class());
//if (m_miss_rate.find(search)!=m_miss_rate.end())
	//  m_miss_rate[search]+=1;
      //else
	//  m_miss_rate[search]=1;
  }
  else{
	hit_rate_update(search,interest.popularity_class());
      //if (m_hit_rate.find(search)!=m_hit_rate.end())
      //	  m_hit_rate[search]+=1;
      //else
	//m_hit_rate[search]=1;
  }
  return cached_chunk;
}



void CCNxNodeDiff::remove_obsolete()
{
	std::multimap<ssim::Time, DATA>::iterator   it;
	std::map<std::string,  std::multimap<double, std::multimap<ssim::Time, DATA>::iterator >::iterator >::iterator   it_search;
	std::set< TFilter<true> > filt;
	std::set< TFilter<true> >:: iterator it_filt;
	    std::multimap<double, multimap<ssim::Time, DATA>::iterator >::iterator it_class_sort;
	    std::map<std::string,  std::multimap<double, multimap<ssim::Time, DATA>::iterator > >::iterator   it_class;


it=m_cache.begin();


while(it != m_cache.end())
{
	 ;
	if ((*it).first>=Sim::clock())
		break;

	string search_str ;
	search_str.append((*it).second.address());
	search_str.append("/");
	char chunk[100];
	sprintf(chunk,"%d",(*it).second.chunk_id());
	search_str.append(chunk);

	it_search=m_cache_search.find(search_str);
	string data_class;
	const map<string,Message::Value>& attrs = (*it).second.msg().getAttributes();
	map<string,Message::Value>::const_iterator k;
	k = attrs.begin();
	data_class = (*k).second.s_value;// 	 std::set<Constraint> constr;

	 double data_size=(*(*(*it_search).second).second).second.size();
	 m_cache_size-=(double)(data_size/1000);
	 	(*(m_cache_class_size.find(data_class))).second-=double (data_size/1000);
     it_class=m_cache_class.find(data_class);
     (*it_class).second.erase((*it_search).second);
     m_cache_search.erase(it_search);
     m_cache.erase(it);
     it=m_cache.begin();
}

}


