//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  unittests.cpp
//
//  Unit Tests for FPGA Modules
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "unittests.hpp"

// FPGA Modules
#include "memory_controller.hpp"
#include "image_cache.hpp"
#include "weights_cache.hpp"
#include "output_cache.hpp"
#include "processing_element.hpp"

// =============================
// = ACTIVATION / DEACTIVATION =
// =============================
#define DO_UNITTESTS 0

// ==================
// = Test Functions =
// ==================
#define EXPECT_EQUAL(...) \
  success &= expect_equal_check(__VA_ARGS__, __func__, __FILE__, __LINE__);

template <typename T1, typename T2>
bool expect_equal_check(T1 acquired, T2 expected, const char* fn,
                        const char* file, int line) {
  if (acquired == expected)
    return true;
  else {
    std::cout << "ERROR: " << acquired << " != " << expected << " in " << fn
              << " (" << file << ", line " << line << ")" << std::endl;
    return false;
  }
}
template <typename T>
bool expect_equal_check(double acquired, T expected, const char* fn,
                        const char* file, int line) {
  if (fabs(acquired - expected) < 1e-4)
    return true;
  else {
    std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2)
              << "ERROR: " << acquired << " != " << expected << " in " << fn
              << " (" << file << ", line " << line << ")" << std::endl;
    return false;
  }
}
template <typename T>
bool expect_equal_check(float acquired, T expected, const char* fn,
                        const char* file, int line) {
  return expect_equal_check((double)acquired, expected, fn, file, line);
}

#if DO_UNITTESTS
// =====================
// = Memory Controller =
// =====================
bool test_MemoryController() {
  LOG_LEVEL = 8;
  printf(" MemoryController Test\n");

  bool success = true;
  const int MEM_SIZE = 2000;

  // Initialize Test Memory [CONFIG 0-100, WEIGHTS 100-500, DATA 500+]
  data_t TEST_MEMORY[MEM_SIZE];
  for (int i = 0; i < MEM_SIZE; i++) {
    TEST_MEMORY[i] = i;
  }

  // Write Layer Config to Test Memory
  // Layer    : ( NAME   , TYPE      ,   W,   H, CI,CO, K, P, S)
  layer_t layer1("LAYER1", LAYER_CONV, 15, 13, 3, 8, 3, 1, 1);
  layer1.mem_addr_weights = 0;
  layer1.mem_addr_input = 0;
  layer1.mem_addr_output = 100;
  layer1.is_expand_layer = true;
  // print_layer(&layer1);
  layer_t layer2("LAYER2", LAYER_NONE, 14, 12, 8, 16, 1, 0, 2);
  layer2.mem_addr_weights = 100;
  layer2.mem_addr_input = 100;
  layer2.mem_addr_output = 200;
  layer2.pool = POOL_GLOBAL;
  layer_to_floats(layer1, &TEST_MEMORY[0]);
  layer_to_floats(layer2, &TEST_MEMORY[12]);

  // Test Constructor
  MemoryController DRAM(TEST_MEMORY, 100, 500);

  printf("    - loadConfig()\n");
  layer_t CONFIG[2];
  layer_t layer;
  DRAM.loadConfig(2, CONFIG);

  // Check Results
  layer = CONFIG[0];
  // print_layer(&layer);
  EXPECT_EQUAL(layer.width, 15);
  EXPECT_EQUAL(layer.height, 13);
  EXPECT_EQUAL(layer.channels_in, 3);
  EXPECT_EQUAL(layer.channels_out, 8);
  EXPECT_EQUAL(layer.kernel, 3);
  EXPECT_EQUAL(layer.pad, 1);
  EXPECT_EQUAL(layer.stride, 1);
  EXPECT_EQUAL(layer.mem_addr_input, 0);
  EXPECT_EQUAL(layer.mem_addr_output, 100);
  EXPECT_EQUAL(layer.mem_addr_weights, 0);
  EXPECT_EQUAL(layer.is_expand_layer, true);
  EXPECT_EQUAL(layer.pool, POOL_NONE);
  layer = CONFIG[1];
  EXPECT_EQUAL(layer.width, 14);
  EXPECT_EQUAL(layer.height, 12);
  EXPECT_EQUAL(layer.channels_in, 8);
  EXPECT_EQUAL(layer.channels_out, 16);
  EXPECT_EQUAL(layer.kernel, 1);
  EXPECT_EQUAL(layer.pad, 0);
  EXPECT_EQUAL(layer.stride, 2);
  EXPECT_EQUAL(layer.mem_addr_input, 100);
  EXPECT_EQUAL(layer.mem_addr_output, 200);
  EXPECT_EQUAL(layer.mem_addr_weights, 100);
  EXPECT_EQUAL(layer.is_expand_layer, false);
  EXPECT_EQUAL(layer.pool, POOL_GLOBAL);

  printf("    - setLayerConfig()\n");
  printf("    - loadNextWeight()\n");
  DRAM.setLayerConfig(CONFIG[0]);  // Should load Weights [100+]
  EXPECT_EQUAL(DRAM.loadNextWeight(), 100);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 101);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 102);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 103);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 104);
  DRAM.setLayerConfig(CONFIG[1]);  // Should load Weights [200+]
  EXPECT_EQUAL(DRAM.loadNextWeight(), 200);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 201);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 202);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 203);
  EXPECT_EQUAL(DRAM.loadNextWeight(), 204);

  printf("    - setLoadRow()\n");
  printf("    - loadNextChannel()\n");
  DRAM.setLayerConfig(CONFIG[0]);
  DRAM.setPixelLoadRow(0);  // Should load Input [500+]
  EXPECT_EQUAL(DRAM.loadNextChannel(), 500);
  EXPECT_EQUAL(DRAM.loadNextChannel(), 501);
  EXPECT_EQUAL(DRAM.loadNextChannel(), 502);
  EXPECT_EQUAL(DRAM.loadNextChannel(), 503);
  DRAM.setPixelLoadRow(1);  // Should load Input [500+3*15]
  EXPECT_EQUAL(DRAM.loadNextChannel(), 545);
  EXPECT_EQUAL(DRAM.loadNextChannel(), 546);
  EXPECT_EQUAL(DRAM.loadNextChannel(), 547);

  printf("    - writeBackOuputPixel()\n");
  DRAM.setLayerConfig(CONFIG[0]);  // Output Addr. is 600+
  data_t OUTPUT[8];
  for (int i = 0; i < 8; i++) OUTPUT[i] = i;

  DRAM.writeBackOutputPixel(0, 0, OUTPUT);
  EXPECT_EQUAL(TEST_MEMORY[600], 0);
  EXPECT_EQUAL(TEST_MEMORY[601], 1);
  EXPECT_EQUAL(TEST_MEMORY[602], 2);
  EXPECT_EQUAL(TEST_MEMORY[603], 3);
  EXPECT_EQUAL(TEST_MEMORY[607], 7);

  DRAM.writeBackOutputPixel(3, 4, OUTPUT);
  // ch_out: 8, width_out: 15, y: 3, x: 4, is_expand_layer -> ch_out=16
  int addr = 600 + 3 * (15 * 16) + 4 * 16;
  EXPECT_EQUAL(TEST_MEMORY[addr + 0], 0);
  EXPECT_EQUAL(TEST_MEMORY[addr + 1], 1);
  EXPECT_EQUAL(TEST_MEMORY[addr + 7], 7);

  DRAM.setLayerConfig(CONFIG[1]);  // Output Addr. is 700+
  DRAM.writeBackOutputPixel(2, 1, OUTPUT);
  // ch_out: 16, width_out: 14, y: 2, x: 1, stride2 -> width_out = 7
  addr = 700 + 2 * (7 * 16) + 1 * 16;
  EXPECT_EQUAL(TEST_MEMORY[addr + 0], 0);
  EXPECT_EQUAL(TEST_MEMORY[addr + 1], 1);
  EXPECT_EQUAL(TEST_MEMORY[addr + 7], 7);

  printf("    - writeBackResult()\n");
  DRAM.setLayerConfig(CONFIG[0]);  // ch_out: 8
  DRAM.writeBackResult(OUTPUT);    // written to 500+
  EXPECT_EQUAL(TEST_MEMORY[500 + 0], 0);
  EXPECT_EQUAL(TEST_MEMORY[500 + 1], 1);
  EXPECT_EQUAL(TEST_MEMORY[500 + 7], 7);

  return success;
}

// =================
// = Weights Cache =
// =================
bool test_WeightsCache() {
  printf(" WeightsCache Test\n");

  bool success = true;
  const int MEM_SIZE = 2000;

  // Initialize Test Memory [CONFIG 0-100, WEIGHTS 100-500, DATA 500+]
  data_t TEST_MEMORY[MEM_SIZE];
  for (int i = 0; i < MEM_SIZE; i++) {
    TEST_MEMORY[i] = i;
  }

  // Initialize Weights for Layer1:
  // chIn = 4, chOut = 5, kernel = 3    (total 180 weights at 100+)
  // weight style: [chIn][chOut].[ky][kx]
  for (int ci = 0; ci < 4; ci++) {
    for (int co = 0; co < 5; co++) {
      for (int ky = 0; ky < 3; ky++) {
        for (int kx = 0; kx < 3; kx++) {
          TEST_MEMORY[100 + ci * 5 * 3 * 3 + co * 3 * 3 + ky * 3 + kx] =
              (ci * 100 + co + ky / 10.0 + kx / 100.0);
        }
      }
    }
  }

  // Initialize Weights for Layer2:
  // chIn = 5, chOut = 4, kernel = 1 (total 20 weights at 300+, internally 180w)
  // weight style: [chIn][chOut].[ky][kx]
  for (int ci = 0; ci < 5; ci++) {
    for (int co = 0; co < 4; co++) {
      TEST_MEMORY[300 + ci * 4 + co] = (ci * 100 + co);
    }
  }

  // Write Layer Config to Test Memory (Layer with ~220 Weights)
  // Layer    : ( NAME   , TYPE    ,   W,  H,CI,CO, K, P, S)
  layer_t layer1("LAYER1", LAYER_CONV, 10, 10, 4, 5, 3, 1, 1);
  layer1.mem_addr_weights = 0;
  layer1.mem_addr_input = 0;
  layer1.mem_addr_output = 100;
  layer1.is_expand_layer = true;
  // print_layer(&layer1);
  layer_t layer2("LAYER2", LAYER_NONE, 14, 12, 5, 4, 1, 0, 2);
  layer2.mem_addr_weights = 200;
  layer2.mem_addr_input = 100;
  layer2.mem_addr_output = 200;
  layer2.pool = POOL_GLOBAL;
  layer_to_floats(layer1, &TEST_MEMORY[0]);
  layer_to_floats(layer2, &TEST_MEMORY[12]);

  // Memory Controller
  MemoryController DRAM(TEST_MEMORY, 100, 500);

  // Weights Cache
  WeightsCache WCache;

  // Setup
  printf("    - setLayerConfig()\n");
  DRAM.setLayerConfig(layer1);
  WCache.setLayerConfig(layer1);

  printf("    - loadfromDRAM()\n");
  WCache.loadFromDRAM(&DRAM);

  // Check Contents
  printf("    - setInputChannel()\n");
  WCache.setInputChannel(0);
  printf("    - getNineWeights()\n");
  data_t weights[9];
  WCache.getNineWeights(0, weights);
  // weight style: [chIn][chOut].[ky][kx]
  EXPECT_EQUAL(weights[0], 0.00);
  EXPECT_EQUAL(weights[1], 0.01);
  EXPECT_EQUAL(weights[2], 0.02);
  EXPECT_EQUAL(weights[3], 0.10);
  EXPECT_EQUAL(weights[4], 0.11);
  EXPECT_EQUAL(weights[5], 0.12);
  EXPECT_EQUAL(weights[6], 0.20);
  EXPECT_EQUAL(weights[7], 0.21);
  EXPECT_EQUAL(weights[8], 0.22);

  WCache.getNineWeights(2, weights);
  // weight style: [chIn][chOut].[ky][kx]
  EXPECT_EQUAL(weights[0], 2.00);
  EXPECT_EQUAL(weights[1], 2.01);
  EXPECT_EQUAL(weights[2], 2.02);
  EXPECT_EQUAL(weights[3], 2.10);
  EXPECT_EQUAL(weights[4], 2.11);
  EXPECT_EQUAL(weights[5], 2.12);
  EXPECT_EQUAL(weights[6], 2.20);
  EXPECT_EQUAL(weights[7], 2.21);
  EXPECT_EQUAL(weights[8], 2.22);

  printf("    - setInputChannel()\n");
  WCache.setInputChannel(3);

  WCache.getNineWeights(2, weights);
  // weight style: [chIn][chOut].[ky][kx]
  EXPECT_EQUAL(weights[0], 302.00);
  EXPECT_EQUAL(weights[1], 302.01);
  EXPECT_EQUAL(weights[2], 302.02);
  EXPECT_EQUAL(weights[3], 302.10);
  EXPECT_EQUAL(weights[4], 302.11);
  EXPECT_EQUAL(weights[5], 302.12);
  EXPECT_EQUAL(weights[6], 302.20);
  EXPECT_EQUAL(weights[7], 302.21);
  EXPECT_EQUAL(weights[8], 302.22);

  printf("    - setLayerConfig()\n");
  DRAM.setLayerConfig(layer2);
  WCache.setLayerConfig(layer2);
  WCache.print_setup();

  printf("    - loadFromDRAM()\n");
  WCache.loadFromDRAM(&DRAM);

  printf("    - sanityCheck\n");
  WCache.print_setup();

  // weight style: [chIn][chOut].[ky][kx]
  printf("    - setInputChannel()\n");
  printf("    - getNineWeights()\n");
  WCache.setInputChannel(0);
  WCache.getNineWeights(1, weights);
  EXPECT_EQUAL(weights[0], 0.00);
  EXPECT_EQUAL(weights[1], 0.00);
  EXPECT_EQUAL(weights[2], 0.00);
  EXPECT_EQUAL(weights[3], 0.00);
  EXPECT_EQUAL(weights[4], 1.00);
  EXPECT_EQUAL(weights[5], 0.00);
  EXPECT_EQUAL(weights[6], 0.00);
  EXPECT_EQUAL(weights[7], 0.00);
  EXPECT_EQUAL(weights[8], 0.00);
  WCache.setInputChannel(3);
  WCache.getNineWeights(3, weights);
  EXPECT_EQUAL(weights[0], 0.00);
  EXPECT_EQUAL(weights[1], 0.00);
  EXPECT_EQUAL(weights[2], 0.00);
  EXPECT_EQUAL(weights[3], 0.00);
  EXPECT_EQUAL(weights[4], 303.00);
  EXPECT_EQUAL(weights[5], 0.00);
  EXPECT_EQUAL(weights[6], 0.00);
  EXPECT_EQUAL(weights[7], 0.00);
  EXPECT_EQUAL(weights[8], 0.00);

  return success;
}

// ===============
// = Image Cache =
// ===============
bool test_ImageCache() {
  printf(" ImageCache Test\n");

  bool success = true;
  const int MEM_SIZE = 2000;

  // Initialize Test Memory [CONFIG 0-100, WEIGHTS 100-500, DATA 500+]
  data_t TEST_MEMORY[MEM_SIZE];
  for (int i = 0; i < MEM_SIZE; i++) {
    TEST_MEMORY[i] = i;
  }

  // Initialize Image Data for Layer1:
  // height = 4, width = 8, ch_in = 3  [96 ch/px, 500+]
  // data style: [y][0x].[ci]
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 8; x++) {
      for (int ci = 0; ci < 3; ci++) {
        TEST_MEMORY[500 + y * 8 * 3 + x * 3 + ci] = 100 * y + x + ci / 10.0;
      }
    }
  }

  // Initialize Image Data for Layer2:
  // height = 4, width = 4, ch_in = 8  [128 ch/px, 600+]
  // data style: [y][0x].[ci]
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      for (int ci = 0; ci < 8; ci++) {
        TEST_MEMORY[800 + y * 4 * 8 + x * 8 + ci] = 100 * y + x + ci / 10.0;
      }
    }
  }

  // Write Layer Config to Test Memory (Layer with ~400 Weights)
  // Layer    : ( NAME   , TYPE    ,   W,  H,CI,CO, K, P, S)
  layer_t layer1("LAYER1", LAYER_CONV, 8, 4, 3, 16, 3, 1, 1);
  layer1.mem_addr_weights = 0;
  layer1.mem_addr_input = 0;
  layer1.mem_addr_output = 400;
  layer1.is_expand_layer = true;
  // print_layer(&layer1);
  layer_t layer2("LAYER2", LAYER_NONE, 4, 4, 8, 4, 1, 0, 2);
  layer2.mem_addr_weights = 200;
  layer2.mem_addr_input = 100;
  layer2.mem_addr_output = 600;
  layer2.pool = POOL_GLOBAL;
  layer_to_floats(layer1, &TEST_MEMORY[0]);
  layer_to_floats(layer2, &TEST_MEMORY[12]);

  // Memory Controller
  MemoryController DRAM(TEST_MEMORY, 100, 500);

  // Weights Cache
  ImageCache ICache;

  // Setup
  printf("    - setLayerConfig()\n");
  DRAM.setLayerConfig(layer1);
  ICache.setLayerConfig(layer1);

  printf("    - setNextChannel()\n");
  ICache.setNextChannel(0.0);
  ICache.setNextChannel(1.0);
  ICache.setNextChannel(2.0);
  ICache.setNextChannel(3.0);

  data_t px;
  px = ICache.getPixel(0, 0, 0);
  EXPECT_EQUAL(px, 0.0);
  px = ICache.getPixel(0, 0, 1);
  EXPECT_EQUAL(px, 1.0);
  px = ICache.getPixel(0, 0, 2);
  EXPECT_EQUAL(px, 2.0);
  px = ICache.getPixel(0, 1, 0);
  EXPECT_EQUAL(px, 3.0);

  printf("    - load from DRAM\n");
  ICache.reset();
  for (int y = 0; y < 4; y++) {
    DRAM.setPixelLoadRow(y);
    for (int x = 0; x < 8; x++) {
      for (int ch = 0; ch < 3; ch++) {
        px = DRAM.loadNextChannel();
        ICache.setNextChannel(px);
      }
    }
  }

  printf("    - getPixel()\n");
  // ICache shoud contain last 3 rows from Image Data [rows 1-3]
  // data style: [y][0x].[ci]
  px = ICache.getPixel(3, 0, 0);
  EXPECT_EQUAL(px, 300.0);
  px = ICache.getPixel(3, 0, 2);
  EXPECT_EQUAL(px, 300.2);
  px = ICache.getPixel(1, 0, 1);
  EXPECT_EQUAL(px, 100.1);
  px = ICache.getPixel(2, 7, 2);
  EXPECT_EQUAL(px, 207.2);

  return success;
}

// ================
// = Output Cache =
// =================
bool test_OutputCache() {
  printf(" OutputCache Test\n");
  bool success = true;

  OutputCache OCache;

  printf("    - setChannel\n");
  for (int i = 0; i < MAX_NUM_CHOUT; i++) {
    OCache.setChannel(i, (float)i);
  }

  printf("    - getChannel\n");
  EXPECT_EQUAL(OCache.getChannel(3), 3.0);
  EXPECT_EQUAL(OCache.getChannel(2), 2.0);
  EXPECT_EQUAL(OCache.getChannel(5), 5.0);

  // set all to i+2
  printf("    - setChannel\n");
  for (int i = 0; i < MAX_NUM_CHOUT; i++) {
    OCache.setChannel(i, i + 2.0);
  }

  EXPECT_EQUAL(OCache.getChannel(3), 5.0);
  EXPECT_EQUAL(OCache.getChannel(2), 4.0);
  EXPECT_EQUAL(OCache.getChannel(5), 7.0);

  // subtract 2
  printf("    - accumulateChannel\n");
  for (int i = 0; i < MAX_NUM_CHOUT; i++) {
    OCache.accumulateChannel(i, -2.0);
  }

  EXPECT_EQUAL(OCache.getChannel(3), 3.0);
  EXPECT_EQUAL(OCache.getChannel(2), 2.0);
  EXPECT_EQUAL(OCache.getChannel(5), 5.0);

  return success;
}

// =========================
// = Main UnitTests Runner =
// =========================
bool do_unittests() {
  if (!DO_UNITTESTS) return true;

  bool success = true;
  printf("Execute Unit Tests on FPGA Modules...\n");

  success &= test_MemoryController();
  success &= test_WeightsCache();
  success &= test_ImageCache();
  success &= test_OutputCache();

  return success;
}
#else
bool do_unittests() { return true; }
#endif
