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
#include <stdlib.h>
#include "cbcbsim/SocketManager.h"
#include <vector>


using namespace cbcbsim;
using namespace ssim;
using namespace std;



SocketManager::SocketManager(const ProtocolOptions& options, unsigned int id, const INFOPacket &info, 
			     unsigned int port, const set<siena::if_t>& matches) : 
			     Socket(options, id, info, port, matches)
{
  m_aborted = false;
  m_lower_rx_chunk = 0;
  /** @brief create one socket for each possible path using matches for ifid of the Receiver Sockets
   */      
   
  m_fc_type=m_options.fc_type; 

   
  for( set<siena::if_t>::const_iterator it = matches.begin();it != matches.end(); it++ ){

    if (m_fc_type==ProtocolOptions::fc_ICP){
      m_window_controllers.push_back(new ICP(m_id, m_content_name, m_port, m_options, (unsigned int)(*it), start_time()) );
    }else if (m_fc_type==ProtocolOptions::fc_RAQM){
      m_window_controllers.push_back(new RAQM(m_id, m_content_name, m_port, m_options, (unsigned int)(*it), start_time()));
    }else if (m_fc_type==ProtocolOptions::fc_RAQMsource){
       m_window_controllers.push_back(new RAQMsource(m_id, m_content_name, m_port, m_options, (unsigned int)(*it), start_time()) );
    }else if (m_fc_type==ProtocolOptions::fc_CBR){
      m_window_controllers.push_back(new CBR(m_id, m_content_name, m_port, m_options, (unsigned int)(*it), start_time(),
				    m_options.rbfcp_win,m_options.cbr_rate));
    }
  }
}


std::map<string,int> SocketManager::get_paths_stat(){
  std::map<string,int> stat;
  unsigned int i=0;
  bool found = false;
  if (m_fc_type!=ProtocolOptions::fc_RAQMsource){
   return stat;
  }
  while (i < m_window_controllers.size() )
  {
    if (m_window_controllers[i]->ifid() == 1){
      RAQMsource* raqm = (RAQMsource*) m_window_controllers[i];
      stat = raqm->get_paths_stat();
      found=true;
    }
    i++;
  }
  return stat;
}


bool SocketManager :: received_chunk( int received_chunk , unsigned int size, unsigned int ifid, string source)
{	
  bool complete = false;
  m_missing_chunks.erase(received_chunk); 
  if ( received_chunk == 0 ) //can be un-used
  {
    m_file_chunks[0].received = true;
    return false;
  }

  /** @brief select receiver socket to which send this received chunk using the ifid and remove 
    *  the chunk_id from the interest window
    */
  unsigned int i=0;
  bool found = false;
  while (i < m_window_controllers.size() )
  {
    if (m_window_controllers[i]->ifid() == ifid)
    {
      found = true;
      bool retransmitted = (m_file_chunks[received_chunk].chunk_request>1);
      m_window_controllers[i]->received_chunk(received_chunk,retransmitted,source);
      /** @brief if not a DUPLICATED DATA INCREMENT DATA SIZE and mark as received the chunk in the file bitmap
	*/
      if (m_file_chunks[received_chunk].received == false)
      {
	/** @brief keep track of the received chunk
	  */
	received++; 
	m_file_chunks[received_chunk].received = true;
	m_missing_chunks.erase(received_chunk); //maybe this is not needed (chunk is removed from missing 
						 //whenever it is assigned to a particular interface)
	
	/** @brief Update VRTT and delay measures
	  */
	
	double chunk_RTT = Sim::clock()-m_file_chunks[received_chunk].chunk_RTT;
	m_file_chunks[received_chunk].chunk_RTT=chunk_RTT;
	double chunk_retrieving_time = Sim::clock()-m_file_chunks[received_chunk].chunk_retrieving_time;
	m_file_chunks[received_chunk].chunk_retrieving_time = chunk_retrieving_time;
	VRTT_update(chunk_RTT);
	delay_update(chunk_retrieving_time);
	size_update(size);
      }
      break;
    }
    i++;
  }
  if (!found)
    cout << "window control for interface " << ifid<< " do not exist!!" << endl;
  return complete;
}

void SocketManager :: delete_receiver()
{
  for(unsigned int i=0;i<m_window_controllers.size();i++)
  {      
    if(m_window_controllers[i]!=NULL)
    {
      delete(m_window_controllers[i]);
      m_window_controllers[i]=NULL;
    }
  }
  m_window_controllers.clear();
}

void SocketManager :: receiver_TO(unsigned int ifid, unsigned int chunk_id)
{
  bool found=false;
  unsigned int i=0;
  while (i < m_window_controllers.size() )
  {
    /** @brief select the window controller to which send this receiver timer
     */
    if (m_window_controllers[i]->ifid() == ifid)
    {      
      found = true;
      m_window_controllers[i]->receiver_TO(chunk_id);
      if(!m_file_chunks[chunk_id].received)
	m_file_chunks[chunk_id].chunk_timeout++;
      
      /** @brief if reached the maximum number of interest re-expressions for the same chunk, abort the download
       *  and signal this to the CCNx node
       */
      if(m_file_chunks[chunk_id].chunk_request >= m_options.to_limit)
      {
	cout << Sim::clock() << " Aborting " << m_content_name << "/" << chunk_id  << " port " << m_port << endl;
	m_missing_chunks.clear();
	m_aborted=true;
      }
      break;
    }
    i++;
  }
  if (!found)
    cout << "window control for interface " << ifid << " do not exist!!" << endl;
}

list< requesting_chunk > SocketManager :: get_interested_chunk()
{
  list< requesting_chunk > interested_chunk;


  for(unsigned int s=0;s<m_window_controllers.size();s++)
  {
    /** for each window controller that has free slots in his current interest window */
    int free_slots=m_window_controllers[s]->free_slot_win();

    for(int i=0; i<free_slots;i++)
    {
      bool timed_out=false;
      
      /** check if the window controller of a given interface has interest to reissue */
      pair<int,double> reissue =  m_window_controllers[s]->timed_out_chunk();
      
      int selected_chunk = reissue.first;
      /** select a chunk that has not been yet requested (for the first time) and received 
       */
      if(selected_chunk == -1)
	selected_chunk = select_chunk();
      else
	timed_out=true;
      
//       int last_inseq = (* m_missing_chunks.begin());
// 
//       cout << Sim::clock() << " " << m_content_name << " win size: " << m_window_controllers[s]->interest_win_size() << " selected_chunk " << selected_chunk << " last_inseq " << last_inseq << endl;
//       if ( (m_window_controllers[s]->interest_win_size() < (selected_chunk-last_inseq)) && (m_options.request_order == ProtocolOptions::INSEQUENCE))
//       { 
// 	cout << Sim::clock() << " " << m_content_name << " freezed " << endl;
// 	break;
//       }
      
      
      /** if there is an interesed chunk, prepare the information for the CCNx Node 
       */
      if(selected_chunk != -1)
      {
	/** remove the chunk from the list from which it was selected
	 */
	if (timed_out)
	  m_window_controllers[s]->pop_timed_out();
	else
	  remove_notreq(selected_chunk);
	requesting_chunk request;
	request.ifid = m_window_controllers[s]->ifid();
	request.chunk_id = selected_chunk;
	/** start a new time-out on the interested chunk
	 */
	interested_chunk.push_back(request);
	/** insert the chunk id in the appropriate window controller with the time at which
	 *  the chunk id was firt requested
	 */
	if(reissue.second > 0)
	  m_window_controllers[s]->request_chunk(selected_chunk,reissue.second);
	else
	  m_window_controllers[s]->request_chunk(selected_chunk,Sim::clock());
	continue;
      }
    }
  }
  return interested_chunk;

}

double SocketManager::avg_win()
{
  double average = 0;
  for(unsigned int i=0;i<m_window_controllers.size();i++)
  {
    average += m_window_controllers[i]->average_win();
  }
  
  return average/(double)m_window_controllers.size();
}