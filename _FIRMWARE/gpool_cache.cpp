//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  gpool_cache.cpp
//
//  Global Pooling Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "gpool_cache.hpp"

// =============
// = Variables =
// =============
data_t GPoolCache::GBRAM[MAX_NUM_CHOUT];

// ================
// = Output Cache =
// ================

void GPoolCache::accumulateChannel(channel_t co, data_t value_to_add) {
#pragma HLS inline

#pragma HLS FUNCTION_INSTANTIATE variable = co
#pragma HLS ARRAY_PARTITION variable = GBRAM cyclic factor = N_PE
#pragma HLS RESOURCE variable=GBRAM core=RAM_T2P_BRAM latency=2

  assert(co < MAX_NUM_CHOUT && "tried to access invalid channel");

#pragma HLS DEPENDENCE variable=GBRAM inter false
  data_t old_ch = GBRAM[co];
  data_t new_ch = old_ch + value_to_add;
  GBRAM[co] = new_ch;
  LOG("GPoolCache: accumulateChannel( ch%-2d ) add %+.2f -> %.2f\n", (int)co,
      value_to_add, new_ch);
};

data_t GPoolCache::getChannel(channel_t c) {
#pragma HLS inline
  assert(c < MAX_NUM_CHOUT && "tried to read invalid channel");
  if (LOG_DETAILS)
    LOG("GPoolCache: getChannel( ch%-2d ) -> %6.2f\n", (int)c, GBRAM[c]);
  return GBRAM[c];
}

void GPoolCache::setChannel(channel_t c, data_t data) {
#pragma HLS inline
  assert(c < MAX_NUM_CHOUT && "tried to write invalid channel");
  if (LOG_DETAILS)
    LOG("GPoolCache: setChannel( ch%-2d ) <- %6.2f\n", (int)c, data);
  GBRAM[c] = data;
}

/*void GPoolCache::reset() {
#pragma HLS inline
  LOG("GPoolCache: reset( all ) <- 0.0f\n");
  LOG_LEVEL_DECR;
L_OCache_reset:
  for (int i = 0; i < MAX_NUM_CHOUT; i++) {
    setChannel(i, 0.0f);
    // BRAM[i] = 0.0f;
  }
  LOG_LEVEL_DECR;
}*/
