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

#include <cbcbsim/OutputQueue.h>
#include <cbnsim/logging.h>
#include <cbnsim/DeQueuePacket.h>
  #include <cbcbsim/CBCBNode.h>
#include <iostream>

using namespace cbcbsim;
using namespace cbnsim;

#define NDEBUG(m) CBNDEBUG( "CBCBNode[" << pid() << "@" << Sim::clock() << "]: " << m )


int FIFO :: enqueue(CCNxPacket *pkt)
{
  if ( length()+pkt->size() > size() )
  {
    cout << Sim::clock() << " dropFIFO " 
      << pkt->content_name() 
      << " node = " << nid() 
      << " interface= " << ifid() << endl;
/*    CBNDEBUG( Sim::clock() << " dropFIFO " << pkt->content_name() << endl);*/
   // m_loss_rate.time_counter(Sim::clock()*m_node->m_options.sim_time_unit, 1);

    return drop(pkt);
  }
  //m_loss_rate.time_counter(Sim::clock()*m_node->m_options.sim_time_unit, 0);
  m_packet_list.push_back(pkt);
  m_length += pkt->size();
    
//   cout << "FIFO\t" << m_packet_list.front()->content_name() 
//   << "\tnode="<< nid() 
//   << "\tface=" << ifid() 
//   << "\t"  << Sim::clock() 
//   << "\t" << length() 
//   << "\t" << rate() 
//   << "\t" << avg_length() << endl;
  if (m_length == pkt->size())
      return dequeue();
  return 1;
}
        
void FIFO :: transmit()
{
  m_length -= m_packet_list.front()->size();
  ewma(length());
//    cout << "FIFO\t" << m_packet_list.front()->content_name() 
//    << "\tnode="<< nid() 
//    << "\tface=" << ifid() 
//    << "\tto " << m_node->i2p(ifid())
//    << "\t"  << Sim::clock()
//    << "\t" << length()
//    << "\t" << rate() 
//    << "\t" << avg_length()<< endl;

 
  Sim::signal_event( m_remote_nid, m_packet_list.front(), m_link_delay);
  m_packet_list.pop_front();
}

int FIFO :: dequeue()
{
  CCNxPacket *pkt = m_packet_list.front();
  if (!m_packet_list.size())
    return 0;
  
  Sim::signal_event( nid(), new cbnsim::DeQueuePacket((void *)this), 
			    (double)pkt->size()/rate()/m_sim_time_unit);
  return 1;
}

int FIFO :: drop(CCNxPacket *pkt)
{
  delete(pkt);
  return 0;
}

int FQ :: enqueue(CCNxPacket *pkt)
{
  stringstream port;  port << pkt->port();
  string  cnp = pkt->content_name() + "/port:" + port.str();
  
  vqit = m_virtual_queues.find(cnp);
  if ( vqit ==  m_virtual_queues.end() )
    vqit = insert_queue(cnp);
  if ( vqit !=  m_virtual_queues.end() ) 
  {
    vqit->second.enqueue(pkt);
    m_length+=pkt->size();    
    if (m_length == pkt->size())
    {
      m_actual_queue = m_virtual_queues.begin();
      m_actual_queue->second.inc_dc(m_quantum);
      dequeue();
    }
  }
  while ((length() > size()))
    drop();  
//   cout << "FQ\t" << pkt->content_name() 
//   << "\tnode="<< nid() 
//   << "\tface=" << ifid() 
//   << "\tto " << m_node->i2p(ifid())
//   << "\t"  << Sim::clock() 
//   << "\t" << length() 
//   << "\t" << rate() << endl;
  return 0;
}

void FQ :: transmit()
{
  CCNxPacket *pkt = m_actual_queue->second.head();
  m_length -= pkt->size();
  Sim::signal_event( m_remote_nid, pkt, m_link_delay);
  m_actual_queue->second.dequeue();
}

virtual_queues::iterator FQ :: insert_queue(cname content_name)
{
  VirtualQueue new_virtual_queue = VirtualQueue();
  std::pair<virtual_queues::iterator,bool > result;
  result = m_virtual_queues.insert(make_pair(content_name, new_virtual_queue));    
  if (!result.second)
    return m_virtual_queues.end();
  return result.first;
}

int FQ :: dequeue()
{
  if (m_virtual_queues.empty())
    return 0;

  CCNxPacket *pkt = m_actual_queue->second.head();
  
  /** if actual queue is allowed to transmit 
   *  (it has deficit counter lower than the first packe in queue)
   *  schedule a departure from this queue
   */
   if (pkt && (pkt->size() <= m_actual_queue->second.deficit_counter()) ) 
    {
      m_actual_queue->second.dec_dc(pkt->size());
      Sim::signal_event( nid(), new cbnsim::DeQueuePacket((void *)this),
				(double)pkt->size()/rate()/m_sim_time_unit);				
      return 1;
    }
  next_queue();
  dequeue();
  return 0;
}

void FQ :: next_queue()
{
  if(m_actual_queue->second.length() == 0)
    m_virtual_queues.erase(m_actual_queue++); 
  else
    m_actual_queue++;
  if  (m_actual_queue == m_virtual_queues.end())
  {  
    m_actual_queue = m_virtual_queues.begin();
    inc_counts();// for the Jim algorithm
  }
  
  
  if(!m_virtual_queues.empty())
    m_actual_queue->second.inc_dc(m_quantum);
}

void FQ :: inc_counts()
{
  for (vqit=m_virtual_queues.begin();vqit!=m_virtual_queues.end();vqit++)
  {
    vqit->second.inc_count(m_quantum);
  }
}

bool FQ :: dec_count(INTERESTPacket interest)
{
  
  stringstream port;  port << interest.port();
  string  content_name = interest.content_name() + "/port:" + port.str();
  vqit = m_virtual_queues.find(content_name);
  if ( vqit !=  m_virtual_queues.end() )
  {
    if (vqit->second.dec_count(interest.data_size()))
      return true;
    else
      return false;
  }
 
       
  return true;
}

int FQ :: drop()
{
  virtual_queues::iterator max_flow;  
  unsigned int max_length=0;
  if (m_virtual_queues.empty())
  {
    cout << "Should not be here! drop called and " << endl;
    return 0;
  }
  for (vqit = m_virtual_queues.begin(); vqit != m_virtual_queues.end(); vqit++)
  {
    if ( max_length < max_flow->second.length() )
    {
      max_flow = vqit;
      max_length = max_flow->second.length();
    }
  }  
  m_length -=  max_flow->second.drop();;
  return 1;
}

void VirtualQueue::enqueue(CCNxPacket* pkt) 
{
    m_packet_list.push_back(pkt);
    m_length+=pkt->size();
}
    
CCNxPacket* VirtualQueue::head()
{
    if (m_length == 0)
	return NULL;
    return m_packet_list.front();
}
    
void VirtualQueue::dequeue()
{
   if (m_length == 0) 
      return;
   m_length-=m_packet_list.front()->size(); 
   m_packet_list.pop_front();
}

unsigned int VirtualQueue::drop()
{
  if (m_length == 0)
    return 0;
  
  CCNxPacket* pkt=m_packet_list.back();
  unsigned int pkt_size = pkt->size();
  m_packet_list.pop_back();
  delete(pkt);
  return pkt_size;
}

bool VirtualQueue::dec_count(unsigned int dec)
{
  if(m_counter >= dec) 
  {
    m_counter-=dec;
    return true;
  }
  return false;
}



