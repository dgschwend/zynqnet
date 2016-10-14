
#ifndef AXILITE_H_9B5B43B5
#define AXILITE_H_9B5B43B5

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

// Location + Size of Axilite-MMAP segment:
// - from Vivado Block Designer (Address Editor): 
//     AXILITE memory starts at 0x43c00000 – 0x43c0FFFF, SIZE: 64KB
// - from xfpga_top_hw.h:
//     highest byte-address is 0xa0. 0xa4 is "reserved"
const off_t AXILITE_BASE_ADDR = 0x43c00000; 
const size_t AXILITE_MEM_SIZE = 0xFFFF;  // actual address range is 64KB
extern int AXILITE_FD;
extern volatile u32* AXILITE_BUS;

volatile u32* map_axilite_bus(off_t base_addr);
void release_axilite_bus(volatile u32* axilite);

bool axilite_open();
bool axilite_close();

// 32-bit word read + write (other sizes not supported!)
void axilite_write(u32 byte_addr, u32 value); 
u32 axilite_read(u32 byte_addr);

#endif /* end of include guard: AXILITE_H_9B5B43B5 */


