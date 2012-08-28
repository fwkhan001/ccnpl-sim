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

#include <cbcbsim/LoadBalancing.h>
#include <cbcbsim/CBCBNode.h>

void LoadBalancing :: add(cname pn,unsigned int ifid)
{
  prefix_table::iterator it;
      
  it = m_ptable.find(pn);
  if (it == m_ptable.end())
  {
    prefix_stats ps;
    m_ptable[pn] = ps;
  }
  unsigned int i=0;
  while(i < m_ptable[pn].stats.size()) 
  {
    if (m_ptable[pn].stats[i].ifid == ifid)
      return;
        i++;
  }
  /** init. pending interests (and its avg) from 1 so that when there are no interest pending, 
   *  choice is uniform between avail. ifid
   */
  stats_entry se = {ifid,0,0,1};
  se.split_stats.init(Sim::clock()*m_node->m_options.sim_time_unit);
  m_ptable[pn].stats.push_back(se);
  m_ptable[pn].weight_sum++;

     
     //here you can had static optmal wieghts on each node
     //topo 4 paths
//     switch (m_node->pid()){
//       case 0:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//       case 1:
// 	cout << (*m_node).pid() <<" i=" << i << " ifid=" << m_ptable[pn].stats[i].ifid << "to node " << (*m_node).i2p(m_ptable[pn].stats[i].ifid) << endl;
// 	if (i==0)
// 	  m_ptable[pn].stats[0].weight=0.33;
// 	else
// 	  m_ptable[pn].stats[1].weight=0.67;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//       case 2:
// 	cout << (*m_node).pid() <<" i=" << i << " ifid=" << m_ptable[pn].stats[i].ifid << "to node " << (*m_node).i2p(m_ptable[pn].stats[i].ifid) << endl;
// 	if (i==0)
// 	  m_ptable[pn].stats[0].weight=0.25;
// 	else
// 	  m_ptable[pn].stats[1].weight=0.75;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//       case 3:
// 	cout << (*m_node).pid() <<" i=" << i << " ifid=" << m_ptable[pn].stats[i].ifid << "to node " << (*m_node).i2p(m_ptable[pn].stats[i].ifid) << endl;
// 	if (i==0)
// 	  m_ptable[pn].stats[0].weight=0.125;
// 	else
// 	  m_ptable[pn].stats[1].weight=0.875;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//       case 4:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//       case 5:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//        case 6:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//        case 7:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//        case 8:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//        case 9:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
// 	case 10:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
// 	case 11:
// 	m_ptable[pn].stats[i].weight=1;
// 	m_ptable[pn].weight_sum=1;
// 	break;
//     }
}

void LoadBalancing :: pending(cname pn,unsigned int ifid, char op)
{
  unsigned int i=0;
  while(i < m_ptable[pn].stats.size()) 
  {
    if (m_ptable[pn].stats[i].ifid == ifid)
    {
      if (op == '+')
	m_ptable[pn].stats[i].pending++;
      else if (op == '-')
	if (m_ptable[pn].stats[i].pending>0){
	  m_ptable[pn].stats[i].pending--;
	}
	
	//if you want to use static weight comment this weight update
	m_ptable[pn].weight_sum-=m_ptable[pn].stats[i].weight;
	ewma(m_ptable[pn].stats[i].pending,m_ptable[pn].stats[i].avg_pending,m_alpha);
	if (m_ptable[pn].stats[i].avg_pending==0.0)
	  m_ptable[pn].stats[i].avg_pending=0.1;
	m_ptable[pn].stats[i].weight=1/(m_ptable[pn].stats[i].avg_pending);     
	m_ptable[pn].weight_sum+=m_ptable[pn].stats[i].weight;
	
      return;
    } 
    i++;
  }
}
      
unsigned int LoadBalancing :: fwd_interest(cname pname)
{
  prefix_table::iterator i;
  unsigned int fwd_ifid=0;
  i = m_ptable.find(pname);
  if (i==m_ptable.end())
    return fwd_ifid; 
  if (m_node->m_options.fwd == ProtocolOptions::FWD_RND){
      fwd_ifid = rnd(i);
  }
  if (m_node->m_options.fwd == ProtocolOptions::FWD_BALANCE){
      fwd_ifid = weighted_rnd(i);
  }
  
  //update stats for the use interface
  int j=0;
  bool found = false;
  while((j < m_ptable[pname].stats.size())&&(!found)){
      if (m_ptable[pname].stats[j].ifid == fwd_ifid){
	  i->second.stats[j].split_stats.time_counter(Sim::clock()*m_node->m_options.sim_time_unit,true);
	  found=true;
      }
      j++;
  }
  
  pending(pname,fwd_ifid,'+');
 
  return fwd_ifid;
}

unsigned int LoadBalancing :: weighted_rnd(prefix_table::iterator i)
{
  double r = ((double)random() / (double)(RAND_MAX));   
  double cum = i->second.stats[0].weight/i->second.weight_sum;
  unsigned int j=0;
  while((cum<r)&&(j < i->second.stats.size()))
  {
    j++;
    cum+=(i->second.stats[j].weight/i->second.weight_sum);
  }
  return i->second.stats[j].ifid;
}

unsigned int LoadBalancing :: rnd(prefix_table::iterator i)
{
  /**Choose rnd interface
  */
  unsigned int r = rand()% i->second.stats.size();
  return i->second.stats[r].ifid;  
}

bool SplitStats:: time_counter(Time t_now, bool sum)
{
  unsigned int new_measure = (unsigned int)floor((t_now - start_time)/m_time_window_max);
  if ( win_counter == new_measure )
  {
     samples+=sum;
     return false;
  }
  ewma((double)samples/m_time_window_max, rate);
  samples = 0;
  win_counter++;
  time_counter(t_now, sum);
  return true;
}

void SplitStats::init(Time start)
{
  start_time=start;
  win_counter=0;
  samples=0;
  m_time_window_max=0.1;//10.0;
  alpha=0.99;
  rate=0.0;
  start_time=0.0;
  time_prev=0.0;
}