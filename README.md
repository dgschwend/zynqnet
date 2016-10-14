# ZynqNet: An FPGA-Accelerated Embedded Convolutional Neural Network

This repository contains the results from my Master Thesis. The project has been enabled and supported by [Supercomputing Systems AG](http://www.scs.ch).

## Abstract

Image Understanding is becoming a vital feature in ever more applications ranging from
medical diagnostics to autonomous vehicles. Many applications demand for embedded
solutions that integrate into existing systems with tight real-time and power constraints.

Convolutional Neural Networks (CNNs) presently achieve record-breaking accuracies in
all image understanding benchmarks, but have a very high computational complexity.
Embedded CNNs thus call for small and efficient, yet very powerful computing platforms.
This master thesis explores the potential of FPGA-based CNN acceleration and demonstrates
a fully functional proof-of-concept CNN implementation on a Zynq System-on-Chip. The
_ZynqNet Embedded CNN_ is designed for image classification on ImageNet and consists of
_ZynqNet CNN_, an optimized and customized CNN topology, and the _ZynqNet FPGA Accelerator_,
an FPGA-based architecture for its evaluation.

_ZynqNet CNN_ is a highly efficient CNN topology. Detailed analysis and optimization of
prior topologies using the custom-designed _Netscope CNN Analyzer_ have enabled a CNN
with 84.5% top-5 accuracy at a computational complexity of only 530 million multiplyaccumulate
operations. The topology is highly regular and consists exclusively of convolutional
layers, ReLU nonlinearities and one global pooling layer. The CNN fits ideally onto the
FPGA accelerator.

The _ZynqNet FPGA Accelerator_ allows an efficient evaluation of ZynqNet CNN. It accelerates
the full network based on a nested-loop algorithm which minimizes the number of arithmetic
operations and memory accesses. The FPGA accelerator has been synthesized using High-
Level Synthesis for the Xilinx Zynq XC-7Z045, and reaches a clock frequency of 200MHz
with a device utilization of 80% to 90 %.

## Contribution
Initially, this master aimed to explore, benchmark and optimize one or more commercial
approaches to the acceleration of convolutional neural networks on FPGAs, with a focus
on embedded systems. Multiple FPGA and intellectual property vendors have announced
frameworks and libraries that target the acceleration of deep learning systems.However,
none of these solutions turned out to be ready and available for testing.

Nevertheless, we decided to further pursue this promising approach by building our own
proof-of-concept FPGA-based CNN implementation from scratch, with a special focus on the
optimized co-operation between the underlying hardware architecture and the convolutional
neural network. The result is the ZynqNet Embedded CNN, an FPGA-based convolutional
neural network for image classification. The solution consists of two main components:

1. The _ZynqNet CNN_, a customized convolutional neural network topology, specifically shaped to fit ideally onto the FPGA. The CNN is exceptionally regular, and reaches a satisfying classification accuracy with minimal computational effort.
2. The _ZynqNet FPGA Accelerator_, a specialized FPGA architecture for the efficient acceleration of ZynqNet CNN and similar convolutional neural networks.

ZynqNet CNN is trained offline on GPUs using the Caffe framework, while the ZynqNet FPGA
Accelerator employs the CNN for image classification, or _inference_, on a Xilinx Zynq XC-
7Z045 System-on-Chip (SoC). Both components have been developed and optimized within
the six month time frame of this master thesis, and together constitute a fully functional
convolutional neural network implementation on the small and low-power Zynq platform.

This report documents the ZynqNet CNN and the ZynqNet FPGA Accelerator and gives
insight into their development. In addition, the _Netscope CNN Analyzer_ is introduced, a
custom tool for visualizing, analyzing and editing convolutional neural network topologies.
Netscope has been used to analyze a number of different CNN architectures, and the findings
are presented in the form of a _Design Space Exploration_ (DSE) of CNN topologies from
prior work. Finally, the performance of the ZynqNet Embedded CNN is evaluated and its
performance is compared to other platforms.

## Report
The report includes
- overview + detailed analysis of many popular CNN architectures for image classification (AlexNet, VGG, NiN, GoogLeNet, Inception v.X, ResNet, SqueezeNet)
- detailed description of the [*Netscope CNN Analyzer* tool]([https://github.com/dgschwend/netscope)
- overview of *CNN analysis and optimization techniques*
- detailed report on the design and implementation of the FPGA-based accelerator

The final report can be found in [zynqnet_report.pdf](https://github.com/dgschwend/zynqnet/tree/master/zynqnet_report.pdf).

## ZynqNet CNN
The fully trained CNN with .prototxt network description and pretrained weights can be found under "_TRAINED_MODEL"

## ZynqNet FPGA Accelerator
The C/C++ source code for building the FPGA accelerator using High-Level Synthesis (Vivado HLS) can be found under [_HLS_CODE](https://github.com/dgschwend/zynqnet/tree/master/_HLS_CODE).

The compiled accelerator bitstream can be found under [_BITSTREAM](https://github.com/dgschwend/zynqnet/tree/master/_BITSTREAM).

The firmware for the Zynq XC-7Z045 ARM processors is stored under [_FIRMWARE](https://github.com/dgschwend/zynqnet/tree/master/_FIRMWARE).

## Netscope CNN Analyzer
The CNN analysis tool can be found in a separate repository here: [dgschwend/netscope](https://github.com/dgschwend/netscope)

## Copyright and License
ZynqNet is Copyright 2016 by David Gschwend.
All files in this repository are released under the GNU General Public License as found in the LICENSE file.
