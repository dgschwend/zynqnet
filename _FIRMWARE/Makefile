NETWORK = #./networks/AllPoolToSq3x3S2

CC = g++
CFLAGS = -I. -I./vivado_include -I$(NETWORK) -Wall -Wno-unknown-pragmas -Wno-unused-label -O3
# Change in Makefile or Network should trigger recompile, too:
DEPS = *.h Makefile $(NETWORK)/*
CPP_FILES = $(wildcard *.cpp) $(wildcard $(NETWORK)/*.cpp)
OBJS = $(CPP_FILES: .cpp=.o)

all: compileandlink 

run: compileandlink
	#-rm test.out
	-./test
	-./test.exe

loadbit: 
#Load Bitstream
	loadbit ./cnn_on_fpga_200MHz.bit
#Set AXI HP0 to 32bit
	axi_hp_config 0 32
	
debug: CFLAGS += -DEBUG
debug: compileandlink
#cp $(NETWORK)/indata.bin indata.bin
	./test 2>&1 | tee test.out

clean:
	-rm *.o
	-rm test
	-rm test.out
	-rm -rf test.dSYM/

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compileandlink: $(OBJS)
#copy weights binary from network to root directory
#cp $(NETWORK)/weights.bin weights.bin
	$(CC) -o test $^ $(CFLAGS)

.PHONY: all test clean run debug loadbit