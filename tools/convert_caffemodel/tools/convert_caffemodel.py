#!/usr/bin/env python2.7

#######
## convert_caffemodel.py
## (c) 2016 David Gschwend
##
## Usage: python2.7 convert_caffemodel.py [<model.prototxt> <snapshot.caffemodel>]
#######

import os
import sys
import struct
import caffe
import time
import numpy as np
import random

## Get Input Files
## Auto-Detect .caffemodel and .prototxt files, if only one of each is present
prototxt = "undef"
caffemodels = "undef"
if len(sys.argv) == 3:
    ## Files given via CLI argument
    if (os.path.isfile(sys.argv[1])): prototxt = sys.argv[1]
    if (os.path.isfile(sys.argv[2])): caffemodel = sys.argv[2]
elif len(sys.argv) == 1:
    ## No CLI Arguments, try to auto-detect files
    prototxts = [file for file in os.listdir(".") if file.endswith(".prototxt")]
    caffemodels = [file for file in os.listdir(".") if file.endswith(".caffemodel")]
    if len(prototxts) == 1 and len(caffemodels) == 1:
        prototxt = prototxts[0]
        caffemodel = caffemodels[0]
    else:
        print("Error: Could not auto-detect .prototxt and .caffemodel files.")

## Usage Help if Input Files not Found
if prototxt == "undef" or caffemodel == "undef" :
    print("Usage: %s [<model.prototxt> <snapshot.caffemodel>]" % sys.argv[0])
    exit(-1)
print("Using prototxt: {}, caffemodel: {}".format(prototxt, caffemodel))

# Functions to Reshape and Save given Weight/Bias Blob
def append_filters(weights, blob):
    ch_out = blob.shape[0]
    ch_in = blob.shape[1]
    kernel = blob.shape[2]
    for ci in range(ch_in):
        for co in range(ch_out):
            for ky in range(kernel):
                for kx in range(kernel):
                    weights.append(blob.data[co][ci][kx][ky]) ### BEWARE: X, Y MIGHT BE SWITCHED!
def append_bias(weights, blob):
    ch_out = blob.shape[0]
    for co in range(ch_out):
        weights.append(blob.data[co])

######
###### Extract C++ Layer Description + Weights
######

start_time = time.clock()

# Load Network from prototxt / caffemodel
caffe.set_mode_cpu() # use CPU for more compatibility
net = caffe.Net(prototxt, caffemodel, caffe.TEST);

# Initialize Helper Variables
weights = []
layer_count = 0
weights_count = 0
weights_cache_needed = 0
image_cache_needed = 0
max_num_chout = 0
max_active_area = 0
max_image_cache = 0
max_dimension = 0
max_channels = 0
total_inputs = 0    # total num. elements read from / written to memory
total_outputs = 0
total_dram_IO = 0   # total num. DRAM accesses (input + output) (data + weights)

# manually initialize width/height for first layer
width_out  = net.blobs['data'].width
height_out = net.blobs['data'].height

# Push all Layer Names into List:
layer_names = list(net._layer_names)
layers = []

#import pdb; pdb.set_trace()

for id,layer_name in enumerate(layer_names):
    # doesn't include input "data": not a layer
    
    layer  = net.layers[id]
    
    if ((layer.type == "Convolution") or (layer.type == "Pooling") or (layer.type == "ReLU")):
        
        try:
            params = net.params[layer_name]
        except:
            #print("Layer %d: %s doesn't have any params.\n" % (id,layer_name))
            pass
        try:
            blob   = net.blobs[layer_name]
        except:
            #print("Layer %d: %s doesn't have any blobs.\n" % (id,layer_name))
            pass
        
        # Trim down Name
        name = layer_name.replace("fire", "f")
        name = name.replace("expand1x1", "e1")
        name = name.replace("expand3x3", "e3")
        name = name.replace("squeeze1x1","s1")
        name = name.replace("squeeze3x3","s3")
        name = name.replace("conv","c")      # conv10 -> c10
        name = name.replace("split1","p1")   # c10/split1 -> c10p1
        name = name.replace("split2","p2")   # c10/split1 -> c10p1
        
        # Extract Layer Attributes
        width_in  = width_out
        height_in = height_out
        width_out = blob.width
        height_out = blob.height
        ch_out = blob.channels
        
        # defaults:
        ch_in = 0
        kernel = 0
        stride = 0
        pad = 0
        relu = 0
        is_first_split_layer = 0
        is_second_split_layer = 0
        global_pool = 0
        layer_type_string = "LAYER_DATA"
        
        # CONV Layer: Extract Specifications, Add to Table
        if (layer.type == "Convolution"):
            
            # Extract CONV Layer Attributes
            kernel = params[0].shape[3]
            ch_in = params[0].shape[1]
            
            # Hack: derive stride from input-to-output relation
            stride = width_in/width_out;
            
            # Hack: derive padding from input-to-output relation
            if (width_in == stride*width_out): pad = (kernel-1)/2
            else: pad = 0
            
            # For "expand" and "split" layers, need to concat the output 
            #    channels. Recognize split layers by their name:
            #    "expand1x1/expand3x3" and "split1/split2".
            if ("expand1x1" in layer_name) or ("split1" in layer_name):
                is_first_split_layer = 1
            if ("expand3x3" in layer_name) or ("split2" in layer_name):
                is_second_split_layer = 1
            
            # Add Layer to Layer Stack
            layers.append({
                'id': id,
                'name': name,
                'type': "LAYER_CONV",
                'width': width_in,
                'height': height_in,
                'ch_in': ch_in,
                'ch_out': ch_out,
                'kernel': kernel,
                'pad': pad,
                'stride': stride,
                'relu': relu,
                'is_first_split_layer': is_first_split_layer,
                'is_second_split_layer': is_second_split_layer,
                'global_pool': 0
            })
            
            # Count Number of Weights:
            weights_count += ch_in*ch_out*kernel*kernel + ch_out
            
            # Append Weights and Biases to List
            append_filters(weights, params[0])
            append_bias(weights, params[1])
            
            # Count Total Number of Layer:
            layer_count = layer_count + 1
            
            # Update Maximum necessary Cache sizes:
            weights_size = ch_in*ch_out*kernel*kernel + ch_out
            #print "layer",name,"weights_size",weights_size
            input_size = ch_in*width_in*height_in
            output_size = ch_out*width_out*height_out
            pixels_per_row = ch_in*width_in;
            image_cache_size = 4*pixels_per_row;
            max_image_cache = max(max_image_cache, input_size)
            weights_cache_needed = max(weights_cache_needed, weights_size)
            image_cache_needed = max(image_cache_size, image_cache_needed)
            max_num_chout = max(ch_out, max_num_chout)
            max_active_area = max(ch_in*kernel*kernel, max_active_area)
            max_dimension = max(max_dimension, max(width_in, height_in))
            max_channels = max(max_channels, max(ch_in, ch_out))
            total_inputs = total_inputs + ch_in*width_in*height_in
            total_outputs = total_outputs + ch_out*width_out*height_out
            total_dram_IO = total_dram_IO + input_size + output_size + weights_size
        
        # POOLING LAYERS:
        # Modify Previous CONV Layer to add Pooling
        if (layer.type == "Pooling"):
            ch_in = blob.channels
            
            # Get Pooling Parameters
            kernel = params[0].shape[3]
            stride = width_in/width_out
            pool_type = "POOL_UNKNOWN"
            
            # Calculate Pooling Type
            # Actually only "POOL_GLOBAL" is currently supported on FPGA
            if (kernel == 3 and stride == 2):
                pool_type = "POOL_3x3S2"
                raise ValueError("FOUND UNSUPPORTED POOL_3x3S2")
            elif (kernel == 2 and stride == 2):
                pool_type = "POOL_2x2S2"
                raise ValueError("FOUND UNSUPPORTED POOL_2x2S2")
            elif (kernel == 1 and stride == width_in):
                pool_type = "POOL_GLOBAL"
            
            # Modify last CONV layer(s) to add Pooling...
            last_conv_layers = [layers.pop()]
            
            # If last layer = "split" layer, fetch it's twin as well...
            # Recognize Split layers by name:
            ll = last_conv_layers[0]['name']
            if ("e1" in ll) or ("e3" in ll) or ("p1" in ll) or ("p2" in ll):
                last_conv_layers.append(layers.pop())
            
            # Set layer pooling parameter
            for convlayer in reversed(last_conv_layers):
                if pool_type == "POOL_GLOBAL":
                    convlayer['global_pool'] = 1
                layers.append(convlayer)
                
        # ReLU LAYERS:
        # Modify Previous CONV Layer to add ReLU
        if (layer.type == "ReLU"):
            
            # Modify last CONV layer(s) to add ReLU...
            last_conv_layer = layers.pop()
    
            # Set layer ReLU parameter
            last_conv_layer['relu'] = 1
            
            # Re-Add to List
            layers.append(last_conv_layer)

# Generate Network Description:
net = ""
for layer in layers:
    if not "CONV" in layer["type"]: continue
    t  = "  addLayer(net, layer_t("
    t += "\"{name:<6s}\", {width:3d}, {height:3d}, {ch_in:4d},"
    t += "{ch_out:4d},{kernel:2d},{pad:2d},{stride:2d},{relu:2d},"
    t += "{is_first_split_layer:3d},{is_second_split_layer:3d},{global_pool:3d}"
    t += "));\n"
    net += t.format(**layer)

# Add Header and Footer
header = """
///////
// Network + Layer Configuration.
// File automatically generated by convert_caffemodel.py
// from {0} and {1}
// (c) 2016 David Gschwend
///////
""".format(prototxt, caffemodel)
hfile = """{}
#ifndef _NETWORK_H_
#define _NETWORK_H_

// Size Limits for this Network
const int MAX_NUM_LAYERS = {};
const int MAX_WEIGHTS_PER_LAYER = {};
const int MAX_IMAGE_CACHE_SIZE = {};
const int MAX_INPUT_PER_LAYER = {};
const int MAX_NUM_CHOUT = {};
const int MAX_DIMENSION = {};
const int MAX_CHANNELS = {};

const int TOTAL_NUM_WEIGHTS = {};
const int TOTAL_NUM_INPUTS = {};
const int TOTAL_NUM_OUTPUTS = {};
const int TOTAL_DRAM_IO = {};

const int DRAM_DEPTH = 0;
const float TEST_RESULT_EXPECTED = 0.00;

// Mean Pixel for ImageNet Data
const float MEAN_R = 104;
const float MEAN_G = 117;
const float MEAN_B = 123;

// include after definitions to allow bit-width calculations
#include "netconfig.hpp"

network_t *get_network_config();

#endif
""".format(header, layer_count, weights_cache_needed,image_cache_needed,
           max_image_cache, max_num_chout, max_dimension, max_channels,
           weights_count, total_inputs, total_outputs, total_dram_IO )

# double braces in function definition needed because of python's str.format()!
cfile = """{0}
#include "network.hpp"
network_t *get_network_config() {{
  network_t *net = new network_t({1:d}, {2:d});\n
  // Layer Attributes: ( NAME   ,   W,   H,   CI,  CO, K, P, S, R, S1, S2, GP)
{3}
  net->num_weights = {4};
  const char* filename = "weights.bin";
  loadWeightsFromFile(net, filename);
  return net;
}}
""".format(header, layer_count, weights_count, net, weights_count)

# Write Network Constructor to <network.h> and <network.c>
with open("network.hpp", "w") as f:
    f.write(hfile);
with open("network.cpp", "w") as f:
    f.write(cfile);

# Write weights to binary file
floatstruct = struct.pack('f'*len(weights), *weights)
with open("weights.bin", "wb") as f:
    f.write(floatstruct)

end_time = time.clock()
time_taken = end_time - start_time

# Print Network Constructor to stdout
print(hfile)
print(cfile)

# Print Confirmation to stdout
print("REPORT:\n-----------")
print("%d-layer Network saved to <network.{hpp,cpp}>" % layer_count)
print("%d Weights saved to <weights.bin>" % len(weights))

print("Total Operations took %d seconds on CPU." % time_taken)

# ### 
