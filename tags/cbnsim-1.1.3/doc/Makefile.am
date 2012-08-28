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
# $Id: Makefile.am,v 1.5 2008-04-01 16:44:58 carzanig Exp $
#
docdir=@prefix@/share/doc/$(PACKAGE)-$(VERSION)/
EXTRA_DIST = $(DOX_FILES) $(DOCPACKAGE)
#
DOX_FILES = doxygen.conf.in cbnsim.css header.html.in footer.html \
	fdl.txt maindoc.h

DOX_INPUT = doxygen.conf cbnsim.css header.html footer.html	\
	$(top_srcdir)/cbnsim/argument_exception.h		\
	$(top_srcdir)/cbnsim/NodeRecoverParser.h		\
	$(top_srcdir)/cbnsim/CBNSimulation.h			\
	$(top_srcdir)/cbnsim/Packet.h				\
	$(top_srcdir)/cbnsim/DataCollector.h			\
	$(top_srcdir)/cbnsim/parse_exception.h			\
	$(top_srcdir)/cbnsim/DeliveryOracle.h			\
	$(top_srcdir)/cbnsim/SendMessageEvent.h			\
	$(top_srcdir)/cbnsim/Driver.h				\
	$(top_srcdir)/cbnsim/SendMessagePacket.h		\
	$(top_srcdir)/cbnsim/io_exception.h			\
	$(top_srcdir)/cbnsim/SendMessageParser.h		\
	$(top_srcdir)/cbnsim/KillerProcess.h			\
	$(top_srcdir)/cbnsim/SendMessageWNPF.h			\
	$(top_srcdir)/cbnsim/logging.h				\
	$(top_srcdir)/cbnsim/SendMessageWNP.h			\
	$(top_srcdir)/cbnsim/MatchHandler.h			\
	$(top_srcdir)/cbnsim/SetPredicateEvent.h		\
	$(top_srcdir)/cbnsim/NodeEvent.h			\
	$(top_srcdir)/cbnsim/SetPredicatePacket.h		\
	$(top_srcdir)/cbnsim/NodeFactory.h			\
	$(top_srcdir)/cbnsim/SetPredicateParser.h		\
	$(top_srcdir)/cbnsim/NodeFailEvent.h			\
	$(top_srcdir)/cbnsim/SetPredicateWNPF.h			\
	$(top_srcdir)/cbnsim/NodeFailPacket.h			\
	$(top_srcdir)/cbnsim/SetPredicateWNP.h			\
	$(top_srcdir)/cbnsim/NodeFailParser.h			\
	$(top_srcdir)/cbnsim/WorkloadEvent.h			\
	$(top_srcdir)/cbnsim/NodeFailWNPF.h			\
	$(top_srcdir)/cbnsim/WorkloadEventParser.h		\
	$(top_srcdir)/cbnsim/NodeFailWNP.h			\
	$(top_srcdir)/cbnsim/WorkloadGenerator.h		\
	$(top_srcdir)/cbnsim/Node.h				\
	$(top_srcdir)/cbnsim/WorkloadNodeProcessFactory.h	\
	$(top_srcdir)/cbnsim/NodeRecoverEvent.h			\
	$(top_srcdir)/cbnsim/WorkloadNodeProcess.h		\
	$(top_srcdir)/cbnsim/NodeRecoverPacket.h		\
	$(top_srcdir)/cbnsim/WorkloadParser.h			\
	maindoc.h

DOCPACKAGE = docpack.tar.gz
#
HTML_OUTPUT = html
#
all: doc
#
if HAVE_DOXYGEN
.PHONY: doc
doc: $(HTML_OUTPUT)/index.html $(HTML_OUTPUT)/fdl.txt

$(HTML_OUTPUT)/fdl.txt: $(srcdir)/fdl.txt
	$(CP) $(srcdir)/fdl.txt $@

$(HTML_OUTPUT)/index.html: $(DOX_INPUT)
	$(RM) -rf $(HTML_OUTPUT)
	$(DOXYGEN) doxygen.conf

$(DOCPACKAGE): $(HTML_OUTPUT)/index.html $(HTML_OUTPUT)/fdl.txt
	$(TAR) cf - $(HTML_OUTPUT) | $(GZIP) > $@

else
.PHONY: doc
doc: $(DOCPACKAGE)
	$(GZIP) -dc $< | $(TAR) xvf -

endif
# install_doc is a "guard" variable that I use to disable the
# installation of the html documentation during an rpm build.  An rpm
# %install script would do something like 
#    'make DESTDIR=... install_doc=no
#
install_doc=yes
install-data-hook: doc
	if [ $(install_doc) = yes ]; then \
		$(mkinstalldirs) $(DESTDIR)$(docdir) && \
		cp -rp $(HTML_OUTPUT) $(DESTDIR)$(docdir) ; \
	fi

# Automake's "distcheck" is sensitive to having files left over
# after "make uninstall", so we have to clean up the install hook.
uninstall-local:
	$(RM) -rf $(DESTDIR)$(docdir)

clean-local:
	$(RM) -rf $(HTML_OUTPUT) $(DOCPACKAGE) $(DOX_OUTPUT_DIRECTORY)

MAINTAINERCLEANFILES = $(DOCPACKAGE)
