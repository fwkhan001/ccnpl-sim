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
#ifndef CBCBSIM_SERVEDPREDICATE_H
#define CBCBSIM_SERVEDPREDICATE_H

#include <cbnsim/types.h>
#include <siena/ssimp_types.h>
#include <string>
#include <stdlib.h>

namespace cbcbsim
{
  class ServedContent 
  {
    cbnsim::Predicate                   m_predicate;
    std::string				m_address;
    unsigned int			m_size; 
    unsigned int			m_chunk_size; 
    unsigned int			m_receiver_win;
    unsigned int			m_fec_perc;
    unsigned int			m_popularity_class;
    
  public:
    ServedContent( cbnsim::Predicate predicate, unsigned int size, unsigned int chunk_size, unsigned int receiver_win, unsigned int fec_perc , unsigned int popularity_class)
      : m_predicate( predicate ),m_size ( size ), m_chunk_size( chunk_size  ), m_receiver_win( receiver_win ), m_fec_perc( fec_perc ), m_popularity_class( popularity_class )
    {
      //TRANSFORM PREDICATE IN CCNX URI
      std::set< ssimp::TFilter<true> > filt;
      std::set< ssimp::TFilter<true> >:: iterator it;
      std::string address;
      filt = m_predicate.getFilters();
      for ( it = filt.begin(); it != filt.end(); it++)
      {
	std::set<ssimp::Constraint> constr;
	std::set<ssimp::Constraint>::iterator i;
	constr = (*it).getConstraints();
	for ( i = constr.begin(); i != constr.end(); i++)
	{
	  address += "/";
	  address +=  std::string((*i).string_value().begin,(*i).string_value().length());
      
	}
      }
      m_address=address;
    }



    unsigned int file_size() const
    {return m_size; } 
    
    unsigned int chunk_size() const
    {return m_chunk_size; } 
   
    cbnsim::Predicate predicate() const
    { return m_predicate; }
    
    std::string address()
    { return m_address; }
    
    unsigned int receiver_win() const
    {return m_receiver_win; }
    
    unsigned int fec_perc() const
    {return m_fec_perc; }
    
    unsigned int popularity_class() const
    {return m_popularity_class; }

  };
};
#endif
