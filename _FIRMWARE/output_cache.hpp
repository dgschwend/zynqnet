//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  output_cache.hpp
//
//  Output Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef OUTPUT_CACHE_HPP_07571FC2
#define OUTPUT_CACHE_HPP_07571FC2

// Data Types for FPGA Implementation
#include "fpga_top.hpp"

// ================
// = Output Cache =
// ================
namespace OutputCache {

  void OutputCache(const char* name = "OutputCache");
  void accumulateChannel(channel_t co, data_t data);
  data_t getChannel(channel_t c);
  void setChannel(channel_t c, data_t data);
  void reset();

  extern data_t OBRAM[MAX_NUM_CHOUT];
};

#endif /* end of include guard: OUTPUT_CACHE_HPP_07571FC2 */
