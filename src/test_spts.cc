// -*- C++ -*-
//
//  This file is part of CBCBSIM, the Combined Broadcast Content-Based
//  (CBCB) routing simulation.
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
// $Id: test_spts.cc,v 1.1 2004/03/20 21:48:53 rutherfo Exp $
//
#include <iostream>
#include <set>

#include <siena/ssim.h>

#include "cbcbsim/SPTsBroadcastLayer.h"

using namespace cbcbsim;
using namespace std;
using namespace ssim;

int main(int argc, char *argv[]) {

    SPTsBroadcastLayer BL;
    ProcessId from, to;
    int len;

    set<ProcessId> nodes;

    while(cin >> from >> to >> len) {
	nodes.insert(from);
	nodes.insert(to);
	BL.addLink(from, to, len);
    }
    BL.computePaths();
    set<ProcessId>::const_iterator i, j;

    for(i = nodes.begin(); i != nodes.end(); ++i)
	for(j = nodes.begin(); j != nodes.end(); ++j) {
	    cout << *i << "->" << *j << ": ";
	    ProcessId curs = *i;
	    cout << *i;
	    for(;;) {
		if (curs == *j) break;
		curs = BL.getParent(*j, curs);
		if (curs == NULL_PROCESSID) break;
		cout << " " << curs;
	    }
	    cout << endl;
	}
    return 0;
}
