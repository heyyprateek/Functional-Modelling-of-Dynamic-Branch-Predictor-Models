CC = g++
OPT = -O3
#OPT = -g
STD = -std=c++11
WARN = -Wall
CFLAGS = $(OPT) $(STD) $(WARN) $(INC) $(LIB)

# List all your .cc/.cpp files here (source files, excluding header files)
SIM_SRC = src/sim_bp.cc

# List corresponding compiled object files here (.o files)
SIM_OBJ = src/sim_bp.o
 
#################################

# default rule

all: sim
	@echo "my work is done here..."


# rule for making sim

sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH sim-----------"


# generic rule for converting any .cpp file to any .o file
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc -o $(SIM_OBJ)

.cpp.o:
	$(CC) $(CFLAGS)  -c $*.cpp


# type "make clean" to remove all .o files plus the sim binary

clean:
	rm -f src/*.o sim


# type "make clobber" to remove all .o files (leaves sim binary)

clobber:
	rm -f src/*.o

P?=bimodal
K?=
M1?=
N?=
M2?=
trace_file?=proj2-traces/gcc_trace.txt

test:
	./sim $(P) \
		  $(K) $(M1) \
		  $(N) $(M2) \
		  $(trace_file)


bimodal1: clean all
	mkdir -p out/$@;
	$(MAKE) P=bimodal M2=6 trace_file=proj2-traces/gcc_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_bimodal_1.txt out/$@/$@.txt

bimodal2: clean all
	mkdir -p out/$@;
	$(MAKE) P=bimodal M2=12 trace_file=proj2-traces/gcc_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_bimodal_2.txt out/$@/$@.txt

bimodal3: clean all
	mkdir -p out/$@;
	$(MAKE) P=bimodal M2=4 trace_file=proj2-traces/jpeg_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_bimodal_3.txt out/$@/$@.txt

bimodal4: clean all
	mkdir -p out/$@;
	$(MAKE) P=bimodal M2=5 trace_file=proj2-traces/perl_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_bimodal_4.txt out/$@/$@.txt

gshare1: clean all
	mkdir -p out/$@;
	$(MAKE) P=gshare M1=9 N=3 trace_file=proj2-traces/gcc_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_gshare_1.txt out/$@/$@.txt

gshare2: clean all
	mkdir -p out/$@;
	$(MAKE) P=gshare M1=14 N=8 trace_file=proj2-traces/gcc_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_gshare_2.txt out/$@/$@.txt

gshare3: clean all
	mkdir -p out/$@;
	$(MAKE) P=gshare M1=11 N=5 trace_file=proj2-traces/jpeg_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_gshare_3.txt out/$@/$@.txt


gshare4: clean all
	mkdir -p out/$@;
	$(MAKE) P=gshare M1=10 N=6 trace_file=proj2-traces/perl_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_gshare_4.txt out/$@/$@.txt

hybrid1: clean all
	mkdir -p out/$@
	$(MAKE) P=hybrid K=8 M1=14 N=10 M2=5 trace_file=proj2-traces/gcc_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_hybrid_1.txt out/$@/$@.txt

hybrid2: clean all
	mkdir -p out/$@
	$(MAKE) P=hybrid K=5 M1=10 N=7 M2=5 trace_file=proj2-traces/jpeg_trace.txt test > out/$@/$@.txt
	sed -i '1,/COMMAND/ {/COMMAND/!d}' out/$@/$@.txt
	sed -i '/^make/d' out/$@/$@.txt
	diff -iw --ignore-matching-lines='sim' proj2-validation/val_hybrid_2.txt out/$@/$@.txt

bimodal_all: bimodal1 bimodal2 bimodal3 bimodal4

gshare_all: gshare1 gshare2 gshare3 gshare4

hybrid_all: hybrid1 hybrid2

valrun_all: clean all bimodal_all gshare_all hybrid_all