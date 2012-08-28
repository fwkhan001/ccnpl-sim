## Process this file with automake to produce Makefile.in
SSIM_DIR = @SSIM_DIR@
SFF_DIR = @SFF_DIR@
SSIMP_DIR = @SSIMP_DIR@
SSBG_DIR = @SSBG_DIR@
CBNSIM_DIR = @CBNSIM_DIR@

AM_CXXFLAGS = -O2
AM_LDFLAGS = -O2
AM_YFLAGS = -d
INCLUDES = -I$(top_srcdir)/include

EXTRA_DIST = test_spts.sh test_common.sh

# Programs
bin_PROGRAMS = cbcbsim

cbcbsim_SOURCES = \
	CBCBNode.cc \
	CBR.cc \
        CCNxNode.cc \
	CacheRND.cc \
	CacheLRU.cc \
        ICP.cc \
	Socket.cc \
	SocketManager.cc \
	CBCBSimulation.cc \
	LoadBalancing.cc \
	MSTBroadcastLayer.cc \
	PredicateIssueTimer.cc \
	PITable.cc \
	PredicateRefreshTimer.cc \
	SPTsBroadcastLayer.cc \
	SR.cc \
	SRSendTimer.cc \
	cbcbsim.cc \
	CBCBDataCollector.cc \
	OutputQueue.cc \
	RAQM.cc \
	RAQMsource.cc \
	RTTstats.cc \
	FlowControllerAIMD.cc


check_PROGRAMS = test_MSTBroadcastLayer test_spts

run_cbcbsim:
	$(TESTS_ENVIRONMENT) ./cbcbsim -h

TESTS = test_MSTBroadcastLayer test_spts.sh

TESTS_ENVIRONMENT = \
	QUIET=1 && \
	export QUIET && \
	LD_LIBRARY_PATH=$(SFF_DIR)/lib:$(SSIM_DIR)/lib:$(SSIMP_DIR)/lib:$(SSBG_DIR)/lib:$(CBNSIM_DIR)/lib:$(LD_LIBRARY_PATH) && \
	export LD_LIBRARY_PATH &&

test_spts_SOURCES = test_spts.cc SPTsBroadcastLayer.cc

test_MSTBroadcastLayer_SOURCES = \
	test_MSTBroadcastLayer.cc MSTBroadcastLayer.cc

test.wl: $(CBNSIM_DIR)/bin/cbnwlgen
	$(CBNSIM_DIR)/bin/cbnwlgen -p 20% -s 20% -l 60 \
		-wtu 0.01 -ttu 0.001 \
		-topo 0005.brite \
		-seed 0 \
		-pt 600 \
		-st 60 \
		-t0-pred > $@

test.out: cbcbsim test.wl
	$(TESTS_ENVIRONMENT) ./cbcbsim \
		-o test.out \
		-dci 60 \
		-w test.wl

# - END TEST STUFF -
