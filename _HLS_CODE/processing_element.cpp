//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  processing_element.cpp
//
//  Processing (MACC) Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "processing_element.hpp"

// =============
// = Variables =
// =============

kernel_t ProcessingElement::kernel;
bool ProcessingElement::relu;

// ======================
// = Processing Element =
// ======================

void ProcessingElement::setLayerConfig(const layer_t layer) {
  kernel = layer.kernel;
  relu = layer.relu;
  // ch_out = layer.channels_out;
  // this->ch_in = chin;

  // if (LOG_DETAILS) {
  //     LOG("PE: setLayerConfig\n");
  //     LOG(" - kernel   = %d\n", (int)kernel);
  //     LOG(" - ch_out   = %d\n", (int)ch_out);
  //     LOG(" - ch_in    = %d\n", (int)ch_in);
  // }
}

void ProcessingElement::processInputChannel(const coordinate_t y,
                                            const coordinate_t x,
                                            const channel_t ci_in,
                                            const channel_t ch_out) {
#pragma HLS inline off
#pragma HLS FUNCTION_INSTANTIATE variable = ci_in

#pragma HLS dataflow

  channel_t ci = ci_in;
  weightaddr_t ci_offset;
  data_t pixel_buffer[9];
#pragma HLS ARRAY_PARTITION variable = pixel_buffer complete dim = 0

  LOG("PE: processInputChannel (ci: %2d)\n", (int)ci);
  LOG_LEVEL_INCR;

  // Preload Image Pixel Buffer (fetch pixels around (y,x,ci))
  preloadPixelsAndPrecalcCIoffset(y, x, ci, ch_out, ci_offset, pixel_buffer);

  // MACC All Output Channels
  processAllCHout(ch_out, ci, ci_offset, pixel_buffer);

  LOG_LEVEL_DECR;
}

void ProcessingElement::processAllCHout(const channel_t ch_out,
                                        const channel_t ci,
                                        const weightaddr_t ci_offset,
                                        const data_t pixels[9]) {
#pragma HLS INLINE off

  //#pragma HLS INTERFACE ap_fifo register port=ch_out
  //#pragma HLS INTERFACE ap_fifo register port=ci
  //#pragma HLS INTERFACE ap_fifo register port=ci_offset_divN_PE
  //#pragma HLS INTERFACE ap_fifo register port=pixels

  LOG("PE: processAllCHout\n");
  LOG_LEVEL_INCR;

L_CH_OUT:
  for (channel_t co = 0; co < ch_out; co++) {
#pragma HLS LOOP_TRIPCOUNT min = 16 max = 1024 avg = 258
#pragma HLS unroll factor = N_PE
#pragma HLS PIPELINE II = 1

    data_t result, weights_local[9];
#pragma HLS ARRAY_PARTITION variable = weights_local complete dim = 0

    LOG(" - process output channel %d\n", (int)co);
    LOG_LEVEL_INCR;

    // fetch weights
    WeightsCache::getNineWeights(co, ci_offset, weights_local);

    // multiply-accumulate
    macc2d(pixels, weights_local, result);

    // save result to Output Buffer
    if (ci == 0) {
      OutputCache::setChannel(co, result);
    } else {
      OutputCache::accumulateChannel(co, result);
    }

    LOG_LEVEL_DECR;
  };
  LOG_LEVEL_DECR;
}

void ProcessingElement::preloadPixels(const coordinate_t y_center,
                                      const coordinate_t x_center,
                                      const channel_t ci, data_t buffer[9]) {
#pragma HLS inline

  LOG("PE: preloadPixels (y_center: %2d, x_center: %2d, ci: %2d)\n",
      (int)y_center, (int)x_center, (int)ci);

#pragma HLS pipeline

L_PE_loadPixel_Y:
  for (int j = 0; j < 3; j++) {
    coordinate_t y = y_center + j - 1;
    imgcacheaddr_t y_offset = ImageCache::precalcYOffset(y);

  L_PE_loadPixel_X:
    for (int i = 0; i < 3; i++) {
      coordinate_t x = x_center + i - 1;

      data_t px = reg(ImageCache::getPixel(y, y_offset, x, ci));
      buffer[j * 3 + i] = px;

      if (LOG_DETAILS)
        LOG(" - loaded (y: %2d, x: %2d, ci: %2d) = %6.2f\n", (int)y, (int)x,
            (int)ci, px);
    }
  }

  LOG("preloadPixels:[ %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f ]\n",
      buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
      buffer[6], buffer[7], buffer[8])
}

void ProcessingElement::preloadPixelsAndPrecalcCIoffset(
    const coordinate_t y, const coordinate_t x, const channel_t ci,
    const channel_t ch_out, weightaddr_t& ci_offset, data_t buffer[9]) {
#pragma HLS inline off
#pragma HLS pipeline II = 7

  LOG("PE: preloadPixelsAndPrecalcCIoffset (ci: %2d, ch_out: %2d)\n", (int)ci,
      (int)ch_out);

  // Preload Pixels from BRAM Image Cache into buffer
  preloadPixels(y, x, ci, buffer);

  // Precalculate Offset into WCache (start of filters for input channel ci)
  ci_offset = WeightsCache::precalcInputOffset(ci);
}

void ProcessingElement::macc2d(const data_t pixels[9], const data_t weights[9],
                               data_t& result) {
#pragma HLS inline

  data_t accumulator = 0.0f;
  data_t multresult[9];
#pragma HLS ARRAY_PARTITION variable = multresult complete dim = 0

L_MACC_multiply:
  for (int i = 0; i < 9; i++) {
#pragma HLS UNROLL
    multresult[i] = pixels[i] * weights[i];
  }

L_MACC_accumulate:
  for (int i = 0; i < 9; i++) {
#pragma HLS UNROLL
    accumulator = accumulator + multresult[i];
  }

  LOG("PE: macc2D -> %.2f \n", accumulator);

  result = accumulator;
}

data_t ProcessingElement::postprocess(const channel_t co,
                                      const weightaddr_t ci_offset) {
#pragma HLS INLINE
  LOG("Postprocess CO=%d:\n", (int)co);
  LOG_LEVEL_INCR;

  data_t raw, biased, rectified, result;

  // Read output channel from Cache
  raw = OutputCache::getChannel(co);
  // Add Bias
  biased = raw + WeightsCache::getOneWeight(co, ci_offset);
  // Nonlinearity: ReLU
  rectified = (biased < 0) ? 0.0f : biased;

  // ReLU needed?
  result = relu ? rectified : biased;

  LOG("raw: %8.2f > biased: %8.2f > rectified: %8.2f > result: %8.2f\n", raw,
      biased, rectified, result);
  LOG_LEVEL_DECR;

  return result;
}
