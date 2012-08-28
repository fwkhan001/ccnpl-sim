## Process this file with automake to produce Makefile.in
CBNSIM_DIR = @CBNSIM_DIR@
SFF_DIR = @SFF_DIR@
SSBG_DIR = @SSBG_DIR@
SSIM_DIR = @SSIM_DIR@
SSIMP_DIR = @SSIMP_DIR@

CAT = @CAT@
CP = @CP@
CUT = @CUT@
ECHO = @ECHO@
GNUPLOT = @GNUPLOT@
GREP = @GREP@
MV = @MV@
TEST = @TEST@

CBNSIM_VERSION = $(shell $(CBNSIM_DIR)/bin/cbnsim-version)
CBNSIM_SHARE_DIR = $(CBNSIM_DIR)/share/cbnsim-$(CBNSIM_VERSION)

CLEANFILES = $(STAT_FILES) test.wl test.out manual.out
EXTRA_DIST = manual.wl eps.gp fn_vs_t.gp fp_vs_t.gp mem_vs_t.gp mmd.gp ra_vs_t.gp scalability.gp srur_vs_t.gp

WLGEN = $(CBNSIM_DIR)/bin/cbnwlgen
WLSTAT = $(CBNSIM_DIR)/bin/cbnwlstat
CBCBSIM_ENV = LD_LIBRARY_PATH=$(CBNSIM_DIR)/lib:$(SFF_DIR)/lib:$(SSBG_DIR)/lib:$(SSIM_DIR)/lib:$(SSIMP_DIR)/lib:$(LD_LIBRARY_PATH) && \
	export LD_LIBRARY_PATH && 
CBCBSIM = $(top_builddir)/src/cbcbsim

#####################
# Debugging Targets #
#####################

TEST_WLFLAGS = -topo 0005.brite -p 1 -s 100% -l 5 -pt 30 -st 120 \
	-attr-min 1 -attr-max 1 -constr-min 1 -constr-max 1 -filt-min 1 -filt-max 1 \
	-wtu 0.01 -ttu 0.001 -seed 0 -p-const
TEST_SFLAGS = -d -i -dci 1 -srsp n -prp n

test.wl: $(WLGEN)
	$< $(TEST_WLFLAGS) > $@

test.out: test.wl
	$(CBCBSIM_ENV) $(CBCBSIM) $(TEST_SFLAGS) < $< > $@

MANUAL_SFLAGS = -d -i -dci 60 -pip i -srsp t 75 -prp n

manual.out: $(top_srcdir)/exp/manual.wl $(CBCBSIM)
	$(CBCBSIM_ENV) $(CBCBSIM) $(MANUAL_SFLAGS) < $< > $@

########################
# Statistical Analyses #
########################

STAT_WL_FLAGS := -topo 0100.brite -p 20% -s 75% -l 120 -pt 20 -st 1440 -wtu 0.01 -ttu 0.001 -seed 0
STAT_FILES = stat.wl mmd.dat mmd.eps

stat.wl: $(WLGEN)
	$< $(STAT_WL_FLAGS) > $@

mmd.dat: $(WLSTAT) stat.wl
	$< -s /dev/null -i stat.wl > $@

mmd.eps: mmd.dat eps.gp mmd.gp
	$(GNUPLOT) $(srcdir)/eps.gp $(srcdir)/mmd.gp > $@

#######################
# Simulation Analyses #
#######################

PLOT = fn_vs_t # default PLOT is always overridden
PLOTS = fn_vs_t fp_vs_t fp_vs_t2 ra_vs_t srur_vs_t mem_vs_t scalability
CLEAN_PLOTS = $(addprefix clean-,$(PLOTS))
EPS_PLOTS = $(addsuffix .eps,$(PLOTS))
RUN_WLS = $(addprefix $(PLOT).wl.,$($(PLOT)_NAMES))
RUN_OUTS = $(addprefix $(PLOT).out.,$($(PLOT)_NAMES))
RUN_DATS = $(addprefix $(PLOT).$($(PLOT)_METRIC).,$($(PLOT)_NAMES))

fn_vs_t_METRIC = INL_FN_PERCENT
fn_vs_t_NAMES = 1 2 3
fn_vs_t_WFLAGS = -p 20% -s 75% -l 1.0 -wtu 0.001 -ttu 0.001 -seed 0 -t0-pred -pt 0.01 
fn_vs_t_WFLAGS1 = -topo 0050.brite
fn_vs_t_WFLAGS2 = -topo 0100.brite
fn_vs_t_WFLAGS3 = -topo 0200.brite
fn_vs_t_SFLAGS = -d -i -dci 0.25 -srsp n -prp n

fp_vs_t_METRIC = INL_FP_PERCENT
fp_vs_t_NAMES = 1 2 3
fp_vs_t_WFLAGS = -p 20% -s 75% -l 360 -wtu 0.1 -ttu 0.001 -seed 0 -pt 30 -st 0 -t0-pred -topo 0025.brite #0050.brite
fp_vs_t_SFLAGS = -i -dci 120 -prp n
fp_vs_t_SFLAGS1 = -srsp t 900
fp_vs_t_SFLAGS2 = -srsp c 900 5
fp_vs_t_SFLAGS3 = -srsp t 600

fp_vs_t2_METRIC = INL_FP_PERCENT
fp_vs_t2_NAMES = 1 2 3
fp_vs_t2_WFLAGS = -p 20% -s 75% -l 360 -wtu 0.01 -ttu 0.001 -seed 0 -pt 60 -st 0 -t0-pred -topo 0025.brite #0050.brite
fp_vs_t2_SFLAGS = -i -dci 120 -prp n
fp_vs_t2_SFLAGS1 = -srsp t 900
fp_vs_t2_SFLAGS2 = -srsp c 900 5
fp_vs_t2_SFLAGS3 = -srsp t 600

ra_vs_t_METRIC = INL_CTRL_PKT_PCT
ra_vs_t_NAMES = 1 2 3
ra_vs_t_WFLAGS = -p 20% -s 75% -l 90 -wtu 0.01 -ttu 0.001 -seed 0 -pt 30 -t0-pred -topo 0100.brite
ra_vs_t_WFLAGS1 = 
ra_vs_t_WFLAGS2 = -st 600
ra_vs_t_WFLAGS3 = -st 300
ra_vs_t_SFLAGS = -i -dci 120 -srsp n -prp n -pip t 200

srur_vs_t_METRIC = INL_SRUR_COUNT
srur_vs_t_NAMES = 1 2 3
srur_vs_t_WFLAGS = -p 20% -s 75% -l 90 -wtu 0.01 -ttu 0.001 -seed 0 -t0-pred -topo 0100.brite
srur_vs_t_SFLAGS = -i -dci 240 -pip i -prp n
srur_vs_t_SFLAGS1 = -srsp c 600 5
srur_vs_t_SFLAGS2 = -srsp t 600
srur_vs_t_SFLAGS3 = -srsp t 480

mem_vs_t_METRIC = INL_MEMORY_USAGE
mem_vs_t_NAMES = 1 2 3
mem_vs_t_WFLAGS = -p 20% -s 75% -l 180 -wtu 0.1 -ttu 0.001 -seed 0 -topo 0050.brite -st 600
mem_vs_t_SFLAGS = -i -dci 240 -pip i -prp n
mem_vs_t_SFLAGS1 = -srsp n
mem_vs_t_SFLAGS2 = -srsp t 900
mem_vs_t_SFLAGS3 = -srsp t 600

scalability_METRIC = FINAL_SRUR_COUNT
scalability_NAMES = 1 2 3 4 5 6 7
scalability_WFLAGS = -p 20% -s 75% -l 30 -filt-min 1 -filt-max 1 -wtu 0.1 -ttu 0.001 -seed 0 -st 0 -t0-pred
scalability_WFLAGS1 = -topo 0050.brite
scalability_WFLAGS2 = -topo 0100.brite
scalability_WFLAGS3 = -topo 0200.brite
scalability_WFLAGS4 = -topo 0300.brite
scalability_WFLAGS5 = -topo 0400.brite
scalability_WFLAGS6 = -topo 0600.brite
scalability_WFLAGS7 = -topo 0800.brite
scalability_SFLAGS = -i -dci 600 -pip i -prp n -srsp t 600

.PHONY: $(CLEAN_PLOTS) $(PLOTS)

$(RUN_WLS): $(PLOT).wl.% : $(WLGEN)
	$< $($(PLOT)_WFLAGS) $($(PLOT)_WFLAGS$*) > $@

$(RUN_OUTS): $(PLOT).out.% : $(PLOT).wl.% $(CBCBSIM)
	$(CBCBSIM_ENV) $(CBCBSIM) $($(PLOT)_SFLAGS) $($(PLOT)_SFLAGS$*) < $< > $@

$(RUN_DATS) : $(PLOT).$($(PLOT)_METRIC).% : $(PLOT).out.%
	$(GREP) $($(PLOT)_METRIC) $< | $(CUT) -f2,3 > $@

$(EPS_PLOTS): $(srcdir)/$(PLOT).gp $(RUN_DATS)
	$(GNUPLOT) < $< > $@

$(CLEAN_PLOTS): clean-% :
	rm -f $(wildcard $*.*)

$(PLOTS):
	$(MAKE) PLOT="$@" $@.eps

FINAL_METRICS = FINAL_SRUR_COUNT

RUN1_FDATS := $(addprefix run1.,$(FINAL_METRICS))
RUN2_FDATS := $(addprefix run2.,$(FINAL_METRICS))
RUN3_FDATS := $(addprefix run3.,$(FINAL_METRICS))
RUN4_FDATS := $(addprefix run4.,$(FINAL_METRICS))
RUN5_FDATS := $(addprefix run5.,$(FINAL_METRICS))
RUN6_FDATS := $(addprefix run6.,$(FINAL_METRICS))
RUN7_FDATS := $(addprefix run7.,$(FINAL_METRICS))

$(RUN1_FDATS): run1.% : run1.out
	$(ECHO) -n "1	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

$(RUN2_FDATS): run2.% : run2.out
	$(ECHO) -n "2	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

$(RUN3_FDATS): run3.% : run3.out
	$(ECHO) -n "3	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

$(RUN4_FDATS): run4.% : run4.out
	$(ECHO) -n "4	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

$(RUN5_FDATS): run5.% : run5.out
	$(ECHO) -n "5	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

$(RUN6_FDATS): run6.% : run6.out
	$(ECHO) -n "6	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

$(RUN7_FDATS): run7.% : run7.out
	$(ECHO) -n "7	" > $@
	$(GREP) $* $< | $(CUT) -f2 >> $@

# scalability:
# 	$(TEST) ! -f scalability.run1.wl || $(CP) -p scalability.run1.wl run1.wl
# 	$(TEST) ! -f scalability.run2.wl || $(CP) -p scalability.run2.wl run2.wl
# 	$(TEST) ! -f scalability.run3.wl || $(CP) -p scalability.run3.wl run3.wl
# 	$(TEST) ! -f scalability.run4.wl || $(CP) -p scalability.run4.wl run4.wl
# 	$(TEST) ! -f scalability.run5.wl || $(CP) -p scalability.run5.wl run5.wl
# 	$(TEST) ! -f scalability.run6.wl || $(CP) -p scalability.run6.wl run6.wl
# 	$(TEST) ! -f scalability.run7.wl || $(CP) -p scalability.run7.wl run7.wl
# 	$(TEST) ! -f scalability.run1.out || $(CP) -p scalability.run1.out run1.out
# 	$(TEST) ! -f scalability.run2.out || $(CP) -p scalability.run2.out run2.out
# 	$(TEST) ! -f scalability.run3.out || $(CP) -p scalability.run3.out run3.out
# 	$(TEST) ! -f scalability.run4.out || $(CP) -p scalability.run4.out run4.out
# 	$(TEST) ! -f scalability.run5.out || $(CP) -p scalability.run5.out run5.out
# 	$(TEST) ! -f scalability.run6.out || $(CP) -p scalability.run6.out run6.out
# 	$(TEST) ! -f scalability.run7.out || $(CP) -p scalability.run7.out run7.out
# 	$(MAKE) \
# 	WFLAGS="-p 20% -s 75% -l 30 -filt-min 1 -filt-max 1 -wtu 0.1 -ttu 0.001 -seed 0 -st 0 -t0-pred" \
# 	run1_WFLAGS="-topo 0050.brite" \
# 	run2_WFLAGS="-topo 0100.brite" \
# 	run3_WFLAGS="-topo 0200.brite" \
# 	run4_WFLAGS="-topo 0300.brite" \
# 	run5_WFLAGS="-topo 0400.brite" \
# 	run6_WFLAGS="-topo 0600.brite" \
# 	run7_WFLAGS="-topo 0800.brite" \
# 	SFLAGS="-i -dci 600 -pip i -prp n -srsp t 600" \
# 	run1_SFLAGS="" \
# 	run2_SFLAGS="" \
# 	run3_SFLAGS="" \
# 	run4_SFLAGS="" \
# 	run5_SFLAGS="" \
# 	run6_SFLAGS="" \
# 	run7_SFLAGS="" \
# 	scalability.eps

# scalability.eps: %.eps : $(srcdir)/%.gp run1.FINAL_SRUR_COUNT run2.FINAL_SRUR_COUNT run3.FINAL_SRUR_COUNT run4.FINAL_SRUR_COUNT run5.FINAL_SRUR_COUNT run6.FINAL_SRUR_COUNT run7.FINAL_SRUR_COUNT 
# 	$(MV) run1.wl scalability.run1.wl
# 	$(MV) run2.wl scalability.run2.wl
# 	$(MV) run3.wl scalability.run3.wl
# 	$(MV) run4.wl scalability.run4.wl
# 	$(MV) run5.wl scalability.run5.wl
# 	$(MV) run6.wl scalability.run6.wl
# 	$(MV) run7.wl scalability.run7.wl
# 	$(MV) run1.out scalability.run1.out
# 	$(MV) run2.out scalability.run2.out
# 	$(MV) run3.out scalability.run3.out
# 	$(MV) run4.out scalability.run4.out
# 	$(MV) run5.out scalability.run5.out
# 	$(MV) run6.out scalability.run6.out
# 	$(MV) run7.out scalability.run7.out
# 	$(MV) run1.FINAL_SRUR_COUNT scalability.run1.FINAL_SRUR_COUNT
# 	$(MV) run2.FINAL_SRUR_COUNT scalability.run2.FINAL_SRUR_COUNT
# 	$(MV) run3.FINAL_SRUR_COUNT scalability.run3.FINAL_SRUR_COUNT
# 	$(MV) run4.FINAL_SRUR_COUNT scalability.run4.FINAL_SRUR_COUNT
# 	$(MV) run5.FINAL_SRUR_COUNT scalability.run5.FINAL_SRUR_COUNT
# 	$(MV) run6.FINAL_SRUR_COUNT scalability.run6.FINAL_SRUR_COUNT
# 	$(MV) run7.FINAL_SRUR_COUNT scalability.run7.FINAL_SRUR_COUNT
# 	$(ECHO) -n "50	" > scalability.dat
# 	$(CAT) scalability.run1.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(ECHO) -n "100	" >> scalability.dat
# 	$(CAT) scalability.run2.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(ECHO) -n "200	" >> scalability.dat
# 	$(CAT) scalability.run3.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(ECHO) -n "300	" >> scalability.dat
# 	$(CAT) scalability.run4.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(ECHO) -n "400	" >> scalability.dat
# 	$(CAT) scalability.run5.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(ECHO) -n "600	" >> scalability.dat
# 	$(CAT) scalability.run6.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(ECHO) -n "800	" >> scalability.dat
# 	$(CAT) scalability.run7.FINAL_SRUR_COUNT | $(CUT) -f2 >> scalability.dat
# 	$(GNUPLOT) < $(srcdir)/$*.gp > $@
