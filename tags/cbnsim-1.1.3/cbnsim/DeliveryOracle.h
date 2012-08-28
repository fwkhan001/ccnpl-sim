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
// $Id: DeliveryOracle.h,v 1.11 2004/06/16 08:31:03 hallcp Exp $
//
#ifndef CBNSIM_DELIVERY_ORACLE_H
#define CBNSIM_DELIVERY_ORACLE_H

#include <iostream>

#include <map>
#include <vector>
#include <list>
#include <siena/forwarding.h>
#include <siena/ssim.h>
#include <siena/types.h>
#include <siena/ssimp_types.h>
#include "cbnsim/types.h"

using namespace std;
using namespace ssim;

namespace cbnsim
{
  /** @brief Class used to compute and report false-positive and false-negative
   * message receipts.
   * <p>
   * Whenever a <code>set_predicate</code> workload event is
   * encountered, the <code>Driver</code> notifies
   * <code>DeliveryOracle</code> which maintains instantaneous
   * predicate knowledge for the system.
   * <p>
   * When a <code>send_message</code> workload event is encountered
   * the <code>Driver</code> notifies <code>DeliveryOracle</code>
   * which computes the set of <code>Node</code> processes that should
   * receive it.
   * <p>
   * The key to getting accurate numbers is the placement of the
   * calls to <code>DeliveryOracle::close_message</code>,
   * <code>DeliveryOracle::log_false_delivery</code>,
   * and <code>DeliveryOracle::log_true_delivery</code>.
   * The <code>MessageId</code> parameter
   * of these messages must be the MessageId that was passed into
   * the <code>Node::send_message</code> method. 
   */
class DeliveryOracle {
public:
    /** @brief Removes the least recent packet window from the moving
	average and creates a new, blank, window in for the next DCI.
     */
    static void update_dci_windows();

    /** @brief Returns the percentage of ideal message deliveries that are
     * false negatives.
     */
    static double false_negative_percent();

    /** @brief Returns the number of message deliveries that are false
     * negatives.
     */
    static unsigned int false_negative_count();

    /** @brief Returns the percentage of ideal message deliveries that are
     * false positives.
     */
    static double false_positive_percent();

    /** @brief Returns the number of message deliveries that are false
     * negatives.
     */
    static unsigned int false_positive_count();

    /** @brief Logs a false message delivery.  A false message
     * delivery is any message delivery that cannot be processed by
     * the receiving <code>Node</code>. This typically means that a
     * <code>Node</code> received a message delivery that did not
     * match its local applications and that could not be relayed to
     * other <code>Node</code> processes.
     */
    static void log_false_delivery( MessageId message_id, ssim::ProcessId pid );

    /** @brief Logs the receipt of a true message delivery. A true
     * message delivery is one that the receiving <code>Node</code>
     * can process by either delivering the message to its local
     * applications or to its neighboring <code>Node</code> processes.
     */
    static void log_true_delivery( MessageId message_id, ssim::ProcessId pid );

    /** @brief Method called by <code>Driver</code> when a
     * <code>send_message</code> workload event is encountered. The
     * <code>DeliveryOracle</code> generates a unique message id each
     * time, and calculates the list of <code>Node</code> processes
     * that should receive this based on its instantaneous knowledge
     * of the system.
     */
    static MessageId send_message( ssim::ProcessId pid, const Message& m );

    /** @brief Method called by <code>Driver</code> when a <code>set_predicate</code>
     * workload event is encountered. The <code>DeliveryOracle</code> updates its
     * internal forwarding table that is used for making delivery predictions.
     */
    static void set_predicate( ssim::ProcessId pid, const Predicate& p );
    
    static void download_content( ssim::ProcessId pid, const Predicate& p );

private:
    static void update_fwd_table();

    typedef std::map<ssim::ProcessId, Predicate> predicate_map_t;
    static predicate_map_t   s_predicate_map;
    static bool		     s_dirty_pmap;
    static siena::FwdTable   s_fwd_table;
    static MessageId         s_message_id_counter;

    struct DCIWindow {
	DCIWindow() : m_false_positive_cnt(0), m_delivered_cnt(0) {}

	list<MessageId>   m_sent_messages;
	unsigned int     m_false_positive_cnt;
	unsigned int     m_delivered_cnt;
    };

    static const unsigned int ORACLE_WINDOW_SIZE = 3;
    static vector<DCIWindow>  s_windows;
};

};
#endif
