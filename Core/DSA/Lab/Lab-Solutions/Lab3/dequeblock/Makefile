# Makefile
SHELL := /bin/bash

CXX = g++
CXXFLAGS = -g -Wall -std=c++20

SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = deque-using-dequeblock
NUM_TEST_CASES = 12
TCDIR = tests

all: build runtests

build: $(EXEC)

run:
	./$(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)

# main.cpp is the only translation unit: the templates in dequeblock.cpp are
# pulled in through the headers, so a change to it must also trigger a rebuild.
IMPL = dequeblock.cpp

%.o: %.cpp *.h $(IMPL)
	$(CXX) $(CXXFLAGS) -c $< -o $@

runtests: $(EXEC)
	@echo -e "Running Test Cases"
	@for i in `seq 1 $(NUM_TEST_CASES)`; do \
		echo -e "\033[0;36m\nTestcase $$i\033[0m"; \
		./$(EXEC) < $(TCDIR)/test$$i/input.txt > output.txt; \
		if diff -Bw output.txt $(TCDIR)/test$$i/output.txt > /dev/null; then \
			echo -e "\033[0;32mPASSED\033[0m"; \
		else \
			diff -Bw output.txt $(TCDIR)/test$$i/output.txt; \
			echo -e "\033[0;31mFAILED\033[0m"; \
		fi \
	done
	@rm -f output.txt

clean:
	rm -f $(OBJECTS) $(EXEC) output.txt
	find . -type f -name '*~' -exec rm {} +
