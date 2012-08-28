# -*- Makefile -*-
#
#  This file is part of CBNSIM, the Content-Based Networking
#  Simulation Library.
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
# $Id: Makefile.am,v 1.10 2008-04-01 16:44:08 carzanig Exp $
#
#SUBDIRS = .  test
#
INCLUDES = -I$(top_srcdir) -I$(top_builddir)
#
EXTRA_DIST = cbnsim.spec \
	topo/0005.brite \
	topo/0010.brite \
	topo/0025.brite \
	topo/0050.brite \
	topo/0100.brite \
	topo/0200.brite \
	topo/0300.brite \
	topo/0400.brite \
	topo/0600.brite \
	topo/0800.brite \
	topo/1600.brite \
	topo/3200.brite \
	dist/attr_names.dist \
	dist/bool_operators.dist \
	dist/bool_values.dist \
	dist/constr_names.dist \
	dist/double_operators.dist \
	dist/double_values.dist \
	dist/int_operators.dist \
	dist/int_values.dist \
	dist/string_operators.dist \
	dist/string_values.dist \
	dist/types.dist

#
bin_SCRIPTS = cbnsim-version
bin_PROGRAMS = cbnwlgen cbnwlstat

cbnwlgen_SOURCES = cbnwlgen.cc
cbnwlgen_LDFLAGS = -L/sw/lib -L$(top_builddir) -lcbnsim
cbnwlgen_LDADD = libcbnsim.la

cbnwlstat_SOURCES = cbnwlstat.cc
cbnwlstat_LDADD = libcbnsim.la

lib_LTLIBRARIES = libcbnsim.la
libcbnsim_la_SOURCES = \
	CBNSimulation.cc \
	DataCollector.cc \
	DeliveryOracle.cc \
	Driver.cc \
	MatchHandler.cc \
	Node.cc \
	NodeFailParser.cc \
	NodeRecoverParser.cc \
	NodeFailWNP.cc \
	NodeFailWNPF.cc \
	SendMessageParser.cc \
	SendMessageWNP.cc \
	SendMessageWNPF.cc \
	SetPredicateParser.cc \
	SetPredicateWNP.cc \
	SetPredicateWNPF.cc \
	DownloadContentParser.cc \
	PublishContentParser.cc \
	SetMultipathParser.cc \
	TopologyParser.cc \
	WorkloadGenerator.cc \
	WorkloadNodeProcess.cc \
	WorkloadParser.cc \
	brite_parser.yy \
	brite_lexer.ll \
	cbnsim.cc \
	logging.cc \
	Crandom.cc

libcbnsimincludedir = $(includedir)/cbnsim
libcbnsiminclude_HEADERS = \
	cbnsim/CBNSimulation.h \
	cbnsim/DataCollector.h \
	cbnsim/DeliveryOracle.h \
	cbnsim/Driver.h \
	cbnsim/KillerProcess.h \
	cbnsim/MatchHandler.h \
	cbnsim/Node.h \
	cbnsim/NodeEvent.h \
	cbnsim/NodeFactory.h \
	cbnsim/NodeRecoverParser.h \
	cbnsim/DeQueuePacket.h \
	cbnsim/NodeRecoverPacket.h \
	cbnsim/NodeRecoverEvent.h \
	cbnsim/NodeFailEvent.h \
	cbnsim/NodeFailPacket.h \
	cbnsim/NodeFailParser.h \
	cbnsim/NodeFailWNP.h \
	cbnsim/NodeFailWNPF.h \
	cbnsim/Packet.h \
	cbnsim/ReceiverTO.h \
	cbnsim/ReceiverCBR.h \
	cbnsim/SendMessageEvent.h \
	cbnsim/SendMessagePacket.h \
	cbnsim/SendMessageParser.h \
	cbnsim/SendMessageWNP.h \
	cbnsim/SendMessageWNPF.h \
	cbnsim/SetPredicateEvent.h \
	cbnsim/SetPredicatePacket.h \
	cbnsim/SetPredicateParser.h \
	cbnsim/SetPredicateWNP.h \
	cbnsim/SetPredicateWNPF.h \
	cbnsim/DownloadContentEvent.h \
	cbnsim/DownloadContentPacket.h \
	cbnsim/DownloadContentParser.h \
	cbnsim/PublishContentEvent.h \
	cbnsim/PublishContentPacket.h \
	cbnsim/PublishContentParser.h \
	cbnsim/SetMultipathEvent.h \
	cbnsim/SetMultipathPacket.h \
	cbnsim/SetMultipathParser.h \
	cbnsim/TopologyParser.h \
	cbnsim/WorkloadEvent.h \
	cbnsim/WorkloadEventParser.h \
	cbnsim/WorkloadGenerator.h \
	cbnsim/WorkloadNodeProcess.h \
	cbnsim/WorkloadNodeProcessFactory.h \
	cbnsim/WorkloadParser.h \
	cbnsim/argument_exception.h \
	cbnsim/io_exception.h \
	cbnsim/logging.h \
	cbnsim/parse_exception.h \
	cbnsim/types.h \
	cbnsim/Crandom.h

nobase_include_HEADERS = $(libcbnsiminclude_HEADERS)

AM_CFLAGS = -Wall @CFLAGS_prof@
AM_CXXFLAGS = -Wall -I/sw/include @CXXFLAGS_prof@
AM_LDFLAGS = @LDFLAGS_prof@ -L/sw/lib @LDFLAGS_gmp@
AM_YFLAGS = -d

libcbnsim_la_LDFLAGS= \
 -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
 -release $(LT_RELEASE)

.PHONY: release snapshot  test rpm

release:
	rm -rf .deps */.deps
	$(MAKE) distcheck

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-`date +%Y-%m-%d`

#doc:
#	cd doc && $(MAKE) doc

#
# RPM stuff
#
RPMBUILD = @RPMBUILD@
RPMROOT = rpmroot
RPMBASE = rpmbase
RPMTREE = $(RPMBASE) \
	$(RPMBASE)/BUILD $(RPMBASE)/RPMS $(RPMBASE)/SOURCES $(RPMBASE)/SPECS

RPMFLAGS = --define="_topdir `pwd`/$(RPMBASE)" \
	--define='version $(VERSION)' \
	--buildroot="`pwd`/$(RPMROOT)"

$(RPMBASE)/SOURCES/$(PACKAGE)-$(VERSION).tar.gz: dist
	$(CP) $(PACKAGE)-$(VERSION).tar.gz $@

$(RPMBASE)/SPECS/cbnsim.spec: cbnsim.spec
	$(CP) $< $@

$(RPMTREE):
	mkdir $@

rpm: $(RPMTREE) $(RPMBASE)/SOURCES/$(PACKAGE)-$(VERSION).tar.gz \
		$(RPMBASE)/SPECS/cbnsim.spec
	$(RPMBUILD) $(RPMFLAGS) -bb $(RPMBASE)/SPECS/cbnsim.spec
	$(MV) $(RPMBASE)/RPMS/*/$(PACKAGE)-$(VERSION)-*.rpm ./
	$(RM) -rf $(RPMBASE)

#
# installation
#
sharedir=@prefix@/share/$(PACKAGE)-$(VERSION)/
TOPODIR=$(top_srcdir)/topo
DISTDIR=$(top_srcdir)/dist
install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(sharedir)/topo $(DESTDIR)$(sharedir)/dist
# cp -rp [!.]* $(TOPODIR) $(DESTDIR)$(sharedir)
	rsync -aC $(TOPODIR) $(DESTDIR)$(sharedir)
	rsync -aC $(DISTDIR) $(DESTDIR)$(sharedir)

uninstall-local:
	chmod -R u+w $(DESTDIR)$(sharedir)
	$(RM) -rf $(DESTDIR)$(sharedir)

#
# cleaning up...
#
CLEANFILES = $(RPMBASE) $(RPMROOT)
DISTCLEANFILES = config/stamp-h1 so_locations
