//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  cpu_top.cpp
//
//  CPU-Side Functions for SqueezeNetOnFPGA
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "cpu_top.hpp"

// ======================================
// = Global Variables (Memory Pointers) =
// ======================================
// Pointers to Shared DRAM Memory
char *SHARED_DRAM;
// layer_t *SHARED_DRAM_LAYER_CONFIG;
float *SHARED_DRAM_LAYER_CONFIG;
data_t *SHARED_DRAM_WEIGHTS;
data_t *SHARED_DRAM_DATA;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// =================
// = Main Function =
// =================

int main() {
  LOG_LEVEL = 0;

  // ==============
  // = Unit Tests =
  // ==============
  if (!do_unittests()) {
    printf("UNIT TESTS FAILED, ABORTING.");
    return -1;
  };

  // =================
  // = Setup Network =
  // =================
  // Generate + Load Network Config from network.hpp/network.cpp
  network_t *net_CPU;
  net_CPU = get_network_config();

  // Assert that layer_t fits into a multiple of bus transactions:
  // ONLY NECESSARY IF WE CAN MAP LAYER_T TRANSFER ONTO BUS_T AXI MASTER
  // printf("size of layer_t: %d, size of bus_t: %d", (int)sizeof(layer_t),
  //       (int)sizeof(bus_t));
  // assert((sizeof(layer_t) % sizeof(bus_t) == 0) &&
  //       "layert_t is not multiple of bus size. adjust size of
  //       layer_t.dummy!");

  // ==========================
  // = Setup FPGA Accelerator =
  // ==========================
  // Allocate Shared Memory for Config, Weights, Data.
  // Copy Layer Config + Weights to FPGA.
  setup_FPGA(net_CPU);

  // ===========================
  // = Load + Copy Input Image =
  // ===========================
  /* Structured: generate_structured_input_image(input_image,win,hin,chin);
   PseudoRandom: generate_random_input_image(input_image, win, hin, chin, 1);
   ReallyRandom: generate_random_input_image(input_image, win, hin, chin -1);
   Prepared Input File (convert_image.py):
   load_prepared_input_image(input_image, "./indata.bin", win, hin, chin);
   JPG/PNG Input File (!not implemented!):
   load_image_file(input_image, "./puppy-500x350.jpg", win, hin, chin);
   do_preprocess(input_image, win, hin, chin); */

  // Allocate Memory on CPU Side:
  layer_t layer0 = net_CPU->layers[0];
  int win = layer0.width;
  int hin = layer0.height;
  int chin = layer0.channels_in;
  data_t *input_image = (data_t *)malloc(win * hin * chin * sizeof(data_t));

  // Load Input Image
  load_prepared_input_image(input_image, "./indata.bin", win, hin, chin);

  // Copy onto FPGA
  copy_input_image_to_FPGA(net_CPU, input_image);

// ============================
// = Execute FPGA Accelerator =
// ============================

L_LAYERS:
  for (int layer_id = 0; layer_id < net_CPU->num_layers; layer_id++) {
    layer_t layer = net_CPU->layers[layer_id];
    LOG("Layer %2d: <%s>\n", layer_id, layer.name);
    LOG_LEVEL_INCR;

    // Calculate Memory Pointers
    LOG("SHARED_DRAM is at address: %lu\n", (long)SHARED_DRAM);
    int weights_offset =
        ((long)SHARED_DRAM_WEIGHTS - (long)SHARED_DRAM) / sizeof(data_t);
    int input_offset =
        ((long)SHARED_DRAM_DATA - (long)SHARED_DRAM) / sizeof(data_t);
    numfilterelems_t weights_per_filter = (layer.kernel == 3) ? 9 : 1;
    weightaddr_t num_weights =
        layer.channels_in * layer.channels_out * weights_per_filter;

    // Print some Info on this Layer
    printf("CPU: Offload CONV Layer ");
    print_layer(&layer);
    fflush(stdout);

    // Offload Layer Calculation to FPGA
    fpga_top(layer, (data_t *)SHARED_DRAM, weights_offset, num_weights,
             input_offset);

    LOG_LEVEL_DECR;
  }

  LOG_LEVEL = 0;

  // ===============================
  // = Copy Results back from FPGA =
  // ===============================
  layer_t *final = &net_CPU->layers[net_CPU->num_layers - 1];
  int ch_out = (final->is_second_split_layer ? 2 : 1) * final->channels_out;
  
  data_t *results = (data_t *)malloc(ch_out * sizeof(data_t));
  copy_results_from_FPGA(net_CPU, results, ch_out);

  // =====================
  // = Calculate Softmax =
  // =====================
  std::vector<std::pair<data_t, int> > probabilities(ch_out);
  calculate_softmax(net_CPU, results, probabilities);

  // ==================
  // = Report Results =
  // ==================
  printf("\nResult (top-5):\n====================\n");
  for (int i = 0; i < std::min(5, ch_out); i++) {
    printf("    %5.2f%%: class %3d (output %6.2f)\n",
           100 * probabilities[i].first, probabilities[i].second,
           results[probabilities[i].second]);
  }

  // ====================
  // = TestBench Result =
  // ====================
  // Check if output is AS EXPECTED (+- 0.5%) (defined in network.hpp)
  if (fabs(100 * probabilities[0].first - TEST_RESULT_EXPECTED) < 0.1) {
    printf("\nTestBench Result: SUCCESS\n");
    return 0;
  } else {
    printf("\nTestBench Result: FAILURE\n");
    printf("Actual: %5.2f, Expected: %5.2f\n", 100 * probabilities[0].first,
           TEST_RESULT_EXPECTED);
    return -1;
  }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// ==============================================
// = Allocate Memory Regions for Data + Weights =
// ==============================================
// Reserve Memory and Assign Pointers
// DRAM:
//      ______________
//     |    weights   |  0
//     |              |  ...
//     |______________|  weightsize - 1
//     |     data     |  weightsize
//     |  in + output |  ...
//     |______________|  weightsize + datasize - 1
//
void allocate_FPGA_memory(network_t *net_CPU) {
  // For Simulation purposes, allocate space on Heap
  // For actual HW Implementation, set fixed memory addresses in Shared DRAM

  // Memory Requirements (Bytes)
  // int configsize = net_CPU->num_layers * (sizeof(layer_t));
  // int configsize = net_CPU->num_layers * (NUM_FLOATS_PER_LAYER *
  // sizeof(float));
  int weightsize = net_CPU->num_weights * sizeof(data_t);
  int datasize = net_CPU->total_pixel_mem * sizeof(data_t);

  // Round memory areas to 32-bit boundaries (4 bytes)
  // configsize = std::ceil(configsize / 4.0) * 4;
  weightsize = std::ceil(weightsize / 4.0) * 4;
  datasize = std::ceil(datasize / 4.0) * 4;

  int total_size = /*configsize + */ weightsize + datasize;

  // Memory Allocation
  SHARED_DRAM = (char *)malloc(total_size);

  // Pointer Redirection
  SHARED_DRAM_LAYER_CONFIG = (float *)(SHARED_DRAM + 0);
  SHARED_DRAM_WEIGHTS = (data_t *)(SHARED_DRAM);
  SHARED_DRAM_DATA = (data_t *)(SHARED_DRAM + weightsize);

  // Debug: Infos about Memory Regions
  printf("CPU: FPGA DRAM Memory Allocation:\n");
  printf("     Bytes allocated: %dB (config) + %dKB (weights) + %dKB (data)\n",
         0, weightsize / 1024, datasize / 1024);
  printf("     region: %lu – %lu\n", (long)SHARED_DRAM,
         (long)(SHARED_DRAM + total_size));

  if (DRAM_DEPTH != total_size / sizeof(data_t)) {
    printf("\n\n!! ERROR !!\n");
    printf("No big deal, but please set DRAM_DEPTH = %d in network.hpp\n\n",
           (int)(total_size / sizeof(data_t)));
    exit(-1);
  }
}

// =====================================================
// = Copy Layer Config + Weights to FPGA (shared DRAM) =
// =====================================================
void copy_config_to_FPGA(network_t *net_CPU) {
  // int configsize = net_CPU->num_layers * sizeof(layer_t);
  // int configsize = net_CPU->num_layers * (NUM_FLOATS_PER_LAYER *
  // sizeof(float));
  int weightsize = net_CPU->num_weights * sizeof(data_t);

  // Info:
  printf("CPU: Copy Config + Weights to FPGA DRAM:\n");
  printf("     %dB (config) + %dKB (weights)\n", 0, weightsize / 1024);

  // Copy Layer Config:
  // memcpy(SHARED_DRAM_LAYER_CONFIG, net_CPU->layers, configsize);
  /*for (int l = 0; l < net_CPU->num_layers; l++) {
   layer_to_floats(net_CPU->layers[l],
   &SHARED_DRAM_LAYER_CONFIG[l * NUM_FLOATS_PER_LAYER]);
   }*/

  // Copy Weights:
  memcpy(SHARED_DRAM_WEIGHTS, net_CPU->weights, weightsize);
}

// ======================================
// = Load Input Data from prepared File =
// ======================================
// Loads input_image with data from given file
// (prepared input file using convert_image.py)
void load_prepared_input_image(data_t *input_image, const char *filename,
                               int win, int hin, int chin) {
  // calculate size of input data
  int num_pixels = win * hin * chin;
  printf("CPU: Loading Input from File %s, %lu kBytes.\n", filename,
         num_pixels * sizeof(data_t) / 1024);

  // load binary data from file
  FILE *infile = fopen(filename, "rb");
  if (!infile) {
    printf("ERROR: File %s could not be opened!\n", filename);
    exit(-1);
  }
  fread(input_image, sizeof(data_t), num_pixels, infile);
  fclose(infile);
}

// ==========================================
// = Copy Input Image to FPGA (shared DRAM) =
// ==========================================
void copy_input_image_to_FPGA(network_t *net_CPU, data_t *image) {
  // Input Data goes into Layer 0:
  int win = net_CPU->layers[0].width;
  int hin = net_CPU->layers[0].height;
  int chin = net_CPU->layers[0].channels_in;
  int input_img_size = win * hin * chin * sizeof(data_t);

  // Info:
  printf("CPU: Copy Input Data: %dKB (input image)\n", input_img_size / 1024);

  // Copy Input Data:
  memcpy(SHARED_DRAM_DATA, image, input_img_size);
}

// =============================================
// = Copy Results back from FPGA (shared DRAM) =
// =============================================
// Assumption: Last Layer reduces data to dimensions 1x1xch_out (global pool)
// Assumption: Output Data is written back to where initial image was placed
// data_t *results: Pointer to data_t array with enough space to hold results
void copy_results_from_FPGA(network_t *net_CPU, data_t *results, int ch_out) {
  // Verify that last layer reduces spatial dimensions to 1x1:
  assert(net_CPU->layers[net_CPU->num_layers - 1].global_pool == true);

  // Assumption: Output Data is at beginning of DATA section (shared DRAM)
  int result_offset = 0;
  int result_size = ch_out * sizeof(data_t);

  printf("CPU: Copy Results from FPGA DRAM: %d Bytes\n", result_size);

  // Copy Result Data:
  memcpy(results, SHARED_DRAM_DATA + result_offset, result_size);
}

// ===========================================
// = Calculate Softmax from Raw FPGA Results =
// ===========================================
void calculate_softmax(network_t *net_CPU, data_t *results,
                       std::vector<std::pair<data_t, int> > &probabilities) {
  // First, finish Global AVG Pooling (FPGA does just accumulation, no division)
  // Then, subtract maximum to avoid Numerical Issues in Exponentiation
  // (as done by CAFFE in caffe/include/caffe/layers/softmax_layer.hpp)
  // Then, calculate actual Softmax [ p_i = e^{r_i} / (\sum(e^{r_i})) ]

  // Divide by WxH of Output Maps:
  layer_t *final = &net_CPU->layers[net_CPU->num_layers - 1];
  data_t num_output_pixels = final->width * final->height;
  if (final->stride == 2) num_output_pixels /= 4;
  int ch_out = final->channels_out;
  if (final->is_second_split_layer) ch_out *= 2;

  data_t maxresult = 0;
  for (int i = 0; i < ch_out; i++) {
    // Average over spatial output dimensions
    results[i] /= num_output_pixels;
    // Find maximum result
    maxresult = std::max(maxresult, results[i]);
    DBG("  %6.2f\n", results[i]);
  }
  DBG("(maximum: %6.2f)\n", maxresult);

  // Calculate Exponentials and Sum
  data_t expsum = 0;
  std::vector<data_t> exponentials(ch_out);
  for (int i = 0; i < ch_out; i++) {
    // Subtract Maximum ("normalize"), then calculate e^()
    exponentials[i] = exp(results[i] - maxresult);
    // Accumulate Sum of Exponentials
    expsum += exponentials[i];
  }
  DBG("sum of exponentials: %f\n", expsum);

  // Calculate Softmax Probabilities [ p_i = e^{r_i} / (\sum(e^{r_i})) ]
  for (int i = 0; i < ch_out; i++) {
    probabilities[i] = std::pair<data_t, int>(exponentials[i] / expsum, i);
    // printf("P(class %3d) = %4.2f%%\n", i, 100 * probabilities[i].first);
  }

  // Sort (small index = high probability)
  std::sort(probabilities.begin(), probabilities.end());
  std::reverse(probabilities.begin(), probabilities.end());
}

// =========================================
// = Debug: Generate Structured Input Data =
// =========================================
// Fills input_image with pixels of format YYYXXX.0CH (for debugging)
void generate_structured_input_image(data_t *input_image, int win, int hin,
                                     int chin) {
  // STRUCTURED INPUT
  for (int x = 0; x < win; x++) {
    for (int y = 0; y < hin; y++) {
      for (int ch = 0; ch < chin; ch++) {
        data_t value = y * 1000.0 + x + ch / 1000.0;
        input_image[y * win * chin + x * chin + ch] = value;
      }
    }
  }
}

// =====================================
// = Debug: Generate Random Input Data =
// =====================================
// Fills input_image with random data_ts between -100 and +100 (for testing)
// Set seed=-1 to shuffle the random generator
void generate_random_input_image(data_t *input_image, int win, int hin,
                                 int chin, int seed = 1) {
  // Expected Results:
  // For seed = 1 [default] and miniFire8UnitFilter, should get result 14154.350
  // for each class.

  // Enable for real randomness: (time() updates every second)
  if (seed == -1) {
    srand(time(NULL));
  } else {
    srand(seed);
  }

  // RANDOM INPUT
  // Generate Input Image (pixels between +-1, 3 places after zero)
  for (int x = 0; x < win; x++) {
    for (int y = 0; y < hin; y++) {
      for (int ch = 0; ch < chin; ch++) {
        //
        data_t value = (rand() % 2000 - 1000) / 2000.0 * 100;
        input_image[y * win * chin + x * chin + ch] = value;
      }
    }
  }
}

// ===========================================
// = TODO: Load Input Data from JPG/PNG File =
// ===========================================
// NOT IMPLEMENTED (YET)
void load_image_file(data_t *input_image, const char *filename, int win,
                     int hin, int chin) {
  // TODO: Implement Image Loading from PNG, JPG
  // (maybe not necessary -> binary data from camera?)
}

// ===================================
// = Preprocess: Subtract Mean Pixel =
// ===================================
// not necessary for prepared input image
void do_preprocess(data_t *input_image, int win, int hin, int chin) {
  for (int y = 0; y < hin; y++) {
    for (int x = 0; x < win; x++) {
      // Subtract Mean Pixel (defined in network.hpp)
      input_image[y * win * chin + x * chin + 0] -= MEAN_R;
      input_image[y * win * chin + x * chin + 1] -= MEAN_G;
      input_image[y * win * chin + x * chin + 2] -= MEAN_B;
    }
  }
}

// =======================================================================
// = Setup Network (Allocate Shared Memory, Copy Layer Config + Weights) =
// =======================================================================
void setup_FPGA(network_t *net_CPU) {
  // Print Network Config
  printf("\n\nCPU: Network Setup:\n=====================\n\n");
  print_layers(net_CPU);
  printf("\n");

  // Setup FPGA DRAM Memory (Config, Weights, Data Sections)
  allocate_FPGA_memory(net_CPU);

  // Copy Network Config (Layer Config, Weights)
  copy_config_to_FPGA(net_CPU);
}
