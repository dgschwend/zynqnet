#!/bin/bash
#set -x 

#####
##
## Model Conversion Run Script
##
## To convert a model:
##   mkdir my_model
##   cp model_download_from_digits/deploy.prototxt my_model
##   cp model_download_from_digits/snap*.caffemodel my_model
##   cp run.sh my_model
##   cd my_model && ./run.sh
##   nano network.hpp -> adjust TEST_RESULT_EXPECTED
##
####

echo "Convert Caffe Model to network.hpp/cpp and weights.bin:"
echo ""

# Convert Caffe Model, create network.hpp/cpp and all intermediate layer results
../tools/convert_caffemodel.py deploy.prototxt snap*.caffemodel

echo ""
echo "Generate indata.bin and Expected Response(s) for Golden Model:"
echo ""

# Run Network with Caffe on "puppy" image to get Expected Top-1 Probability
../tools/classify.py snap*.caffemodel deploy.prototxt ../tools/puppy-500x350.jpg 2>&1| tee expected_result.txt
# Copy Input Data Binary to model folder
cp blobs/result_data.bin indata.bin

echo ""
echo "Please adjust TEST_RESULT_EXPECTED in network.hpp."
echo "Conversion done. Use network.hpp/cpp, indata.bin, weights.bin for FPGA."
echo ""
