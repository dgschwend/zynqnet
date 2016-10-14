#include "shared_dram.hpp"


int SHARED_DRAM_FD = -1;
volatile u32* SHARED_DRAM_PTR = NULL;

  
bool SHARED_DRAM_open() {
  printf("XFPGA Driver: open /dev/mem handle\n");

  // Check that it's not yet open
  if (SHARED_DRAM_FD > -1) {
    printf("SHARED_DRAM already open!\n"); 
    return false;
  }
  
  // Memory Map SHARED_DRAM
  SHARED_DRAM_PTR = map_SHARED_DRAM(SHARED_DRAM_BASE_ADDR);
  printf("SHARED_DRAM_PTR=%X\n", (unsigned long)SHARED_DRAM_PTR);
  
  // Make sure the file handle is really set
  return (SHARED_DRAM_FD > -1);
}

bool SHARED_DRAM_close() {
  printf("XFPGA Driver: close /dev/mem handle\n");

  // Check that memory file is really open
  if (SHARED_DRAM_FD == -1) {
    printf("SHARED_DRAM bus not open!\n"); 
    return false;
  }
  
  // Release Memory Region and File handle
  release_SHARED_DRAM(SHARED_DRAM_PTR);
  
  // Make sure file was correctly released
  return (SHARED_DRAM_FD == -1);
}

volatile u32* SHARED_DRAM_virtual() {
  return (volatile u32*) SHARED_DRAM_PTR;
}

volatile u32* SHARED_DRAM_physical() {
  return (volatile u32*) SHARED_DRAM_BASE_ADDR;
}


////////////////////////////////////////////////////
////////////////// Helper Functions ////////////////

volatile u32* map_SHARED_DRAM(off_t base_addr) {
  printf("XFPGA Driver: map shared DRAM at base address %X\n", (unsigned long)base_addr);
  // make sure that base addr is aligned to memory pages...
  base_addr &= ~(getpagesize() - 1);

  // Open /dev/mem file (need root privileges or setuid!)
  int SHARED_DRAM_FD = open("/dev/mem", O_RDWR);
  if (SHARED_DRAM_FD < 0) err(errno, "could not open /dev/mem. need to be root");

  // Map SHARED_DRAM memory region to pointer
  volatile u32* pointer = (u32*)mmap(NULL, SHARED_DRAM_MEM_SIZE, PROT_READ | PROT_WRITE,
                            MAP_SHARED, SHARED_DRAM_FD, base_addr);
  if (pointer == MAP_FAILED) err(errno, "could not map memory for SHARED_DRAM bus");

  return pointer;
}

void release_SHARED_DRAM(volatile u32* pointer) {
  printf("XFPGA Driver: unmap shared DRAM\n");
  // Release SHARED_DRAM memory region (unmap)
  int retval = munmap((void*)pointer, SHARED_DRAM_MEM_SIZE);
  if (retval < 0) err(errno, "could not unmap memory region for SHARED_DRAM bus");
  
  // release file handle
  retval = close(SHARED_DRAM_FD);
  if (retval < 0) err(errno, "could not release /dev/mem file handle");

  // set file handle variable s.t. we know it's closed
  SHARED_DRAM_FD = -1;
}


////////////////////////////////////////////////////
////////////////// unused Functions ////////////////
/*
// Write one 32bit Word
void SHARED_DRAM_write(u32 byte_addr, u32 value) {
  SHARED_DRAM_PTR[byte_addr / 4] = value;
}

// Read one 32bit Word 
u32 SHARED_DRAM_read(u32 byte_addr) {
  return SHARED_DRAM_PTR[byte_addr / 4];
}
*/