//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  weights_cache.cpp
//
//  Weights Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "weights_cache.hpp"

// =============
// = Variables =
// =============

weightaddr_t WeightsCache::write_addr;
data_t WeightsCache::WBRAM[N_PE][NUM_BRAMS_PER_PE][BLOCK_SIZE][9];
// WBRAM explanation: see weights_cache.hpp
kernel_t WeightsCache::kernel;
channel_t WeightsCache::ch_out;
channel_t WeightsCache::ch_in;
weightaddr_t WeightsCache::current_ci;
numfilterelems_t WeightsCache::weights_per_filter;
weightaddr_t WeightsCache::num_weights;

// =================
// = Weights Cache =
// =================

// Memory Addressing:

weightaddr_t WeightsCache::precalcInputOffset(const channel_t ci) {
#pragma HLS inline

  assert((ch_out % N_PE) == 0 && "Output Channels are no multiple of N_PE.");

  weightaddr_t ci_times_ch_out = ci * ch_out;
#pragma HLS RESOURCE variable = ci_times_ch_out core = MulnS latency = 2

  LOG("WeightCache: precalcInputOffset (ci = %d) -> ci_offset = %d\n", (int)ci,
      (int)ci_times_ch_out);

  return ci_times_ch_out;
}

void WeightsCache::getAddrForSingleWeight(const channel_t co,
                                          const weightaddr_t ci_offset,
                                          PEID_t &PEID, blockID_t &blockID,
                                          rowID_t &rowID,
                                          weightID_t &weightID) {
#pragma HLS INLINE

  if (kernel == 3) {
    // ci_offset = ci * ch_out
    PEID = co % N_PE;
    blockID = (((ci_offset + co) / N_PE)) / BLOCK_SIZE;
    rowID = (((ci_offset + co) / N_PE)) % BLOCK_SIZE;
    weightID = 0;
  } else {  // kernel == 1
    // ci_offset = ci * ch_out
    PEID = co % N_PE;
    blockID = (((ci_offset + co) / N_PE) / 8) / BLOCK_SIZE;
    rowID = (((ci_offset + co) / N_PE) / 8) % BLOCK_SIZE;
    weightID = ((ci_offset + co) / N_PE) % 8;
  }
}

void WeightsCache::loadFromDRAM(data_t *SHARED_DRAM) {
#pragma HLS inline

  LOG("WeightsCache: loadFromDRAM (total %d weights, %d biases)\n",
      (int)(ch_in * ch_out * weights_per_filter), (int)ch_out);
  LOG_LEVEL_INCR;

  // Load Filter Coefficients
  if (LOG_DETAILS)
    LOG("WeightsCache: loading %d weights...\n", (int)num_weights);
  assert(num_weights <= MAX_WEIGHTS_PER_LAYER && "Loading too many Weights!");

  weightaddr_t dram_addr = 0;
// Weights:
L_LOADWEIGHTS_CI:
  for (channel_t ci = 0; ci < ch_in + 1; ci++) {
#pragma HLS LOOP_TRIPCOUNT MIN = 3 AVG = 238 MAX = 1024

    weightaddr_t ci_offset = precalcInputOffset(ci);
    bool bias_or_1x1 = (kernel == 1 | ci == ch_in);
    numfilterelems_t weights_per_filter = (bias_or_1x1) ? 1 : 9;
    weightaddr_t weights_per_ch_out = ch_out * weights_per_filter;

    weightaddr_t addr = 0;
    ap_uint<NBITS(9)> weight_index = 0;
    channel_t co = 0;

  L_LOADWEIGHTS_CO:
    for (addr = 0; addr < weights_per_ch_out; addr++) {
#pragma HLS LOOP_TRIPCOUNT MIN = 16 AVG = 258 MAX = 1024
#pragma HLS PIPELINE II = 2

      data_t weight = MemoryController::loadNextWeight(SHARED_DRAM, dram_addr);
      dram_addr++;

      PEID_t PEID;
      blockID_t blockID;
      rowID_t rowID;
      weightID_t weightID;

      getAddrForSingleWeight(co, ci_offset, PEID, blockID, rowID, weightID);

      if (bias_or_1x1) {
        WBRAM[PEID][blockID][rowID][weightID] = weight;
        LOG(" - save (ci %d, co %d) to WBRAM[%d][%d][%d][%d] = %6.2f\n",
            (int)ci, (int)co, (int)PEID, (int)blockID, (int)rowID,
            (int)weightID, weight);
      } else {  // (kernel == 3)
        WBRAM[PEID][blockID][rowID][weight_index] = weight;
        LOG(" - save (ci %d, co %d, i %d) to WBRAM[%d][%d][%d][%d] = %6.2f\n",
            (int)ci, (int)co, (int)weight_index, (int)PEID, (int)blockID,
            (int)rowID, (int)weight_index, weight);
      }

      weight_index++;
      if (weight_index == weights_per_filter) {
        weight_index = 0;
        co = co + 1;
      }
    }
  }

  if (LOG_DETAILS) LOG("WeightsCache: done loading from DRAM.\n");

  LOG_LEVEL_DECR;
}

void WeightsCache::setLayerConfig(const layer_t &layer,
                                  const weightaddr_t num_weights_new) {
#pragma HLS inline

  kernel = layer.kernel;
  ch_in = layer.channels_in;
  ch_out = layer.channels_out;
  num_weights = num_weights_new;

  write_addr = 0;
  LOG("WeightsCache: setLayerConfig\n");
  LOG(" - kernel          = %d\n", (int)kernel);
  LOG(" - ch_in           = %d\n", (int)ch_in);
  LOG(" - ch_out          = %d\n", (int)ch_out);
  LOG(" - num_weights     = %d\n", (int)num_weights);
}

void WeightsCache::getNineWeights(const channel_t co,
                                  const weightaddr_t ci_offset,
                                  data_t weights_buf[9]) {
#pragma HLS FUNCTION_INSTANTIATE variable = co
#pragma HLS inline

#pragma HLS pipeline

// Array Partitioning
#pragma HLS ARRAY_PARTITION variable = WBRAM complete dim = 1    // PE ID
#pragma HLS ARRAY_PARTITION variable = WBRAM complete dim = 2    // block ID
#pragma HLS ARRAY_PARTITION variable = WBRAM complete dim = 4    // weight ID
#pragma HLS RESOURCE variable = WBRAM core = RAM_S2P_BRAM latency = 3


  // Calculate Memory Address
  PEID_t PEID;
  blockID_t blockID;
  rowID_t rowID;
  weightID_t weightID;
  getAddrForSingleWeight(co, ci_offset, PEID, blockID, rowID, weightID);
  data_t *WBRAM_BLOCK = WBRAM[PEID][blockID][rowID];

  // Fetch Weights into Filter Template
  data_t weights_temp[9];
#pragma HLS array_partition variable = weights_temp complete dim = 0
L_getNineWeights:
  for (int i = 0; i < 9; i++) {
    // Fetch all 9 elements in last dimension into registers (weights_temp)
    weights_temp[i] = WBRAM_BLOCK[i];
    // Fill weights_buf with 0.0f for 1x1 kernel / with weights for 3x3 kernel
    weights_buf[i] = (kernel == 1) ? 0.0f : weights_temp[i];
  }

  // Fill single relevant weight into weights_buf for 1x1 kernel
  if (kernel == 1) weights_buf[4] = weights_temp[weightID];

  // Logging and Debugging
  LOG("WeightsCache: getNineWeights( co=%-2d ) from", (int)co);
  if (kernel == 3) {
    LOG("WCache[%d][%d][%d][...], 3x3\n", (int)PEID, (int)blockID, (int)rowID);
  } else {
    LOG("WCache[%d][%d][%d][%d], 1x1\n", (int)PEID, (int)blockID, (int)rowID,
        (int)weightID);
  }
  LOG("WeightsCache: [ %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f ]\n",
      weights_buf[0], weights_buf[1], weights_buf[2], weights_buf[3],
      weights_buf[4], weights_buf[5], weights_buf[6], weights_buf[7],
      weights_buf[8])
}

data_t WeightsCache::getOneWeight(const channel_t co,
                                  const weightaddr_t ci_offset) {
#pragma HLS FUNCTION_INSTANTIATE variable=co
#pragma HLS pipeline
#pragma HLS inline

  PEID_t PEID;
  blockID_t blockID;
  rowID_t rowID;
  weightID_t weightID;
  getAddrForSingleWeight(co, ci_offset, PEID, blockID, rowID, weightID);
  data_t weight = WBRAM[PEID][blockID][rowID][weightID];

  LOG("WeightsCache: getOneWeight( co=%-2d )  WCache[%d][%d][%d][%d] -> %.2f\n",
      (int)co, (int)PEID, (int)blockID, (int)rowID, (int)weightID, weight);

  return weight;
}
