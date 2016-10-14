
#ifndef SHARED_DRAM_H_9B5B43B5
#define SHARED_DRAM_H_9B5B43B5

#include <sys/mman.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>

#include "xfpga_hw.hpp"   // Register addresses

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// Location + Size of SHARED DRAM segment:
// - from Vivado Block Designer (Address Editor): 
//     AXI M memory bus starts at 0x00000000 – 0xFFFFFFFF, SIZE: 4GB
// - from information by Simon Wright:
// 	   top 128MB of 1GB system memory are not OS-managed
// - from "free -m" on Zynq:
//	   total mem 882MB -> 118MB not OS-managed
// 	   -> place SHARED_DRAM at 896MB (-> max. activations ~100MB)
//	   -> 896MB = 896*1024*1024 = 0x3800'0000 bytes
//	   -> 96MB = 96*1024*1024 = 0x600'0000 bytes

const off_t SHARED_DRAM_BASE_ADDR = 0x38000000; 
const size_t SHARED_DRAM_MEM_SIZE = 0x06000000;
extern int SHARED_DRAM_FD;
extern volatile u32* SHARED_DRAM_PTR;

// External Interface
bool SHARED_DRAM_open();
bool SHARED_DRAM_close();
volatile u32* SHARED_DRAM_virtual();
volatile u32* SHARED_DRAM_physical();

// Internal Functions
volatile u32* map_SHARED_DRAM(off_t base_addr);
void release_SHARED_DRAM(volatile u32* axilite);

// unused:
// 32-bit word read + write (other sizes not supported!)
/* void shared_DRAM_write(u32 byte_addr, u32 value); 
u32 shared_DRAM_read(u32 byte_addr); */

#endif /* end of include guard: SHARED_DRAM_H_9B5B43B5 */


