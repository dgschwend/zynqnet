//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  memory_controller.hpp
//
//  Memory Controller Module for FPGA (connects to AXI_M bus --> DMA)
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef MEMORY_CONTROLLER_HPP_EBD6F5A3
#define MEMORY_CONTROLLER_HPP_EBD6F5A3

// Data Types for FPGA Implementation
#include "fpga_top.hpp"
#include "output_cache.hpp"
#include "gpool_cache.hpp"

// =====================
// = Memory Controller =
// =====================
namespace MemoryController {

void setup(data_t *SHARED_DRAM, unsigned int weights_offset,
           unsigned int data_offset);
void setLayerConfig(layer_t &layer);
data_t loadNextWeight(data_t *SHARED_DRAM, weightaddr_t addr);
void setPixelLoadRow(coordinate_t y);
data_t loadNextChannel(data_t *SHARED_DRAM);
void setupPixelWriteback(coordinate_t y_out, coordinate_t x_out);
void writeBackOutputChannel(data_t *SHARED_DRAM, channel_t co, data_t data);
void writeBackResult(data_t *SHARED_DRAM);

extern memaddr_t pixel_output_offset;
extern memaddr_t layer_weights_offset;
extern memaddr_t layer_input_offset;
extern memaddr_t layer_output_offset;
extern memaddr_t layer_pixel_offset;
extern pixelperrow_t pixels_per_row;
extern dimension_t width_out;
extern channel_t ch_out;
extern unsigned int dram_weights_offset;
extern unsigned int dram_data_offset;
extern bool is_first_split_layer;
extern bool is_second_split_layer;

};

#endif /* end of include guard: MEMORY_CONTROLLER_HPP_EBD6F5A3 */
