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
#include "cbcbsim/RTTstats.h"

using namespace cbcbsim;

RTTstats::RTTstats(unsigned int estimation_length, double timer_estim, double threshold, double alpha):
	estimation_length(estimation_length),timer_estim(timer_estim),threshold(threshold),alpha_rtt(alpha),
	RTT_avg(0.0),RTT_min(0.0),RTT_max(0.0),RTT_actual(0.0){};

void RTTstats :: samples_update(double chunk_RTT){
  
  multiset<double>::iterator it_ordered_set;
  /** @brief keep an that is directly associated with the list of the RTT samples
   *  ordered list for fast max min computation
   */
  
  RTT_actual=chunk_RTT;
  
  it_ordered_set = ordered_samples.insert(chunk_RTT);
  sample new_sample ={chunk_RTT,it_ordered_set};
  samples.push_back(new_sample);
  if(samples.size() > estimation_length)
  {
     ordered_samples.erase(samples.front().it_ordered_set);
     samples.pop_front();
  }
  
  it_ordered_set = ordered_samples.begin();
  RTT_min=(*it_ordered_set);
  
  it_ordered_set = ordered_samples.end();
  it_ordered_set--;
  RTT_max=(*it_ordered_set);

  timer_estim=(RTT_min+(RTT_max-RTT_min)*threshold);
  ewma(chunk_RTT);
  
}