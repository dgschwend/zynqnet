//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  memory_controller.cpp
//
//  Memory Controller Module for FPGA (connects to AXI_M bus --> DMA)
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "memory_controller.hpp"

// =============
// = Variables =
// =============

memaddr_t MemoryController::pixel_output_offset;
memaddr_t MemoryController::layer_weights_offset;
memaddr_t MemoryController::layer_input_offset;
memaddr_t MemoryController::layer_output_offset;
memaddr_t MemoryController::layer_pixel_offset;
pixelperrow_t MemoryController::pixels_per_row;
dimension_t MemoryController::width_out;
channel_t MemoryController::ch_out;
unsigned int MemoryController::dram_weights_offset;
unsigned int MemoryController::dram_data_offset;
bool MemoryController::is_first_split_layer;
bool MemoryController::is_second_split_layer;

// =====================
// = Memory Controller =
// =====================
void MemoryController::setup(data_t* SHARED_DRAM, unsigned int weights_offset,
                             unsigned int data_offset) {
#pragma HLS inline

  dram_weights_offset = (weights_offset);
  dram_data_offset = (data_offset);

  LOG("MemoryCtrl: Constructor.\n");
  LOG(" - SHARED_DRAM     = %lu\n", (long)SHARED_DRAM);
  LOG(" - DRAM_WEIGHTS    = @%luB\n", (long)dram_weights_offset);
  LOG(" - DRAM_DATA       = @%luB\n", (long)dram_data_offset);
}

/*void MemoryController::loadConfig(int num_layers, layer_t *configBRAM) {
 loadConfigViaFloatUnion(num_layers, configBRAM);
 LOG("MemoryCtrl: loadConfig (%d layers).\n", (int)num_layers);
 }

 void MemoryController::loadConfigViaFloatUnion(int num_layers,
 layer_t *configBRAM) {
 // Fetch Layer Configuration by Transferring floats + Converting to layer_t
 float floats[NUM_FLOATS_PER_LAYER];
 layer_t layer;
 L_LoadConfig:
 for (numlayers_t l = 0; l < num_layers; l++) {
 #pragma HLS LOOP_TRIPCOUNT min = 26 max = 26 avg = 26
 #pragma HLS pipeline
 memcpy(floats, &SHARED_DRAM[l * NUM_FLOATS_PER_LAYER],
 NUM_FLOATS_PER_LAYER * sizeof(float));
 floatsToLayerT(floats, layer);
 configBRAM[l] = layer;
 }
 }

 void MemoryController::floatsToLayerT(float floats[NUM_FLOATS_PER_LAYER],
 layer_t &layer) {
 // Extract layer_t hidden in fake floats (which are actually uint32s)

 // clang-format not used on this section to keep it compact
 // clang-format off
 char _undef;
 union { float f; unsigned int i;} u;
 //layer.type = LAYER_NONE;  // type not used
 for (int i = 0; i < NET_NAME_MAX_LEN; i++)
 layer.name[i] = _undef;
 u.f = floats[0]; layer.width = u.i;
 u.f = floats[1]; layer.height = u.i;
 u.f = floats[2]; layer.channels_in = u.i;
 u.f = floats[3]; layer.channels_out = u.i;
 u.f = floats[4]; layer.kernel = u.i;
 u.f = floats[5]; layer.stride = u.i;
 u.f = floats[6]; layer.pad = u.i;
 u.f = floats[7]; layer.mem_addr_input = u.i;
 u.f = floats[8]; layer.mem_addr_output = u.i;
 u.f = floats[9]; layer.mem_addr_weights = u.i;
 u.f = floats[10]; layer.is_expand_layer = u.i;
 u.f = floats[11]; layer.pool = (u.i == 1 ? POOL_GLOBAL : POOL_NONE);
 // clang-format on
 }*/

void MemoryController::setLayerConfig(layer_t& layer) {
  layer_weights_offset = layer.mem_addr_weights;
  layer_input_offset = layer.mem_addr_input;
  layer_output_offset = layer.mem_addr_output;
  pixels_per_row = layer.width * layer.channels_in;
  ch_out = layer.channels_out;
  width_out = (layer.stride == 2) ? (layer.width / 2) : (layer.width / 1);
  is_first_split_layer = layer.is_first_split_layer;
  is_second_split_layer = layer.is_second_split_layer;

  LOG("MemoryCtrl: setLayerConfig.\n");
  LOG(" - weights offset  = %6d Elements, DRAM_WEIGHTS @%8luB\n",
      (int)layer_weights_offset, (int)layer_weights_offset * sizeof(data_t));
  LOG(" - input offset    = %6d Elements, DRAM_DATA    @%8luB\n",
      (int)layer_input_offset, (int)layer_input_offset * sizeof(data_t));
  LOG(" - output offset   = %6d Elements, DRAM_DATA    @%8luB\n",
      (int)layer_output_offset, (int)layer_output_offset * sizeof(data_t));
  LOG(" - pixels per row  = %d\n", (int)pixels_per_row);
  LOG(" - ch_out          = %d\n", (int)ch_out);
  LOG(" - width_out       = %d\n", (int)width_out);
  LOG(" - is_first_split_layer = %d\n", (int)is_second_split_layer);
  LOG(" - is_second_split_layer = %d\n", (int)is_first_split_layer);
}

data_t MemoryController::loadNextWeight(data_t* SHARED_DRAM,
                                        weightaddr_t addr) {
#pragma HLS inline
#pragma HLS pipeline
  data_t read = reg(SHARED_DRAM[dram_weights_offset + layer_weights_offset + addr]);
  if (LOG_DETAILS)
    LOG("MemoryCtrl: loadNextWeight  (#%4d from DRAM @%4luB): %6.2f\n",
        (int)layer_weights_offset, (long)addr, read);
  return read;
}

void MemoryController::setPixelLoadRow(coordinate_t y) {
  LOG("MemoryCtrl: setPixelLoadRow (row %2d).\n", (int)y);
  layer_pixel_offset = layer_input_offset + pixels_per_row * y;
}

data_t MemoryController::loadNextChannel(data_t* SHARED_DRAM) {
#pragma HLS inline
#pragma HLS pipeline II=1
  data_t pixel_from_ram = reg(SHARED_DRAM[dram_data_offset + layer_pixel_offset]);
  if (LOG_DETAILS)
    LOG("MemoryCtrl: loadNextChannel (from DRAM @%4luB) -> %.2f\n",
        (int)layer_pixel_offset * sizeof(data_t), pixel_from_ram);
  layer_pixel_offset++;  // increment address for next fetch
  return pixel_from_ram;
};

void MemoryController::setupPixelWriteback(coordinate_t y_out,
                                           coordinate_t x_out) {
#pragma HLS inline
#pragma HLS pipeline

  // Calculate Output Memory Address
  memaddr_t y_offset = y_out * width_out;
#pragma HLS RESOURCE variable = y_offset core = MulnS latency = 2
  memaddr_t xy_offset = y_offset + x_out;
  memaddr_t px_offset = xy_offset * ch_out;
#pragma HLS RESOURCE variable = px_offset core = MulnS latency = 2

  // Leave double space for "expand" layers (more ch_out will be added)
  bool is_split_layer = (is_first_split_layer | is_second_split_layer);
  pixel_output_offset = layer_output_offset +
                        (is_split_layer ? 2 * (int)px_offset : (int)px_offset);

  LOG("MemoryController: setupPixelWriteback (%2d, %2d)\n", (int)y_out,
      (int)x_out);

  LOG(" - writing %2d channels to DRAM @%luB+\n", (int)ch_out,
      (long)pixel_output_offset);
}

void MemoryController::writeBackOutputChannel(data_t* SHARED_DRAM, channel_t co,
                                              data_t data) {
#pragma HLS inline
  LOG_LEVEL_INCR;
  SHARED_DRAM[dram_data_offset + pixel_output_offset + co] = data;
  LOG(" WB ch%d [@%lu]: %6.2f\n", (int)co, (long)(pixel_output_offset + co),
      data);
  LOG_LEVEL_DECR;
}

void MemoryController::writeBackResult(data_t* SHARED_DRAM) {
#pragma HLS inline

  memaddr_t split_offset = (is_second_split_layer) ? (int)ch_out : 0;
  LOG("MemoryCtrl: writeBackResult (%d Bytes) to DRAM @%luB\n",
      (int)(ch_out * sizeof(data_t)), (long)split_offset);

L_writeBackResult:
  for (int i = 0; i < ch_out; i++) {  // ch_out set from last layer
#pragma HLS LOOP_TRIPCOUNT min = 1000 max = 1024 avg = 1000
#pragma HLS pipeline
    SHARED_DRAM[dram_data_offset + split_offset + i] =
        GPoolCache::getChannel(i);
    LOG(" WB ch%d (@%luB): %6.2f\n", (int)i, (long)(split_offset + i),
        GPoolCache::getChannel(i));
  }
}
