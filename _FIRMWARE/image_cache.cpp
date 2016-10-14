//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  image_cache.cpp
//
//  Image Cache Module for FPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "image_cache.hpp"

// =============
// = Variables =
// =============

data_t ImageCache::IBRAM[MAX_IMAGE_CACHE_SIZE];
cacheline_t ImageCache::curr_img_cache_line;
imgcacheaddr_t ImageCache::curr_img_cache_addr;
imgcacheaddr_t ImageCache::line_width;
imgdramoffset_t ImageCache::loads_left;
dimension_t ImageCache::width_in;
dimension_t ImageCache::height_in;
channel_t ImageCache::ch_in;

// ===============
// = Image Cache =
// ===============

void ImageCache::reset() {
#pragma HLS inline

	LOG("ImageCache: Reset Write Pointers.\n");

	curr_img_cache_line = 0;
	curr_img_cache_addr = 0;
}

void ImageCache::setNextChannel(data_t value) {
	imgcacheaddr_t MAX_ADDR = (line_width * NUM_IMG_CACHE_LINES - 1);
	if (LOG_DETAILS)
		LOG("ImageCache: setNextChannel ICACHE[%3d] <- %.2f.\n",
				(int)curr_img_cache_addr, value);

	// Write Value into IBRAM
	IBRAM[curr_img_cache_addr] = value;

	// Check and Wrap Write Address into IBRAM
	 if (curr_img_cache_addr == MAX_ADDR)
		 curr_img_cache_addr = 0;
	 else
		 curr_img_cache_addr++;
}

void ImageCache::preloadPixelFromDRAM(data_t *SHARED_DRAM) {
#pragma HLS inline

	LOG("ImageCache: preloadPixelFromDRAM (%2d channels)\n", (int)ch_in);

	if (loads_left < ch_in) {
		LOG("ImageCache: NO MORE PIXELS LEFT IN DRAM\n");
		return;
	}

	LOG_LEVEL_INCR;
	L_PRELOAD_PIXEL_FROM_DRAM: for (channel_t ci = 0; ci < ch_in; ci++) {
#pragma HLS LOOP_TRIPCOUNT min = 3 max = 1024 avg = 237
#pragma HLS pipeline II = 1
#pragma HLS latency min=4

		data_t px = MemoryController::loadNextChannel(SHARED_DRAM);
		setNextChannel(px);
	}

	loads_left = loads_left - ch_in;

	LOG_LEVEL_DECR;
}

void ImageCache::preloadRowFromDRAM(data_t *SHARED_DRAM) {
#pragma HLS inline

	LOG("ImageCache: preloadRowFromDRAM (%2d pixels, each %2d channels)\n",
			(int)width_in, (int)ch_in);

	LOG_LEVEL_INCR;

	L_DRAM_PRELOADROW_X: for (coordinate_t x = 0; x < width_in; x++) {
#pragma HLS LOOP_TRIPCOUNT min = 8 max = 256 avg = 45
		preloadPixelFromDRAM(SHARED_DRAM);
	}

	LOG_LEVEL_DECR;
}

void ImageCache::setLayerConfig(layer_t &layer) {
#pragma HLS inline

	width_in = layer.width;
	height_in = layer.height;
	ch_in = layer.channels_in;
	line_width = ch_in * width_in;
	loads_left = line_width * height_in;
	curr_img_cache_addr = 0;
#pragma HLS Resource variable = loads_left core = MulnS latency = 2

	LOG("ImageCache: setLayerConfig\n");
	LOG(" - width_in        = %d\n", (int)width_in);
	LOG(" - ch_in           = %d\n", (int)ch_in);
	LOG(" - line_width      = %d\n", (int)line_width);
	LOG(" - height_in       = %d\n", (int)height_in);
	LOG(" - loads_left      = %d\n", (int)loads_left);

	reset();
}

imgcacheaddr_t ImageCache::precalcYOffset(const coordinate_t y) {
#pragma HLS inline
	cacheline_t req_line = (y) % NUM_IMG_CACHE_LINES;
	imgcacheaddr_t addr_line_offset = req_line * line_width;
#pragma HLS RESOURCE variable=addr_line_offset core=MulnS latency=2
	return addr_line_offset;
}

data_t ImageCache::getPixel(const coordinate_t y, const imgcacheaddr_t y_offset,
		const coordinate_t x, const channel_t ci) {
#pragma HLS inline
#pragma HLS RESOURCE variable = IBRAM core = RAM_S2P_BRAM

	//cacheline_t req_line = (y) % NUM_IMG_CACHE_LINES;
	//imgcacheaddr_t addr_line_offset = req_line * line_width;
//#pragma HLS RESOURCE variable=addr_line_offset core=MulnS latency=2
	imgcacheaddr_t addr_pixel_offset = x * ch_in;
	imgcacheaddr_t addr = y_offset + addr_pixel_offset + ci;

	bool is_padding_pixel = x < 0 | x >= width_in | y < 0 | y >= height_in;

	data_t px = is_padding_pixel ? 0.0f : IBRAM[addr];

	LOG("ImageCache: getPixel( y: %d, x: %d, ci: %d ) %s -> %.2f\n", (int)y,
			(int)x, (int)ci, is_padding_pixel ? "PAD" : "", px);

	return px;
}
