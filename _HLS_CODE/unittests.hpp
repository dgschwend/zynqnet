//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  unittests.hpp
//
//  Unit Tests for FPGA Modules
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#ifndef UNITTESTS_HPP_A9B1CEAB
#define UNITTESTS_HPP_A9B1CEAB

// ========================
// = Standard C Libraries =
// ========================
#include <cstdio>  // printf
#include <ctime>   // time() for random seed
#include <cmath>   // fabs, fmax, ...

// ===========================
// = CNN Network Definitions =
// ===========================
#include "network.hpp"    // load before netconfig.hpp for bit-width calculation
#include "netconfig.hpp"  // network config (layer_t, network_t)

// ==================
// = FPGA Algorithm =
// ==================
#include "fpga_top.hpp"  // top-level FPGA module

// ==============
// = Unit Tests =
// ==============
bool do_unittests();

#endif /* end of include guard: UNITTESTS_HPP_A9B1CEAB */