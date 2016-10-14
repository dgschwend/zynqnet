# ZynqNet: An FPGA-Accelerated Embedded Convolutional Neural Network

This repository contains the results from my Master Thesis.

## Report
The report includes
- overview + detailed analysis of many popular CNN architectures for image classification (AlexNet, VGG, NiN, GoogLeNet, Inception v.X, ResNet, SqueezeNet)
- detailed description of the [*Netscope CNN Analyzer* tool]([https://github.com/dgschwend/netscope)
- overview of CNN analysis and optimization techniques
- detailed report on the design and implementation of the FPGA-based accelerator

The final report can be found in "zynqnet_report.pdf"

## ZynqNet CNN: An efficient, optimized architecture for image classification on ImageNet (ILSVRC)
The fully trained CNN with .prototxt network description and pretrained weights can be found under "_TRAINED_MODEL"

## ZynqNet FPGA Accelerator: High-Level synthesis source code for FPGA accelerator
The C/C++ source code for building the FPGA accelerator using High-Level Synthesis (Vivado HLS) can be found under "_HLS_CODE".

The compiled accelerator bitstream can be found under "_BITSTREAM".

The firmware for the Zynq XC-7Z045 ARM processors is stored under "_FIRMWARE".

## Netscope CNN Analyzer
The CNN analysis tool can be found in a separate repository here: [dgschwend/netscope](https://github.com/dgschwend/netscope)

## Copyright and License
ZynqNet is Copyright 2016 by David Gschwend.
All files in this repository are released under the GNU General Public License as found in the LICENSE file.
