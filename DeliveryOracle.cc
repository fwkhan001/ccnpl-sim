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
// $Id: DeliveryOracle.cc,v 1.8 2004/06/12 00:43:21 hallcp Exp $
//
#include <iostream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include "cbnsim/DeliveryOracle.h"
#include "cbnsim/MatchHandler.h"
#include "cbnsim/logging.h"

using namespace cbnsim;
using namespace siena;
using namespace ssim;
using namespace std;

siena::FwdTable			  DeliveryOracle::s_fwd_table;
MessageId			  DeliveryOracle::s_message_id_counter( 0 );
bool				  DeliveryOracle::s_dirty_pmap( false );
DeliveryOracle::predicate_map_t	  DeliveryOracle::s_predicate_map;
vector<DeliveryOracle::DCIWindow> DeliveryOracle::s_windows(
					    DeliveryOracle::ORACLE_WINDOW_SIZE);

void DeliveryOracle::log_false_delivery(MessageId message_id, ProcessId pid) {
    CBNDEBUG("DeliveryOracle::log_false_delivery( " << message_id << ", " 
	     << pid << " )");
    //printf("********log_false_delivery(%u, %u) \n", message_id, pid);

    s_windows[0].m_delivered_cnt++;
    s_windows[0].m_false_positive_cnt++;
}

void DeliveryOracle::log_true_delivery(MessageId message_id, ProcessId pid) {
    list<MessageId>::iterator first;
    bool mesg_found = false;
    unsigned int i;

    CBNDEBUG("DeliveryOracle::log_true_delivery( " << message_id << ", " 
	     << pid << " )");
    //printf("*********log_true_delivery(%u, %u) \n", message_id, pid );

    s_windows[0].m_delivered_cnt++;

    for(i = 0; i < ORACLE_WINDOW_SIZE; ++i) {
	first = find(s_windows[i].m_sent_messages.begin(), 
		     s_windows[i].m_sent_messages.end(),
		     message_id);
	if(first != s_windows[i].m_sent_messages.end()) {
	    mesg_found = true;
	    break;
	}
    }

    if(!mesg_found) {
	s_windows[0].m_false_positive_cnt++;
    } else {
	s_windows[i].m_sent_messages.erase(first);
    }
}

MessageId DeliveryOracle::send_message(ProcessId pid, const Message& m) {
    unsigned int size;
    MessageId message_id = ++s_message_id_counter;

    if(s_dirty_pmap){
	update_fwd_table();
    }

    cbnsim::MatchHandler mh;
    s_fwd_table.match(m, mh);

    CBNDEBUG("DeliveryOracle::send_message( " << message_id << " , " << pid
	     << " ): should match " << mh.getIds().size() << " nodes");
    //printf("*****send_message(%u, %u): should match %d nodes \n", message_id, pid, mh.getIds().size());

    size = mh.getIds().size();
    for(unsigned int i = 0; i < size; i++) {
	s_windows[0].m_sent_messages.push_back(message_id);
    }
    //printf("&&&&&&& windows[0].m_sent_messages size is %d. \n", s_windows[0].m_sent_messages.size());
    //printf("&&&&&&& windows[1].m_sent_messages size is %d. \n", s_windows[1].m_sent_messages.size());
    //printf("&&&&&&& windows[2].m_sent_messages size is %d. \n", s_windows[2].m_sent_messages.size());
    return message_id;
}

void DeliveryOracle::set_predicate(ProcessId pid, const Predicate& p) {
    CBNDEBUG("DeliveryOracle::set_predicate( " << pid << " )");
    //printf("*#*#*#*# set_predicate(%u) \n", pid);
    
    s_predicate_map.erase(pid);
    s_predicate_map.insert(make_pair(pid, p));
    s_dirty_pmap = true;
}

void DeliveryOracle::download_content(ProcessId pid, const Predicate& p) {
    CBNDEBUG("DeliveryOracle::download_content( " << pid << " )");
    //printf("*#*#*#*# set_predicate(%u) \n", pid);
    cout << "download_content" << endl;
    //s_predicate_map.erase(pid);
    //s_predicate_map.insert(make_pair(pid, p));
    //s_dirty_pmap = true;
}

void DeliveryOracle::update_fwd_table() {
    s_fwd_table.clear_recycle();

    for(predicate_map_t::const_iterator idx = s_predicate_map.begin();
	idx != s_predicate_map.end(); ++idx)
    {
	s_fwd_table.ifconfig((*idx).first, (*idx).second);
    }

    s_fwd_table.consolidate();
    s_dirty_pmap = false;
}

void DeliveryOracle::update_dci_windows() {
    for(int i = ORACLE_WINDOW_SIZE - 1; i > 0; --i) {
	s_windows[i] = s_windows[i - 1];
    }
    
    s_windows[0].m_sent_messages.clear();
    s_windows[0].m_delivered_cnt = s_windows[0].m_false_positive_cnt = 0;
}

double DeliveryOracle::false_negative_percent() {
    double tot_mesgs, ret;

    tot_mesgs = ret = 0.0;
    for(unsigned int i = 0; i < ORACLE_WINDOW_SIZE; ++i) {
	tot_mesgs += s_windows[i].m_delivered_cnt; 
	   // + s_windows[i].m_sent_messages.size(); 
    }
    tot_mesgs += s_windows[ORACLE_WINDOW_SIZE - 1].m_sent_messages.size();  //liufeng

    if(tot_mesgs != 0) {
	ret = (100.0 * (false_negative_count() / tot_mesgs));
        //printf("&&&& false_negative_count = %d, tot_mesgs = %f \n", false_negative_count(), tot_mesgs);
    }

    return (ret < 1.0e-4 ? 0.0 : ret);
}

unsigned int DeliveryOracle::false_negative_count() {
    unsigned int undelivered = 0;

    //for(unsigned int i = 0; i < ORACLE_WINDOW_SIZE; ++i) {
	undelivered += s_windows[ORACLE_WINDOW_SIZE - 1].m_sent_messages.size();
    //}

    return undelivered;
}

double DeliveryOracle::false_positive_percent() {
    double tot_mesgs, ret;

    tot_mesgs = ret = 0.0;
    for(unsigned int i = 0; i < ORACLE_WINDOW_SIZE; ++i) {
	tot_mesgs += s_windows[i].m_delivered_cnt;
	//           + s_windows[i].m_sent_messages.size();
    }

    if(tot_mesgs != 0) {
	ret = (100.0 * (false_positive_count() / tot_mesgs));
    }

    return ret;    
}

unsigned int DeliveryOracle::false_positive_count() {
    unsigned int false_pos = 0;

    //for(unsigned int i = 0; i < ORACLE_WINDOW_SIZE; ++i) {
	false_pos += s_windows[ORACLE_WINDOW_SIZE - 1].m_false_positive_cnt;
   // }
    
    return false_pos;
}
