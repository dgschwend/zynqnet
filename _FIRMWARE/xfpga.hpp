
#ifndef XFPGA_H
#define XFPGA_H

/***************************** Include Files *********************************/
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

#include "axilite.hpp"
#include "shared_dram.hpp"
#include "network.hpp"
#include "netconfig.hpp"

/************************** Function Prototypes *****************************/
void XFPGA_Initialize();
void XFPGA_Release();

void XFPGA_Start();
u32 XFPGA_IsDone();
u32 XFPGA_IsIdle();
u32 XFPGA_IsReady();

void XFPGA_setLayerConfig(layer_t &layer);
void XFPGA_setInputOffset(u32 offset);
void XFPGA_setWeightsOffset(u32 offset);
void XFPGA_setDRAMBase();

volatile float *XFPGA_shared_DRAM_virtual();
volatile float *XFPGA_shared_DRAM_physical();

void XFPGA_Set_layer_width_V(u32 Data);
u32 XFPGA_Get_layer_width_V();
void XFPGA_Set_layer_height_V(u32 Data);
u32 XFPGA_Get_layer_height_V();
void XFPGA_Set_layer_channels_in_V(u32 Data);
u32 XFPGA_Get_layer_channels_in_V();
void XFPGA_Set_layer_channels_out_V(u32 Data);
u32 XFPGA_Get_layer_channels_out_V();
void XFPGA_Set_layer_kernel_V(u32 Data);
u32 XFPGA_Get_layer_kernel_V();
void XFPGA_Set_layer_stride_V(u32 Data);
u32 XFPGA_Get_layer_stride_V();
void XFPGA_Set_layer_pad(u32 Data);
u32 XFPGA_Get_layer_pad();
void XFPGA_Set_layer_relu(u32 Data);
u32 XFPGA_Get_layer_relu();
void XFPGA_Set_layer_is_first_split_layer(u32 Data);
u32 XFPGA_Get_layer_is_first_split_layer();
void XFPGA_Set_layer_is_second_split_layer(u32 Data);
u32 XFPGA_Get_layer_is_second_split_layer();
void XFPGA_Set_layer_global_pool(u32 Data);
u32 XFPGA_Get_layer_global_pool();
void XFPGA_Set_layer_mem_addr_input_V(u32 Data);
u32 XFPGA_Get_layer_mem_addr_input_V();
void XFPGA_Set_layer_mem_addr_output_V(u32 Data);
u32 XFPGA_Get_layer_mem_addr_output_V();
void XFPGA_Set_layer_mem_addr_weights_V(u32 Data);
u32 XFPGA_Get_layer_mem_addr_weights_V();
void XFPGA_Set_SHARED_DRAM(u32 Data);
u32 XFPGA_Get_SHARED_DRAM();
void XFPGA_Set_weights_offset(u32 Data);
u32 XFPGA_Get_weights_offset();
void XFPGA_Set_num_weights_V(u32 Data);
u32 XFPGA_Get_num_weights_V();
void XFPGA_Set_input_offset(u32 Data);
u32 XFPGA_Get_input_offset();

void XFPGA_InterruptGlobalEnable();
void XFPGA_InterruptGlobalDisable();
void XFPGA_InterruptEnable(u32 Mask);
void XFPGA_InterruptDisable(u32 Mask);
void XFPGA_InterruptClear(u32 Mask);
u32 XFPGA_InterruptGetEnabled();
u32 XFPGA_InterruptGetStatus();

#endif
