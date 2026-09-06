#===============================================================================
# Tiny Processing Unit (TPU)
# Makefile for Icarus Verilog
#===============================================================================

# Tools
IVERILOG := iverilog
VVP      := vvp

# Directories
RTL_DIR := rtl
TB_DIR  := tb
BUILD   := build

# Output files
SIM      := $(BUILD)/sim.out
VCD      := $(BUILD)/dump.vcd

# Compiler flags
SVFLAGS := -g2012 -Wall
SRC := \
	$(RTL_DIR)/alu.sv \
	$(RTL_DIR)/regfile.sv \
	$(RTL_DIR)/top.sv \
	$(TB_DIR)/tb_top.sv

ifdef DEBUG
SVFLAGS += -DDEBUG
endif

#===============================================================================
# Targets
#===============================================================================

.PHONY: all run wave clean debug help

all: run

$(BUILD):
	mkdir -p $(BUILD)

$(SIM): $(SRC) | $(BUILD)
	$(IVERILOG) $(SVFLAGS) \
	    -DDUMPFILE=\"$(VCD)\" \
	    -I$(RTL_DIR) \
	    -o $@ \
	    $(SRC)

run: $(SIM)
	$(VVP) $(SIM)

debug:
	$(MAKE) DEBUG=1 run

clean:
	rm -rf $(BUILD)

help:
	@echo ""
	@echo "Tiny Processing Unit (TPU)"
	@echo ""
	@echo "Targets:"
	@echo "  make         Compile and run simulation"
	@echo "  make run     Compile and run simulation"
	@echo "  make debug   Compile with DEBUG enabled"
	@echo "  make clean   Remove generated files"
	@echo ""