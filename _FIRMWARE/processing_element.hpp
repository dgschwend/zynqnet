//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  processing_element.hpp
//
//  Processing (MACC) Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef PROCESSING_ELEMENT_HPP_C609EBE9
#define PROCESSING_ELEMENT_HPP_C609EBE9

// Data Types for FPGA Implementation
#include "weights_cache.hpp"
#include "image_cache.hpp"
#include "output_cache.hpp"
#include "fpga_top.hpp"

// ======================
// = Processing Element =
// ======================
namespace ProcessingElement {

  void setLayerConfig(const layer_t layer);
  void processInputChannel(const coordinate_t y, const coordinate_t x,
                           const channel_t ci, const channel_t ch_out);
  data_t postprocess(const channel_t co, const weightaddr_t ci_offset);

  void preloadPixelsAndPrecalcCIoffset(const coordinate_t y,
                                       const coordinate_t x, const channel_t ci,
                                       const channel_t ch_out,
                                       weightaddr_t &ci_offset, data_t buffer[9]);
  void preloadPixels(const coordinate_t y, const coordinate_t x,
                     const channel_t ci, data_t buffer[9]);
  void processAllCHout(const channel_t ch_out, const channel_t ci,
                       const weightaddr_t ci_offset, const data_t pixels[9]);
  void macc2d(const data_t pixels[9], const data_t weights[9], data_t &result);

  extern kernel_t kernel;
  extern bool relu;
};

#endif /* end of include guard: PROCESSING_ELEMENT_HPP_C609EBE9 */
