//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  fpga_top.hpp
//
//  Top-Level Module for SqueezeNetOnFGPA (Header File)
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef _FPGA_TOP_H_
#define _FPGA_TOP_H_

#include <cassert>
#include <cmath>
#include "ap_int.h"

#ifdef __SYNTHESIS__
#include <ap_utils.h>
#include <hls_math.h>
#endif

// Network Configuration + Data Types + Constants
#include "network.hpp"
#include "netconfig.hpp"

// ==========================
// = Architecture Constants =
// ==========================
// Number of Image Cache Lines (need 3, use 4 for simplified Addressing HW)
const int NUM_IMG_CACHE_LINES = 4;
// Number of Processing Elements
const int N_PE = 16;

// ====================
// = Type Definitions =
// ====================
typedef ap_uint<2> cacheline_t;  // cache height = 3 or 4 lines
typedef ap_uint<NBITS(MAX_INPUT_PER_LAYER)> imgdramoffset_t;
typedef ap_uint<NBITS(MAX_IMAGE_CACHE_SIZE)> imgcacheaddr_t;
typedef ap_uint<NBITS(MAX_IMAGE_CACHE_SIZE / 4)> pixelperrow_t;

typedef ap_int<NBITS(MAX_DIMENSION) + 2> coordinate_t;
// coordinates run worst-case from -1 ... +W or +H
// -> need bits for (W or H) + 1 bit more for signed + 1 bit more for +H / +W
// -> could be implemented more efficiently, but coordinates become more ugly.

// ==============================
// = FPGA Top Function / Module =
// ==============================
void fpga_top(layer_t layer, data_t *SHARED_DRAM, unsigned int weights_offset,
              weightaddr_t num_weights, unsigned int input_offset);

#ifndef __SYNTHESIS__
// Register Stage for manual Pipelining:
template <class T>  T reg(T x) {
#pragma HLS pipeline
#pragma HLS inline self off
#pragma HLS interface ap_ctrl_none register port=return
	return x;
}
#endif

// ================================
// = Debugging Output (Helper Fn) =
// ================================
// debug mode, -DEBUG
extern int LOG_LEVEL;
extern bool LOG_DETAILS;
void print_indent(int lvl);

#if defined(EBUG)
#define FNAME() \
  fprintf(stdout, "\n%s (%s, line %d)\n", __func__, __FILE__, __LINE__)
#define DBG(...)                 \
  {                              \
    /*print_indent(LOG_LEVEL);*/ \
    printf(__VA_ARGS__);         \
  }
#define LOG(...)             \
  {                          \
    print_indent(LOG_LEVEL); \
    printf(__VA_ARGS__);     \
  }
#define LOG_LEVEL_INCR LOG_LEVEL++
#define LOG_LEVEL_DECR \
  if (LOG_LEVEL > 0) LOG_LEVEL--
#else
#define FNAME() \
  do {          \
  } while (0)
#define DBG(...) \
  do {           \
  } while (0)
#define LOG(...) \
  {}
#define LOG_LEVEL_INCR \
  {}
#define LOG_LEVEL_DECR \
  {}
#endif  // EBUG

// ===================================================================
// = Pragma Indirection (allows use of DEFINED variables in #pragma) =
// ===================================================================
//#define PRAGMA_SUB(x) _Pragma(#x)
//#define PRAGMA_HLS(x) PRAGMA_SUB(x)

#endif
