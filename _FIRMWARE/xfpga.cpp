#include "xfpga.hpp"

// high-level access functions:
/////////////////////////////////

void XFPGA_Initialize() { 
  printf("XFPGA Driver: Initialize\n");
  axilite_open();
  SHARED_DRAM_open();
}

void XFPGA_Release() {
  printf("XFPGA Driver: Release\n");
  axilite_close();
  SHARED_DRAM_close();
}

volatile data_t *XFPGA_shared_DRAM_virtual() {
    printf("XFPGA Driver: SHARED_DRAM_virtual() = %X\n", (unsigned long)(SHARED_DRAM_virtual()));
    return (volatile data_t*) (SHARED_DRAM_virtual());
}

volatile data_t *XFPGA_shared_DRAM_physical() {
    printf("XFPGA Driver: SHARED_DRAM_physical() = %X\n", (unsigned long)(SHARED_DRAM_physical()));
    return (volatile data_t*) (SHARED_DRAM_physical());
}

void XFPGA_setLayerConfig(layer_t &layer) {
  XFPGA_Set_layer_width_V(layer.width);
  XFPGA_Set_layer_height_V(layer.height);
  XFPGA_Set_layer_channels_in_V(layer.channels_in);
  XFPGA_Set_layer_channels_out_V(layer.channels_out);
  XFPGA_Set_layer_kernel_V(layer.kernel);
  XFPGA_Set_layer_stride_V(layer.stride);
  XFPGA_Set_layer_pad(layer.pad);
  XFPGA_Set_layer_relu(layer.relu);
  XFPGA_Set_layer_is_first_split_layer(layer.is_first_split_layer);
  XFPGA_Set_layer_is_second_split_layer(layer.is_second_split_layer);
  XFPGA_Set_layer_global_pool(layer.global_pool);
  XFPGA_Set_layer_mem_addr_input_V(layer.mem_addr_input);
  XFPGA_Set_layer_mem_addr_output_V(layer.mem_addr_output);
  XFPGA_Set_layer_mem_addr_weights_V(layer.mem_addr_weights);
  u32 weights_per_filter = (layer.kernel == 3) ? 9 : 1;
  u32 num_weights =
      layer.channels_in * layer.channels_out * weights_per_filter +
      layer.channels_out;
  XFPGA_Set_num_weights_V(num_weights);
}

void XFPGA_setWeightsOffset(u32 offset) { XFPGA_Set_weights_offset(offset); }
void XFPGA_setInputOffset(u32 offset) { XFPGA_Set_input_offset(offset); }
void XFPGA_setDRAMBase() {
  volatile data_t *SHARED_DRAM = XFPGA_shared_DRAM_physical();
  uint64_t SHARED_DRAM_64b = (uint64_t)SHARED_DRAM;
  // make sure DRAM address really fits into 32bits (no clipping)
  assert(SHARED_DRAM_64b - (u32)SHARED_DRAM_64b == 0);
  // set DRAM address via AXILITE BUS
  XFPGA_Set_SHARED_DRAM((u32)SHARED_DRAM_64b);
}

void XFPGA_Start() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_AP_CTRL) & 0x80;
  axilite_write(XFPGA_TOP_AXILITE_ADDR_AP_CTRL, Data | 0x01);
}

u32 XFPGA_IsDone() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_AP_CTRL);
  return (Data >> 1) & 0x1;
}

u32 XFPGA_IsIdle() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_AP_CTRL);
  return (Data >> 2) & 0x1;
}

u32 XFPGA_IsReady() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_AP_CTRL);
  // check ap_start to see if the pcore is ready for next input
  return !(Data & 0x1);
}

// low-level access:
/////////////////////////////////

void XFPGA_Set_layer_width_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_WIDTH_V_DATA, Data);
}

u32 XFPGA_Get_layer_width_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_WIDTH_V_DATA);
  return Data;
}

void XFPGA_Set_layer_height_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_HEIGHT_V_DATA, Data);
}

u32 XFPGA_Get_layer_height_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_HEIGHT_V_DATA);
  return Data;
}

void XFPGA_Set_layer_channels_in_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_CHANNELS_IN_V_DATA, Data);
}

u32 XFPGA_Get_layer_channels_in_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_CHANNELS_IN_V_DATA);
  return Data;
}

void XFPGA_Set_layer_channels_out_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_CHANNELS_OUT_V_DATA, Data);
}

u32 XFPGA_Get_layer_channels_out_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_CHANNELS_OUT_V_DATA);
  return Data;
}

void XFPGA_Set_layer_kernel_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_KERNEL_V_DATA, Data);
}

u32 XFPGA_Get_layer_kernel_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_KERNEL_V_DATA);
  return Data;
}

void XFPGA_Set_layer_stride_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_STRIDE_V_DATA, Data);
}

u32 XFPGA_Get_layer_stride_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_STRIDE_V_DATA);
  return Data;
}

void XFPGA_Set_layer_pad(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_PAD_DATA, Data);
}

u32 XFPGA_Get_layer_pad() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_PAD_DATA);
  return Data;
}

void XFPGA_Set_layer_relu(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_RELU_DATA, Data);
}

u32 XFPGA_Get_layer_relu() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_RELU_DATA);
  return Data;
}

void XFPGA_Set_layer_is_first_split_layer(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_IS_FIRST_SPLIT_LAYER_DATA, Data);
}

u32 XFPGA_Get_layer_is_first_split_layer() {
  u32 Data =
      axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_IS_FIRST_SPLIT_LAYER_DATA);
  return Data;
}

void XFPGA_Set_layer_is_second_split_layer(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_IS_SECOND_SPLIT_LAYER_DATA, Data);
}

u32 XFPGA_Get_layer_is_second_split_layer() {
  u32 Data =
      axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_IS_SECOND_SPLIT_LAYER_DATA);
  return Data;
}

void XFPGA_Set_layer_global_pool(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_GLOBAL_POOL_DATA, Data);
}

u32 XFPGA_Get_layer_global_pool() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_GLOBAL_POOL_DATA);
  return Data;
}

void XFPGA_Set_layer_mem_addr_input_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_MEM_ADDR_INPUT_V_DATA, Data);
}

u32 XFPGA_Get_layer_mem_addr_input_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_MEM_ADDR_INPUT_V_DATA);
  return Data;
}

void XFPGA_Set_layer_mem_addr_output_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_MEM_ADDR_OUTPUT_V_DATA, Data);
}

u32 XFPGA_Get_layer_mem_addr_output_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_MEM_ADDR_OUTPUT_V_DATA);
  return Data;
}

void XFPGA_Set_layer_mem_addr_weights_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_LAYER_MEM_ADDR_WEIGHTS_V_DATA, Data);
}

u32 XFPGA_Get_layer_mem_addr_weights_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_LAYER_MEM_ADDR_WEIGHTS_V_DATA);
  return Data;
}

void XFPGA_Set_SHARED_DRAM(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_SHARED_DRAM_DATA, Data);
}

u32 XFPGA_Get_SHARED_DRAM() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_SHARED_DRAM_DATA);
  return Data;
}

void XFPGA_Set_weights_offset(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_WEIGHTS_OFFSET_DATA, Data);
}

u32 XFPGA_Get_weights_offset() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_WEIGHTS_OFFSET_DATA);
  return Data;
}

void XFPGA_Set_num_weights_V(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_NUM_WEIGHTS_V_DATA, Data);
}

u32 XFPGA_Get_num_weights_V() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_NUM_WEIGHTS_V_DATA);
  return Data;
}

void XFPGA_Set_input_offset(u32 Data) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_INPUT_OFFSET_DATA, Data);
}

u32 XFPGA_Get_input_offset() {
  u32 Data = axilite_read(XFPGA_TOP_AXILITE_ADDR_INPUT_OFFSET_DATA);
  return Data;
}

void XFPGA_InterruptGlobalEnable() {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_GIE, 1);
}

void XFPGA_InterruptGlobalDisable() {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_GIE, 0);
}

void XFPGA_InterruptEnable(u32 Mask) {
  u32 Register = axilite_read(XFPGA_TOP_AXILITE_ADDR_IER);
  axilite_write(XFPGA_TOP_AXILITE_ADDR_IER, Register | Mask);
}

void XFPGA_InterruptDisable(u32 Mask) {
  u32 Register = axilite_read(XFPGA_TOP_AXILITE_ADDR_IER);
  axilite_write(XFPGA_TOP_AXILITE_ADDR_IER, Register & (~Mask));
}

void XFPGA_InterruptClear(u32 Mask) {
  axilite_write(XFPGA_TOP_AXILITE_ADDR_ISR, Mask);
}

u32 XFPGA_InterruptGetEnabled() {
  return axilite_read(XFPGA_TOP_AXILITE_ADDR_IER);
}

u32 XFPGA_InterruptGetStatus() {
  return axilite_read(XFPGA_TOP_AXILITE_ADDR_ISR);
}