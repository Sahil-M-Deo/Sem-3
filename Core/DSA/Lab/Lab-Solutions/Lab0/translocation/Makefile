# Makefile
SHELL := /bin/bash

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g -Wall -std=c++20

# Source files
SOURCES = translocation.cpp main.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable
EXEC = translocation

# Number of Test Cases
NUM_TEST_CASES = 10

# Testcase Files
TCDIR = tests

# Default target
all: build runtests

build: $(EXEC)

run:
	./$(EXEC)

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
		echo -e "./$(EXEC) < $(TCDIR)/test$$i/input.txt > output.txt"; \
		./$(EXEC) < $(TCDIR)/test$$i/input.txt > output.txt; \
		if diff -Bw output.txt $(TCDIR)/test$$i/output.txt > /dev/null; then \
			echo -e "\033[0;32mPASSED\033[0m"; \
		else \
			echo -e "diff -Bw output.txt $(TCDIR)/test$$i/output.txt";\
			diff -Bw output.txt $(TCDIR)/test$$i/output.txt; \
			echo -e "\033[0;31mFAILED\033[0m"; \
		fi \
	done
	@rm -f output.txt

# Clean up object files and executable (Unix-like commands)
clean:
	rm -f $(OBJECTS) $(EXEC)
	find . -type f -name '*~'  -exec rm {} +
