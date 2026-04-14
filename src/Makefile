CXX            		= g++
CXXFLAGS       	    = -std=c++17 -Wall -Wextra -I.
OPTFLAGS       		= -O3 -pthread -march=native
AUTOFLAGS      		= -fopt-info-vec-optimized -fopt-info-vec-missed

SEQ_TARGETS 		= hashjoin_seq
PAR_P_TARGETS 		= hashjoin_par_p
PAR_PJ_TARGETS 		= hashjoin_par_pj

GCC_SEQ_REPORT  	= compilation/gcc_seq_report.txt
GCC_PAR_P_REPORT	= compilation/gcc_par_p_report.txt
GCC_PAR_PJ_REPORT	= compilation/gcc_par_pj_report.txt

RES_SEQ				= results/hashjoin_seq.csv
RES_PAR_P			= results/hashjoin_par_p.csv
RES_PAR_PJ			= results/hashjoin_par_pj.csv
RESULTS_FILES		= results/*.csv

LIB_SOURCES    		= lib/timing.cpp
LOG_FILES			= out/*.log err/*.log


.PHONY: all clean cleanlogs cleanallseq cleanallpar cleanall

all: $(TARGETS)

hashjoin_seq: hashjoin_seq.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_seq.cpp $(LIB_SOURCES) 2> $(GCC_SEQ_REPORT)

hashjoin_par_p: hashjoin_par_p.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_par_p.cpp $(LIB_SOURCES) 2> $(GCC_PAR_P_REPORT)

hashjoin_par_pj: hashjoin_par_pj.cpp $(LIB_SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(AUTOFLAGS) -o $@ hashjoin_par_pj.cpp $(LIB_SOURCES) 2> $(GCC_PAR_PJ_REPORT)

clean:
	-rm -fr *.o *~

cleanlogs: clean
	-rm -fr $(LOG_FILES)

cleanall_seq: clean
	-rm -fr $(SEQ_TARGETS) $(GCC_SEQ_REPORT) $(GCC_SEQ_REPORT) $(LOG_FILES) $(RES_SEQ)

cleanall_par_p: clean
	-rm -fr $(PAR_P_TARGETS) $(GCC_SEQ_REPORT) $(GCC_PAR_REPORT) $(LOG_FILES) $(RES_PAR_P)

cleanall_par_pj: clean
	-rm -fr $(PAR_PJ_TARGETS) $(GCC_SEQ_REPORT) $(GCC_PAR_REPORT) $(LOG_FILES) $(RES_PAR_PJ)

cleanall: clean
	-rm -fr $(SEQ_TARGETS) $(PAR_TARGETS) $(GCC_SEQ_REPORT) $(GCC_PAR_REPORT) $(LOG_FILES) $(RESULTS_FILES)
