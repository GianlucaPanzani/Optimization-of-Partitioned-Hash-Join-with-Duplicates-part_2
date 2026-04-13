CXX            		= g++
CXXFLAGS       	    = -std=c++17 -Wall -Wextra -I.
OPTFLAGS       		= -O3 -pthread -march=native
AUTOFLAGS      		= -fopt-info-vec-optimized -fopt-info-vec-missed

TARGETS 			= generate_datasets hashjoin_sequential hashjoin_parallel
LIB_SOURCES    		= 
GCC_SEQ_REPORT  	= compilation/gcc_seq_report.txt
GCC_PAR_REPORT		= compilation/gcc_par_report.txt
LOG_FILES			= out/*.log err/*.log


.PHONY: all clean cleanlogs cleanall

all: $(TARGETS)

generate_datasets: generate_datasets.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ generate_datasets.cpp $(LIB_SOURCES)

hashjoin_sequential: hashjoin_sequential.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_sequential.cpp $(LIB_SOURCES) 2> $(GCC_SEQ_REPORT)

hashjoin_parallel: hashjoin_parallel.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_parallel.cpp $(LIB_SOURCES) 2> $(GCC_PAR_REPORT)

clean:
	-rm -fr *.o *~

cleanlogs: clean
	-rm -fr $(LOG_FILES)

cleanall: clean
	-rm -fr $(TARGETS) $(GCC_SEQ_REPORT) $(GCC_PAR_REPORT) $(LOG_FILES)
