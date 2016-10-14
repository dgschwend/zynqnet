//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  output_cache.cpp
//
//  Output Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "output_cache.hpp"

// =============
// = Variables =
// =============

data_t OutputCache::OBRAM[MAX_NUM_CHOUT];

// ================
// = Output Cache =
// ================

void OutputCache::accumulateChannel(channel_t co, data_t value_to_add) {
#pragma HLS inline
//#pragma HLS pipeline

#pragma HLS FUNCTION_INSTANTIATE variable = co
#pragma HLS ARRAY_PARTITION variable = OBRAM cyclic factor = N_PE
#pragma HLS RESOURCE variable=OBRAM core=RAM_T2P_BRAM latency=2

  assert(co < MAX_NUM_CHOUT && "tried to access invalid channel");

  data_t old_ch = getChannel(co); /* BRAM[c] */
  data_t new_ch = old_ch + value_to_add;
  setChannel(co, new_ch); /* BRAM[c] = new_ch; */
  LOG("OCache: accumulateChannel( ch%-2d ) add %+.2f -> %.2f\n", (int)co,
      value_to_add, new_ch);
};

data_t OutputCache::getChannel(channel_t co) {
#pragma HLS inline
  assert(co < MAX_NUM_CHOUT && "tried to read invalid channel");
  if (LOG_DETAILS)
    LOG("OCache: getChannel( ch%-2d ) -> %6.2f\n", (int)co, OBRAM[co]);
  return OBRAM[co];
}

void OutputCache::setChannel(channel_t co, data_t data) {
#pragma HLS inline
#pragma HLS FUNCTION_INSTANTIATE variable = co

  assert(co < MAX_NUM_CHOUT && "tried to write invalid channel");
  if (LOG_DETAILS)
    LOG("OCache: setChannel( ch%-2d ) <- %6.2f\n", (int)co, data);
  OBRAM[co] = data;
}

/*void OutputCache::reset() {
#pragma HLS inline
  LOG("OCache: reset( all ) <- 0.0f\n");
  LOG_LEVEL_INCR;
L_OCache_reset:
  for (int i = 0; i < MAX_NUM_CHOUT; i++) {
    setChannel(i, 0.0f);
    // BRAM[i] = 0.0f;
  }
  LOG_LEVEL_DECR;
}*/
