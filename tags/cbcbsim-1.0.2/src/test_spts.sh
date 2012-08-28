#!/bin/sh
#
#  This file is part of CBCBSIM, the Combined Broadcast Content-Based
#  (CBCB) routing simulation.
#
#  Authors: See the file AUTHORS for full details. 
#
#  Copyright (C) 2002-2004 University of Colorado
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
#  USA, or send email to serl@cs.colorado.edu.
#
# $Id: test_spts.sh,v 1.1 2004/03/20 21:48:53 rutherfo Exp $
#
. $srcdir/test_common.sh
#
cleanup_files='test.out test.expected test.log'
#
test_description "Testing broadcast layer based on shortest-paths trees..."
#
./test_spts <<EOF > test.out
10 11 1
11 12 1
12 13 1
13 10 1
EOF
cat <<EOF > test.expected
10->10: 10
10->11: 10 11
10->12: 10 13 12
10->13: 10 13
11->10: 11 10
11->11: 11
11->12: 11 12
11->13: 11 12 13
12->10: 12 13 10
12->11: 12 11
12->12: 12
12->13: 12 13
13->10: 13 10
13->11: 13 12 11
13->12: 13 12
13->13: 13
EOF
if diff test.out test.expected
then
    test_passed
else
    test_failed 'see test.out, test.expected, and test.log for detail'
fi
