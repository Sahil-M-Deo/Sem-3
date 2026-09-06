# Makefile
#
# Builds and runs `drift_runner_is_back`: four candidate containers
# competing to store the drift_runner log (see README.md for the
# full story).
SHELL := /bin/bash

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g -Wall -std=c++20

# Source files
SOURCES = containers.cpp main.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable
EXEC = drift_runner_is_back

# Number of Test Cases
NUM_TEST_CASES = 3

# Testcase Files
TCDIR = tests

# Default target
all: build runtests

build: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)

# Compile source files to object files
%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

runtests: $(EXEC)
	@echo -e "Running Test Cases"
	@for i in `seq 1 $(NUM_TEST_CASES)`; do \
		echo -e "\033[0;36m\nTestcase $$i\033[0m"; \
		echo -e "./$(EXEC) < $(TCDIR)/test$$i/input.txt > output$$i.txt"; \
		./$(EXEC) < $(TCDIR)/test$$i/input.txt > output$$i.txt; \
		cat output$$i.txt; \
	done

# Plotting is a separate, manual step -- it does NOT run as part of
# `make` or `make runtests`, since a graph window would block an
# automated test run. Run it yourself once you want to *see* the
# growth curves, e.g.:
#   make plot                 # plots test1's output
#   make plot TEST=test2      # plots test2's output
TEST ?= test1
plot: $(EXEC)
	./$(EXEC) < $(TCDIR)/$(TEST)/input.txt > plot_data.txt
	python3 ./plot.py plot_data.txt

# Clean up object files and executable (Unix-like commands)
clean:
	rm -f $(OBJECTS) $(EXEC)
	rm -rf run.data output*.txt plot_data.txt plot_data_plot.png
	find . -type f -name '*~'  -exec rm {} +
