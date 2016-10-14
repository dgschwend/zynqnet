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

#ifndef CPU_TOP_H
#define CPU_TOP_H

// ========================
// = Standard C Libraries =
// ========================
#include <cstdio>     // printf
#include <ctime>      // time() for random seed
#include <sys/time.h> // gettimeofday() for performance measurement
#include <cmath>      // fabs, fmax, ...
#include <vector>     // std::vector for softmax calculation
#include <algorithm>  // sort, reverse (on std::vector)

// ===========================
// = CNN Network Definitions =
// ===========================
#include "network.hpp"    // load before netconfig.hpp for bit-width calculation
#include "netconfig.hpp"  // network config (layer_t, network_t)
#include "fpga_top.hpp"   // Software Simulation Code
#include "xfpga.hpp"      // Hardware Block Wrapper

// ===========================================
// = CPU-Side Functions for SqueezeNetOnFPGA =
// ===========================================
void allocate_DRAM_memory(network_t *net_CPU);
void copy_weights_to_DRAM(network_t *net_CPU);
void load_prepared_input_image(layer_t &layer, data_t *input_image,
                                const char *filename);
void copy_input_image_to_DRAM(layer_t &layer, data_t *image);
void copy_results_from_DRAM(data_t *results, int ch_out);
void calculate_softmax(network_t *net_CPU, data_t *results,
                       std::vector<std::pair<data_t, int> > &probabilities);
data_t *allocate_image_memory(layer_t &layer);
int main(int argc, char **argv);

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

#endif // #ifdef CPU_TOP_H
