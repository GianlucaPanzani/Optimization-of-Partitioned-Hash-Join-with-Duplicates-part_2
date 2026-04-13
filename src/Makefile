CXX            		= g++ -std=c++17
OPTFLAGS       		= -O3 -march=native
AVXFLAGS       		= -mavx2
AUTOFLAGS      		= -fopt-info-vec-optimized -fopt-info-vec-missed
NOVECFLAGS     		= -fno-tree-vectorize
CXXFLAGS       	    = -Wall -Wextra -I.
LIBS           		=

TARGETS 			= generate_datasets hashjoin_sequential hashjoin_parallel
LIB_SOURCES    		= lib/dataset.cpp lib/timing.cpp lib/checksum.cpp lib/results.cpp lib/partition.cpp lib/config.cpp
AVX_SOURCES 		= lib/partition_avx2.cpp
GCC_SEQ_REPORT  	= compilation/gcc_seq_report.txt
GCC_PAR_REPORT		= compilation/gcc_par_report.txt
LOG_FILES			= out/*.log err/*.log


.PHONY: all clean cleanlogs cleanall

all: $(TARGETS)

generate_datasets: generate_datasets.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ generate_datasets.cpp $(LIB_SOURCES) $(LIBS)

hashjoin_sequential: hashjoin_sequential.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_sequential.cpp $(LIB_SOURCES) $(LIBS) 2> $(GCC_SEQ_REPORT)

hashjoin_parallel: hashjoin_parallel.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_parallel.cpp $(LIB_SOURCES) $(LIBS) 2> $(GCC_PAR_REPORT)

clean:
	-rm -fr *.o *~

cleanlogs: clean
	-rm -fr $(LOG_FILES)

cleanall: clean
	-rm -fr $(TARGETS) $(GCC_SEQ_REPORT) $(GCC_PAR_REPORT) $(LOG_FILES)