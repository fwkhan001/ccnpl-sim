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


#ifndef CBCBSIM_OUTPUTQUEUE_H
#define CBCBSIM_OUTPUTQUEUE_H


#include <cbcbsim/CCNxPackets.h>

#define ALPHA 0.9

namespace cbcbsim
{
  
  class CBCBNode;
  class TimeStats 
  {
    protected:
	unsigned int	win_counter;
	double		m_time_window_max;
	double		alpha;
	double		m_value;
	double		average;
    public:
	unsigned int	samples;
	double		rate;
	double		arrival_time;
	double		time_prev;

	TimeStats():win_counter(0),m_time_window_max(0.01),alpha(0.99),m_value(0.0),average(0.0),samples(0),
	rate(0.0),arrival_time(0.0),time_prev(0.0) {}

	bool time_counter(double t_now, double value)
	{
	  unsigned int new_measure = (unsigned int)floor((t_now - arrival_time)/m_time_window_max);
	  if ( win_counter == new_measure )
	  {
	    samples++;
	    m_value+=value;
	    return false;
	  }
	  ewma((double)samples/m_time_window_max, rate);
	  ewma((samples) ? m_value/(double)samples: 0, average);
	  samples = 0;
	  m_value = 0.0;
	  win_counter++;
	  time_counter(t_now, value);
	  return true;
	}
	

	
	double get_average() { return average; }
	
	double get_rate() { return rate; }
	
	void ewma(double smp, ssim::Time &var) 
	{ var = alpha * var + (1-alpha) * smp;  }
 
  };
  
  class Queue
  {
  protected: 
    unsigned int m_nid;
    unsigned int m_remote_nid;
    unsigned int m_ifid;
    unsigned int m_size;
    double m_link_rate;
    ssim::Time m_link_delay;
    ssim::Time m_sim_time_unit;
    CBCBNode *m_node;
    
    double	m_avg_length;

    TimeStats	m_loss_rate;
  public:
    /** @brief queue lenght
     */
    unsigned int m_length;
    
    Queue(CBCBNode *node,unsigned int nid, unsigned int remote_nid, unsigned ifid, 
	  unsigned int size, double link_rate, ssim::Time link_delay, 
	  ssim::Time sim_time_unit) : m_nid(nid), m_remote_nid(remote_nid),
	  m_ifid(ifid), m_size(size), m_link_rate(link_rate), m_link_delay(link_delay), 
	  m_sim_time_unit(sim_time_unit), m_node(node), m_avg_length(0.0), m_length(0){};
    
    /** @brief enqueue the packet 
     */ 
    virtual int enqueue(CCNxPacket *pkt)=0;
    
    /** @brief dequeue the packet 
     */
    virtual int dequeue()=0;

    /** @brief transmit the packet to the neighbor node 
     */
    virtual void transmit()=0;
    
    /** @brief when the queue si full, drop a packet
     */
    virtual int drop(CCNxPacket *pkt)=0;
    
    /** @brief queue length
     */
    unsigned int length(){ return m_length; }
    
    /** @brief queue max size
     */
    unsigned int size(){ return m_size; }
    
    /** @brief link rate
     */
    double rate(){ return m_link_rate; }
    
    /** @brief link delay
     */
    ssim::Time delay(){ return m_link_delay; }
    
    /** @brief node id 
     */
    unsigned int nid(){ return m_nid; }
    
    CBCBNode *node() { return m_node;}

    /** @brief interface id this queue is associated to
     */
    unsigned int ifid(){ return m_ifid; }
    
    /** @brief sim time, used to inizialize derived classes
     */
     ssim::Time sim_time_unit() {return m_sim_time_unit;}
     
     void ewma(int sample) {m_avg_length=(1-ALPHA)*sample+ALPHA*m_avg_length; }
     
     double avg_length(){return m_avg_length;}
    
  };
  
  class FIFO : public Queue
  {
    std::list<CCNxPacket*>	m_packet_list;
  public:
    FIFO(CBCBNode *node, unsigned int nid, unsigned int remote_nid, 
	 unsigned ifid, unsigned int size, double link_rate, 
	 ssim::Time link_delay, ssim::Time sim_time_unit) :
	 Queue(node, nid,remote_nid, ifid, size, link_rate, link_delay, sim_time_unit){};
	  
    /** @brief enqueue the packet, 
     *  apply the transport delay + the transport delay of the packet that are in front of this packet
     */
    int enqueue(CCNxPacket *pkt);
          
    /** @brief serve the next packet in the queue
     */
    int dequeue();
    
    /** @brief physcally transfer the packet to the next node, applying the propagation delay
     */
    void transmit();
    
    /** @brief when the queue si full, drop a packet with a free of the memory
     */
    int drop(CCNxPacket *pkt);
  };

  
  class VirtualQueue 
  {
    unsigned int						m_counter;
    unsigned int						m_deficit_counter;
    unsigned int						m_length;
    std::list<CCNxPacket*>					m_packet_list;
    /** @brief maximum value that the counter can assume
     */
    unsigned int						m_b;
    
  public:
    VirtualQueue() : m_counter(0), m_deficit_counter(0), 
    m_length(0), m_b(80000) {m_counter=m_b;};
    
    void enqueue(CCNxPacket* pkt);
    
    CCNxPacket* head();
    
    void dequeue();
    
    unsigned int drop();
    
    unsigned int counter() { return m_counter; } 

    unsigned int deficit_counter(){ return m_deficit_counter; }
    
    unsigned int length() {return m_length; }

    void inc_dc(unsigned int inc) { m_deficit_counter+=inc; }
    
    void dec_dc(unsigned int dec) { m_deficit_counter-=dec; }
    
    void inc_count(unsigned int inc) { m_counter = min(m_b,m_counter+inc); }
    
    bool dec_count(unsigned int dec);

  };
  
  /** @brief Hash table for the virtual queues key: content_name+"port:"+port_id of the packet 
   */
  typedef boost::unordered_map <std::string, VirtualQueue >  virtual_queues;
  
  /** @brief Fair queing implementation that follows the one 
   *  @brief described  by M. Shreedhar and G. Varghese: 
   *  "Efficient fair queuing using deficit round-robin." 
   *   in IEEE/ACM Transactions on Networking, 1996
   */
  class FQ : public Queue
  { 
  public: 
    
    virtual_queues::iterator 					vqit;
    unsigned int 						m_quantum;
    virtual_queues						m_virtual_queues;
    virtual_queues::iterator					m_actual_queue;
   
  
    FQ(CBCBNode *node, unsigned int nid, unsigned int remote_nid, unsigned ifid, 
       unsigned int size, unsigned int link_rate, unsigned int link_delay, 
       ssim::Time sim_time_unit) :  Queue(node, nid,remote_nid, ifid, size, 
	link_rate, link_delay, sim_time_unit), m_quantum(80000){m_virtual_queues.rehash(1000);};
    
    /** @brief enqueue the packet
     *  enqueue it in the right queue. Queues are stored in a hash table with key content_name+port_id of the pkt
     */   
    int enqueue(CCNxPacket *pkt);
    
    /** @brief serve the packet
     */
    int dequeue();
    
    /** @brief physcally transfer the packet to the next node, applying the propagation delay
     */
    void transmit();
    
    /** @brief drop a packet from the longest queue
     */
    int drop(CCNxPacket *pkt){return drop();}
    
    int drop();
    
    void inc_counts();
    
    bool dec_count(INTERESTPacket interest);
    
    /** @brief moves the actual_queue iterator to the next queue,
     * - if the queue pointed by m_actual_queue is empty, erase the virual queue from the list of active flows 
     *   and then moves the iterator
     * - moves to the first active queue if reached the end of the active queue list. 
     * - increments the deficit counter of the queue that is pointed after the move
     */
    void next_queue();
    
    /** @brief insert a new VirtualQueue with the specified name in the active flow list and returns an iterator 
     *  that points to the newly inserted VirtualQueue
     */
    virtual_queues::iterator insert_queue(cname content_name);
  };


};

#endif
