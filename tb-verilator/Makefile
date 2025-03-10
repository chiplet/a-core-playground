# Check for sanity to avoid later confusion
ifneq ($(words $(CURDIR)),1)
 $(error Unsupported: GNU Make cannot build in directories containing spaces, build elsewhere: '$(CURDIR)')
endif

######################################################################
# Set up variables

# If $VERILATOR_ROOT isn't in the environment, we assume it is part of a
# package install, and verilator is in your path. Otherwise find the
# binary relative to $VERILATOR_ROOT (such as when inside the git sources).
ifeq ($(VERILATOR_ROOT),)
VERILATOR = verilator
VERILATOR_COVERAGE = verilator_coverage
else
export VERILATOR_ROOT
VERILATOR = $(VERILATOR_ROOT)/bin/verilator
VERILATOR_COVERAGE = $(VERILATOR_ROOT)/bin/verilator_coverage
endif

# Generate C++ in library form
VERILATOR_FLAGS += -cc
VERILATOR_FLAGS += --threads 1
VERILATOR_FLAGS += -CFLAGS "-pthread"
# Optimize
VERILATOR_FLAGS += -x-assign fast
# Warn abount lint issues; may not want this on less solid designs
# VERILATOR_FLAGS += -Wall
# Make waveforms
VERILATOR_FLAGS += --trace
# Check SystemVerilog assertions
VERILATOR_FLAGS += --assert
# Generate coverage analysis
#VERILATOR_FLAGS += --coverage
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
#VERILATOR_FLAGS += --gdbbt

VERILOG_TOP = acorechip

# Verilog sources
VLOG_SOURCES = $(addprefix verilog/, acorechip.v AsyncResetSyncBB.v)
#VLOG_SOURCES = verilog/test_pattern.sv

VERILATOR_SOURCES =
VERILATOR_SOURCES += $(VERILATOR_ROOT)/include/verilated.cpp
VERILATOR_SOURCES += $(VERILATOR_ROOT)/include/verilated_threads.cpp
VERILATOR_SOURCES += $(VERILATOR_ROOT)/include/verilated_vcd_c.cpp

# Input files for Verilator
# VERILATOR_INPUT = -f input.vc top.v sim_main.cpp
VERILATOR_INPUT = --top-module $(VERILOG_TOP) $(VLOG_SOURCES) src/sim_main.cpp #$(wildcard src/*.cpp)

######################################################################
default: run

INCLUDES =
INCLUDES += -I ./include
INCLUDES += -I ./obj_dir
INCLUDES += -I ./verilog/include
INCLUDES += -I $(VERILATOR_ROOT)/include/
INCLUDES += -I $(VERILATOR_ROOT)/include/vltstd
INCLUDES += `sdl2-config --cflags`

AC_LIBS =
AC_LIBS += -L ./obj_dir

CFLAGS =
# multithreading
CFLAGS += -DVL_THREADED=1
CFLAGS += -lpthread

LDFLAGS =
LDFLAGS += -l:V$(VERILOG_TOP)__ALL.a
# LDFLAGS += -pthread
LDFLAGS += `sdl2-config --cflags --libs`

run: build
	@echo
	@echo "-- Verilator tracing example"

	@echo
	@echo "-- RUN ---------------------"
	@rm -rf logs
	@mkdir -p logs
	obj_dir/V$(VERILOG_TOP) doom-riscv.bin +trace

	@echo
	@echo "-- DONE --------------------"
	@echo "To see waveforms, open $(VERILOG_TOP).vcd in a waveform viewer"
	@echo

# Include global rules
include $(VERILATOR_ROOT)/include/verilated.mk

build: verilate
	@echo
	@echo "-- BUILD -------------------"
	g++ $(CFLAGS) -o obj_dir/V$(VERILOG_TOP) -O2 -g -fsanitize=address $(INCLUDES) $(AC_LIBS) src/sim_main.cpp src/sim_display.cpp src/remote_bitbang.cpp $(VERILATOR_SOURCES) $(LDFLAGS)

verilate: verilog/acorechip.v
	@echo
	@echo "-- VERILATE ----------------"
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILATOR_INPUT)
	make -j`nproc` -C ./obj_dir -f V$(VERILOG_TOP).mk

# hackety hack!
# first cocotbify Chisel-generated verilog and then
# remove all delays
verilog/acorechip.v:
	cd ../ACoreChip && ./configure && make core_config=core-doom.yml gen_cheaders=true
	cocotbify -v verilog/acorechip.v -o verilog/acorechip.v
	sed -z -E -i 's/`ifdef COCOTB_SIM((.|\n)*)`endif\n/\1/g; s/ *#1;\n//g' verilog/acorechip.v

# generate program dump to be loaded to simulation


######################################################################
# Other targets

.PHONY: wave
wave:
	gtkwave -T gtkwave/general.tcl acorechip.vcd

show-config:
	$(VERILATOR) -V

maintainer-copy::
clean mostlyclean distclean maintainer-clean::
	-rm -rf obj_dir logs *.log *.dmp *.vpd coverage.dat core
