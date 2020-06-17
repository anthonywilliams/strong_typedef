.PHONY: test

ifeq ($(OS),Windows_NT)
EXE_SUFFIX=.exe
RUN_PREFIX=
else
EXE_SUFFIX=
RUN_PREFIX=./
endif

ifeq ($(CXX),cl)
CXXFLAGS=/std:c++17 /EHsc
OUTPUTFLAG=/Fe
else
CXXFLAGS=-std=c++17
OUTPUTFLAG=-o 
endif

TEST_EXE=test_strong_typedef$(EXE_SUFFIX)

test: $(TEST_EXE)
	$(RUN_PREFIX)$(TEST_EXE)

$(TEST_EXE): test_strong_typedef.cpp strong_typedef.hpp
	$(CXX) $(CXXFLAGS) $(OUTPUTFLAG)$@ $<
