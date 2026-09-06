SHELL := /bin/bash
CXX := g++
CXXFLAGS = -g -Wall -std=c++20
SOURCES = range-sum-bst.cpp main.cpp
HEADERS = common.h rangeSumBST.h
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = rangeSumBST
NUM_TEST_CASES = 10
TCDIR = tests

all: build runtests

build: $(EXEC)

run: 
	./$(EXEC)
        

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)
        

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
    
   
runtests: $(EXEC)
	@echo -e "Running Test Cases"
	@for i in $$(seq 1 $(NUM_TEST_CASES)); do \
        	echo -e "\033[0;36m\nTestcase $$i\033[0m"; \
        	if [ ! -f $(TCDIR)/test$$i/input.txt ]; then \
                	echo -e "\033[0;33mSKIPPED: $(TCDIR)/test$$i/input.txt not found\033[0m"; \
                	continue; \
                fi; \
                #echo -e "./$(EXEC) < $(TCDIR)/test$$i/input.txt > output.txt"; \
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

clean:
	rm -f $(OBJECTS) $(EXEC)
	find . -type f -name '*~' -exec rm {} +
        
