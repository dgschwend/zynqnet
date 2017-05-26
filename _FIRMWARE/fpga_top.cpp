//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  fpga_top.cpp
//
//  Top-Level Module for SqueezeNetOnFGPA.
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "fpga_top.hpp"

// FPGA Modules
#include "memory_controller.hpp"
#include "image_cache.hpp"
#include "weights_cache.hpp"
#include "output_cache.hpp"
#include "gpool_cache.hpp"
#include "processing_element.hpp"

// ==============
// =  FPGA TOP  =
// ==============
void fpga_top(layer_t layer, data_t *SHARED_DRAM, unsigned int weights_offset,
              weightaddr_t num_weights, unsigned int input_offset) {
#pragma HLS INTERFACE m_axi depth = DRAM_DEPTH port = SHARED_DRAM offset = \
    slave bundle = memorybus register
#pragma HLS INTERFACE s_axilite bundle = axilite port = layer register
#pragma HLS INTERFACE s_axilite bundle = axilite port = num_weights register
#pragma HLS INTERFACE s_axilite bundle = axilite port = weights_offset register
#pragma HLS INTERFACE s_axilite bundle = axilite port = input_offset register
#pragma HLS INTERFACE s_axilite bundle = axilite port = return register

  printf("FPGA: Computing ");
  LOG_LEVEL_INCR;

  // =============================
  // = Module + Memory Instances =
  // =============================

  // DRAM_t DRAM(/*SHARED_DRAM,*/ weights_offset, input_offset);
  /*ImageCache ICache;
  WeightsCache WCache;
  OutputCache OCache("OCache");
  OutputCache GPoolCache("GPoolCache");
  ProcessingElement PE;*/

  coordinate_t y, x;
  channel_t ci, co;

  // Setup Processing Elements
  LOG("Initial Module Setup:\n");
  LOG_LEVEL_INCR;

P_TOP_SETUP : {
  // Setup Memory Controller
  MemoryController::setup(SHARED_DRAM, weights_offset, input_offset);
  // Setup Global Pooling Cache
  // if (layer.pool == POOL_GLOBAL) GPoolCache::reset(); -> in loop.
}
  LOG_LEVEL_DECR;

  // Set Layer Configuration
  LOG("Configure Modules for Layer:\n");
  LOG_LEVEL_INCR;

P_layer_setup : {
P_setLayerConfigs : {
  ImageCache::setLayerConfig(layer);
  WeightsCache::setLayerConfig(layer, num_weights);
  MemoryController::setLayerConfig(layer);
  ProcessingElement::setLayerConfig(layer);
}
  LOG_LEVEL_DECR;

  // Load Weights from DRAM
  WeightsCache::loadFromDRAM(SHARED_DRAM);

  // Preload Row 0 + Pixel (1,0)
  MemoryController::setPixelLoadRow(0);
  ImageCache::preloadRowFromDRAM(SHARED_DRAM);
  MemoryController::setPixelLoadRow(1);
  ImageCache::preloadPixelFromDRAM(SHARED_DRAM);
}

// Y Loop
L_Y:
  for (y = 0; y < layer.height; y++) {
#pragma HLS LOOP_TRIPCOUNT min = 8 max = 256 avg = 45
#ifndef __SYNTHESIS__
    printf(".");
    fflush(stdout);
#endif
    LOG("Y = %d:\n", (int)y);
    LOG_LEVEL_INCR;

  // X Loop
  L_X:
    for (x = 0; x < layer.width; x++) {
#pragma HLS LOOP_TRIPCOUNT min = 8 max = 256 avg = 45
      LOG("X = %d:\n", (int)x);
      LOG_LEVEL_INCR;

    p_pixelSetup : {
      LOG("pixel setup\n")
      // Reset Output Cache -> done in L_CH_OUT for ci==0! ;)
      // OutputCache::reset();
      // Load Next Pixel (automatically checks #pixels left)
      ImageCache::preloadPixelFromDRAM(SHARED_DRAM);
    }

      // Stride-2 Skipping
      if (layer.stride == 2 & (x % 2 | y % 2)) {
        LOG("stride-2, skipping pixel\n");
        LOG_LEVEL_DECR;
        continue;
      }

    // Input Channel Loop
    L_CH_IN:
      for (ci = 0; ci < layer.channels_in; ci++) {
#pragma HLS LOOP_TRIPCOUNT min = 3 max = 1024 avg = 237

        LOG("CI = %d:\n", (int)ci);
        LOG("Start PE for Pixel(%d,%d), ch_in %d, all ch_out\n", (int)y, (int)x,
            (int)ci);
        LOG_LEVEL_INCR;

        ProcessingElement::processInputChannel(y, x, ci, layer.channels_out);

        LOG_LEVEL_DECR;

      }  // end L_CH_IN. Pixel (Y,X) is finished.
      LOG("All CI, CO done for pixel(%d,%d)\n", (int)y, (int)x);

      // ===============
      // = Postprocess =
      // ===============
      LOG("Postprocess Pixel(%d,%d)\n", (int)y, (int)x);

      // Calculate Output Pixel Coordinates
      dimension_t y_out = (layer.stride == 2) ? (int)y / 2 : (int)y;
      dimension_t x_out = (layer.stride == 2) ? (int)x / 2 : (int)x;
      MemoryController::setupPixelWriteback(y_out, x_out);

      LOG_LEVEL_INCR;
      // Select bias coefficients
      // WCache.setInputChannel(layer.channels_in, layer.channels_out);
      weightaddr_t ci_offset =
          WeightsCache::precalcInputOffset(layer.channels_in);


    L_POSTPROCESS:
      for (co = 0; co < layer.channels_out; co++) {
#pragma HLS LOOP_TRIPCOUNT min = 16 max = 1024 avg = 258
#pragma HLS pipeline II = 1

        // Postprocess
        data_t processed = ProcessingElement::postprocess(co, ci_offset);

        // Writeback to DRAM
        MemoryController::writeBackOutputChannel(SHARED_DRAM, co, processed);


        // Accumulate for Global Pooling (if enabled)
        if (layer.global_pool == true) {
          if (x_out == 0 && y_out == 0)
            GPoolCache::setChannel(co, processed);
          else
            GPoolCache::accumulateChannel(co, processed);
        }
      }
      LOG_LEVEL_DECR;

      LOG_LEVEL_DECR;
    }  // L_X

    LOG("One row finished.\n");
    LOG_LEVEL_DECR;
  }  // L_Y
  LOG_LEVEL_DECR;

  // Write Back final Result
  if (layer.global_pool == true) {
    MemoryController::writeBackResult(SHARED_DRAM);
  }

  LOG_LEVEL_DECR;
  printf(" done.\n");

// =============================
// = Debug: Write DRAM to File =
// =============================
// For Debugging: Write DATA Region and WEIGHT Region to files...
#ifndef __SYNTHESIS__
#ifdef EBUG
  system("mkdir blobs");

  int nbytes;
  FILE *outfile;

  // Write INPUT DATA DRAM Region to File:
  char fname[30];
  char layername[10];
  int i = 0;
  while (char c = layer.name[i])
    layername[i++] = (c == '/' | c == ' ') ? '_' : c;
  layername[i] = '\0';
  sprintf(fname, "blobs/IN_%s.bin", layername);
  LOG("Save Input Data to file %s\n", fname);
  outfile = fopen(fname, "wb");
  nbytes = (layer.channels_in * layer.width * layer.height) * sizeof(data_t);
  fwrite(&SHARED_DRAM[input_offset + layer.mem_addr_input], sizeof(char),
         nbytes, outfile);
  fclose(outfile);

  // Write OUTPUT DATA DRAM Region to File:
  // for split_layers, print whole region
  sprintf(fname, "blobs/OUT_%s.bin", layername);
  LOG("Save Output Data to file %s\n", fname);
  outfile = fopen(fname, "wb");
  nbytes = layer.channels_out * layer.width * layer.height /
           (layer.stride * layer.stride) * sizeof(data_t);
  if (layer.is_first_split_layer | layer.is_second_split_layer) nbytes *= 2;

  int startaddr = input_offset + layer.mem_addr_output;
  if (layer.is_second_split_layer) startaddr -= layer.channels_out;

  fwrite(&SHARED_DRAM[startaddr], sizeof(char), nbytes, outfile);
  fclose(outfile);

  // Write WEIGHT DRAM Region to File:
  sprintf(fname, "blobs/WEIGHTS_%s.bin", layername);
  LOG("Save Weights Data to file %s\n", fname);
  outfile = fopen(fname, "wb");
  nbytes = (num_weights) * sizeof(data_t);
  fwrite(&SHARED_DRAM[weights_offset + layer.mem_addr_weights], sizeof(char),
         nbytes, outfile);
  fclose(outfile);
#endif
#endif
}
