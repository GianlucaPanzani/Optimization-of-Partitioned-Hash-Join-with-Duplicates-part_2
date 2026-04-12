CXX            		= g++ -std=c++17
OPTFLAGS       		= -O3 -march=native
AVXFLAGS       		= -mavx2
AUTOFLAGS      		= -fopt-info-vec-optimized -fopt-info-vec-missed
NOVECFLAGS     		= -fno-tree-vectorize
CXXFLAGS       	    = -Wall -Wextra -I.
LIBS           		=

TARGETS 			= generate_datasets plain_novec plain_vec avx2
LIB_SOURCES    		= lib/dataset.cpp lib/timing.cpp lib/checksum.cpp lib/results.cpp lib/partition.cpp lib/config.cpp
AVX_SOURCES 		= lib/partition_avx2.cpp
GCC_VEC_REPORT  	= compilation/gcc_vec_report.txt
GCC_NOVEC_REPORT	= compilation/gcc_novec_report.txt
GCC_AVX2_REPORT 	= compilation/gcc_avx2_report.txt
RESULTS_FILES		=
LOG_FILES			= out/*.log err/*.log


.PHONY: all clean cleanlogs cleanall

all: $(TARGETS)

generate_datasets: generate_datasets.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ generate_datasets.cpp $(LIB_SOURCES) $(LIBS)

plain_novec: main.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(NOVECFLAGS) -o $@ main.cpp $(LIB_SOURCES) $(LIBS) 2> $(GCC_NOVEC_REPORT)

plain_vec: main.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ main.cpp $(LIB_SOURCES) $(LIBS) 2> $(GCC_VEC_REPORT)

avx2: main.cpp $(LIB_SOURCES) $(AVX_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AVXFLAGS) $(AUTOFLAGS) -DENABLE_AVX2 -o $@ main.cpp $(LIB_SOURCES) $(AVX_SOURCES) $(LIBS) 2> $(GCC_AVX2_REPORT)

clean:
	-rm -fr *.o *~

cleanlogs: clean
	-rm -fr $(LOG_FILES)

cleanall: clean
	-rm -fr $(TARGETS) $(GCC_VEC_REPORT) $(GCC_NOVEC_REPORT) $(GCC_AVX2_REPORT) $(RESULTS_FILES) $(LOG_FILES)
