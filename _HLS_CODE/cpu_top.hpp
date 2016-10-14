//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  cpu_top.hpp
//
//  CPU-Side Functions for SqueezeNetOnFPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef _FPGA_SIMULATOR_H_
#define _FPGA_SIMULATOR_H_

// ========================
// = Standard C Libraries =
// ========================
#include <cstdio>     // printf
#include <ctime>      // time() for random seed
#include <cmath>      // fabs, fmax, ...
#include <vector>     // std::vector for softmax calculation
#include <algorithm>  // sort, reverse (on std::vector)

// ===========================
// = CNN Network Definitions =
// ===========================
#include "network.hpp"    // load before netconfig.hpp for bit-width calculation
#include "netconfig.hpp"  // network config (layer_t, network_t)

// ==============
// = Unit Tests =
// ==============
#include "unittests.hpp"  // Unit Tests for Modules

// ==================
// = FPGA Algorithm =
// ==================
#include "fpga_top.hpp"  // top-level FPGA module

// ===========================================
// = CPU-Side Functions for SqueezeNetOnFPGA =
// ===========================================
void allocate_FPGA_memory(network_t *net_CPU);
void copy_config_to_FPGA(network_t *net_CPU);
void load_prepared_input_image(data_t *input_image, const char *filename,
                               int win, int hin, int chin);
void copy_input_image_to_FPGA(network_t *net_CPU, data_t *image);
void copy_results_from_FPGA(network_t *net_CPU, data_t *results, int ch_out);
void calculate_softmax(network_t *net_CPU, data_t *results,
                       std::vector<std::pair<data_t, int> > &probabilities);
void generate_structured_input_image(data_t *input_image, int win, int hin,
                                     int chin);
void generate_random_input_image(data_t *input_image, int win, int hin,
                                 int chin, int seed);
void load_image_file(data_t *input_image, const char *filename, int win,
                     int hin, int chin);
void do_preprocess(data_t *input_image, int win, int hin, int chin);
void setup_FPGA(network_t *net_CPU);
int main();

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

#endif
