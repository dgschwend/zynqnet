//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  image_cache.hpp
//
//  Image Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef IMAGE_CACHE_HPP_94D645A0
#define IMAGE_CACHE_HPP_94D645A0

// Data Types for FPGA Implementation
#include "fpga_top.hpp"

#include "memory_controller.hpp"

// ===============
// = Image Cache =
// ===============
namespace ImageCache {

  void reset();
  void setNextChannel(data_t value);
  void preloadPixelFromDRAM(data_t* SHARED_DRAM);
  void preloadRowFromDRAM(data_t* SHARED_DRAM);
  void setLayerConfig(layer_t &layer);
  data_t getPixel(const coordinate_t y, const imgcacheaddr_t y_offset, const coordinate_t x, const channel_t ci);
  imgcacheaddr_t precalcYOffset(const coordinate_t y);

  extern data_t IBRAM[MAX_IMAGE_CACHE_SIZE];
  extern cacheline_t curr_img_cache_line;
  extern imgcacheaddr_t curr_img_cache_addr;
  extern imgcacheaddr_t line_width;
  extern imgdramoffset_t loads_left;
  extern dimension_t width_in;
  extern dimension_t height_in;
  extern channel_t ch_in;
};

#endif /* end of include guard: IMAGE_CACHE_HPP_94D645A0 */
