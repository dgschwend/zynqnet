//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  gpool_cache.hpp
//
//  Global Pooling Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef GPool_CACHE_HPP_07571FC2
#define GPool_CACHE_HPP_07571FC2

// Data Types for FPGA Implementation
#include "fpga_top.hpp"

// ========================
// = Global Pooling Cache =
// ========================
namespace GPoolCache {
  void accumulateChannel(channel_t co, data_t data);
  data_t getChannel(channel_t c);
  void setChannel(channel_t c, data_t data);
  void reset();

  extern data_t GBRAM[MAX_NUM_CHOUT];
};

#endif /* end of include guard: OUTPUT_CACHE_HPP_07571FC2 */
