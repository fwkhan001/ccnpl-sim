#include "cbcbsim/FlowControllerAIMD.h"
#include <cbnsim/ReceiverTO.h>


using namespace cbcbsim;
using namespace ssim;
using namespace std;


void FlowControllerAIMD :: receiver_TO(int chunk_id)
{
  boost::unordered_map<unsigned int, Time >::iterator it;
  it=m_interest_win.find(chunk_id);
  if (it != m_interest_win.end())
  {
    m_reissuing_chunks.insert(make_pair(chunk_id,(*it).second));
    m_interest_win.erase(it);
    
    if((m_options.rbfcp == ProtocolOptions::RBFCP_AIMD ))
      decrease_win();
  }
}


int FlowControllerAIMD :: request_chunk( unsigned int interested_chunk , double time) 
{
  stringstream port;  port << m_port;
  string  socket_address = m_content_name + "/port:" + port.str();  
  Sim::signal_event( m_id, new cbnsim::ReceiverTO(m_ifid,socket_address,interested_chunk,m_start_time), m_timer);
  m_interest_win.insert( make_pair(interested_chunk,time));
  return 1;
}

std::pair<int,double> FlowControllerAIMD :: timed_out_chunk()
{
  if(m_reissuing_chunks.size()>0)
  {
    std::map<int, double>::iterator it=m_reissuing_chunks.begin();
    std::pair<int,double> result = make_pair((*it).first,(*it).second);
    return result;
  }
  return make_pair(-1,0);
}

void FlowControllerAIMD :: pop_timed_out()
{
  std::map<int, double>::iterator it=m_reissuing_chunks.begin();
  m_reissuing_chunks.erase(it);
}
