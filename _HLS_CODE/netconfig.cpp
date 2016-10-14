//------------------------------------------------------------------------------
//  SqueezeNetOnFPGA
//------------------------------------------------------------------------------
//
//	File:  netconfig.cpp
//
//  addLayer(), loadWeightsFromFile(), print_layer(), print_layers()
//
//	(c) David Gschwend, 2016
//
//------------------------------------------------------------------------------

#include "network.hpp"
#include "netconfig.hpp"

// ==============================
// = Add Layer to given Network =
// ==============================

void addLayer(network_t *net, layer_t layer) {
  // Assumes that Network has been initialized with enough memory (not checked!)
  // Uses static variables -> can only be used for 1 network definition per
  // program!
  // If layer.is_first_split_layer==true, reserves double amount of
  //     output memory to allow implicit output-channel concatenation.
  //     Use for (expand1x1) and other "output-channel split" layers.
  // If layer.is_second_split_layer==true, uses same input memory address as in
  //     last layer, and moves output address only by layer.channels_out/2
  //     to interleave with previous layer's output.
  //     Use for (expand3x3) and other "output-channel split" layers.

  // Keep Track of Memory Locations for Activations + Weights (-> static)
  static int current_output_addr = 0;
  static int current_input_addr = 0;
  static int current_weights_addr = 0;

  // Align to memory borders (float needed because of ceil() operation below)
  float mem_border = MEMORY_ALIGNMENT / sizeof(data_t);

  // Data Size Calculations
  int input_data_pixels = layer.width * layer.height * layer.channels_in;
  int width_out =
      1 + std::floor((float)(layer.width + 2 * layer.pad - layer.kernel) /
                     layer.stride);
  int height_out =
      1 + std::floor((float)(layer.height + 2 * layer.pad - layer.kernel) /
                     layer.stride);
  int output_data_pixels = width_out * height_out * layer.channels_out;
  int num_weights =  // conv + bias weights
      layer.channels_out * layer.channels_in * layer.kernel * layer.kernel +
      layer.channels_out;

  if (layer.is_second_split_layer == false) {
    // For "normal" layers, and "1st split" layers:
    // Update Input + Output Memory Address
    // - read input from previous layer's output address
    // - write output to next free memory position

    // this layer's input memory = last layer's output memory
    current_input_addr = current_output_addr;
    layer.mem_addr_input = current_input_addr;

    // this layer's output comes right next, aligned to memory border
    current_output_addr += input_data_pixels;
    current_output_addr = ceil(current_output_addr / mem_border) * mem_border;
    layer.mem_addr_output = current_output_addr;

  } else {
    // For "2nd split" layer (expand3x3, conv10/split2):
    // - read from same input location
    // - interleave output channels (write to slightly offset memory address)

    // current_output_addr + current_input_addr is still as in last layer...
    layer.mem_addr_output = current_output_addr + layer.channels_out;
    layer.mem_addr_input = current_input_addr;
  }

  /*printf("layer %s, update_memory_address = %d, current_input_addr = %d, "
   "current_output_addr = %d", layer.name, update_memory_address,
   current_input_addr, current_output_addr);*/

  // Store + Update Weights Memory Address
  // (don't align, weights are read sequentially)
  layer.mem_addr_weights = current_weights_addr;
  current_weights_addr += num_weights;

  // Write Options into Layer Config
  /* layer.is_expand_layer = is_expand_layer;
   layer.pool = pool_type;*/

  // Add Layer to network
  net->layers[net->num_layers] = layer;
  net->num_layers++;
  net->total_pixel_mem = current_output_addr + output_data_pixels;
};

// =================================
// = Load Weights from Binary File =
// =================================
// prepare with convert_caffemodel.py

void loadWeightsFromFile(network_t *net, const char *filename) {
  FILE *filehandle = fopen(filename, "rb");
  if (!filehandle) {
    printf("ERROR: File %s could not be opened!\n", filename);
    exit(-1);
  }

  for (int i = 0; i < net->num_layers; i++) {
    layer_t *layer = &net->layers[i];
    int chout = layer->channels_out;
    int chin = layer->channels_in;
    int kernel = layer->kernel;

    // calculate address within weight memory section
    int num_weights = chout * chin * kernel * kernel + chout;
    float *weights_addr = (net->weights + layer->mem_addr_weights);

    // read portion of input file
    fread(weights_addr, sizeof(data_t), num_weights, filehandle);
  }

  fclose(filehandle);
}

// =========================================
// = Convert layer_t struct to float array =
// =========================================
/*void layer_to_floats(layer_t &layer, float floats[NUM_FLOATS_PER_LAYER]) {
  // struct layer_t fields:
  // ----------------------
  // char name[NET_NAME_MAX_LEN + 1];
  // layertype_t type;
  // dimension_t width;  // input dimensions
  // dimension_t height;
  // channel_t channels_in;
  // channel_t channels_out;
  // kernel_t kernel;  // kernel sizes supported: 3 or 1
  // bool pad;         // padding is either 0 or 1 pixel
  // stride_t stride;  // only stride 1 or 2 supported
  // memaddr_t mem_addr_input;
  // memaddr_t mem_addr_output;
  // memaddr_t mem_addr_weights;
  // bool is_expand_layer;
  // pooltype_t pool;

  // clang-format off
  union { float f; unsigned int i; } u;
  u.i = layer.width;        floats[0] = u.f;
  u.i = layer.height;       floats[1] = u.f;
  u.i = layer.channels_in;  floats[2] = u.f;
  u.i = layer.channels_out; floats[3] = u.f;
  u.i = layer.kernel;       floats[4] = u.f;
  u.i = layer.stride;       floats[5] = u.f;
  u.i = layer.pad;          floats[6] = u.f;
  u.i = layer.mem_addr_input;   floats[7] = u.f;
  u.i = layer.mem_addr_output;  floats[8] = u.f;
  u.i = layer.mem_addr_weights; floats[9] = u.f;
  u.i = layer.is_expand_layer;  floats[10] = u.f;
  u.i = (layer.pool == POOL_GLOBAL ? 1 : 0);  floats[11] = u.f;
  // clang-format on
}*/

// =========================================
// = Print Overview Table of given Network =
// =========================================
// Print List of all Layers + Attributes + Memory Locations
#define use_KB 0
#if use_KB
#define unit "k"
#define divi 1024
#else
#define unit ""
#define divi 1
#endif

void print_layer(layer_t *layer) {
  int memory_needed = layer->height * layer->width * layer->channels_in;
  int weights_size =
      layer->kernel * layer->kernel * layer->channels_in * layer->channels_out +
      layer->channels_out;

  printf("%6s: %3dx%-3d x %3d > %-3d, ", layer->name, (int)layer->height,
         (int)layer->width, (int)layer->channels_in, (int)layer->channels_out);

  printf("CONV (%dx%d)/%d%s %s, ", (int)layer->kernel, (int)layer->kernel,
         (int)layer->stride, layer->pad ? "p" : " ",
         layer->relu ? "+ ReLU" : "      ");

  printf("IN @mem(%8lu-%8lu" unit "B), ",
         long(layer->mem_addr_input * sizeof(float) / divi),
         long((layer->mem_addr_input + memory_needed) * sizeof(float) / divi));

  printf("OUT @mem(%8lu" unit "B), ",
         long(layer->mem_addr_output * sizeof(float) / divi));

  printf("WEIGHTS @mem(%8lu-%8lu" unit "B)",
         long(layer->mem_addr_weights * sizeof(float) / divi),
         long((layer->mem_addr_weights + weights_size) * sizeof(float) / divi));

  if (layer->is_second_split_layer | layer->is_first_split_layer)
    printf(" (split%d)", (layer->is_first_split_layer) ? 1 : 2);

  if (layer->global_pool == true) printf(" GLOBAL POOL");

  printf("\n");
};

void print_layers(network_t *net) {
  for (int i = 0; i < net->num_layers; i++) {
    layer_t *layer = &net->layers[i];
    print_layer(layer);
  }
}
