//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  weights_cache.hpp
//
//  Weights Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifndef WEIGHTS_CACHE_HPP_E36181B8
#define WEIGHTS_CACHE_HPP_E36181B8

#include "fpga_top.hpp"

// Data Types for FPGA Implementation
#include "fpga_top.hpp"
#include "memory_controller.hpp"

#define CEIL_DIV(x, y) (((x) + (y)-1) / (y))

// Depth of single BRAM36 in (1K x 32b) configuration
const int BLOCK_SIZE = 1024;
// Number of BRAM36 needed per PE
const int NUM_BRAMS_PER_PE =
    (CEIL_DIV(((MAX_WEIGHTS_PER_LAYER) / 8), BLOCK_SIZE) / N_PE);

// Type Definitions needed
typedef ap_uint<NBITS(N_PE)> PEID_t;
typedef ap_uint<NBITS(NUM_BRAMS_PER_PE)> blockID_t;
typedef ap_uint<NBITS(BLOCK_SIZE)> rowID_t;
typedef ap_uint<NBITS(9)> weightID_t;

// =================
// = Weights Cache =
// =================
namespace WeightsCache {

void setLayerConfig(const layer_t &layer, const weightaddr_t num_weights_new);
void addWeight(const data_t weight);
void getAddrForSingleWeight(const channel_t co, const weightaddr_t ci_offset,
                            PEID_t &PEID, blockID_t &blockID, rowID_t &rowID,
                            weightID_t &weightID);
void loadFromDRAM(data_t *SHARED_DRAM);
weightaddr_t precalcInputOffset(const channel_t ci);
void getNineWeights(const channel_t co, const weightaddr_t ci_offset,
                    data_t wbuffer[9]);
data_t getOneWeight(const channel_t co, const weightaddr_t ci_offset);

extern data_t WBRAM[N_PE][NUM_BRAMS_PER_PE][BLOCK_SIZE][9];
// WBRAM:
// dim0 = PEID = ID of Processing Element associated with this memory portion
// dim1 = blockID = Used to split memory into junks that fit into BRAM32 units
// dim2 = rowID = Row address inside one BRAM32 unit
// dim3 = weightID = last dimension
//         - either contains 1 filter = 9 weights (3x3 kernel)
//         - or 8 individual weights (1x1 kernel or bias values)
// Only 8 of 9 places in dim3 are used for 1x1 kernels + biases for efficient
// addressing (div/mod8=easy, div/mod9=ugly). Therefore dim1 * dim2 needs
// length >= WEIGHTS_PER_LAYER/8, even though it wastes 1/9 of memowry (+12.5%)

extern weightaddr_t write_addr;
extern kernel_t kernel;
extern channel_t ch_out;
extern channel_t ch_in;
extern weightaddr_t current_ci;
extern numfilterelems_t weights_per_filter;
extern weightaddr_t num_weights;
extern data_t PREFETCH[9];
};

#endif /* end of include guard: WEIGHTS_CACHE_HPP_E36181B8 */
