#include "axilite.hpp"

int AXILITE_FD = -1;
volatile u32* AXILITE_BUS = NULL;
  
volatile u32* map_axilite_bus(off_t base_addr) {
  // make sure that base addr is aligned to memory pages...
  base_addr &= ~(getpagesize() - 1);

  // Open /dev/mem file (need root privileges or setuid!)
  AXILITE_FD = open("/dev/mem", O_RDWR);
  if (AXILITE_FD < 0) err(errno, "could not open /dev/mem. need to be root");

  // Map AXILITE memory region to pointer
  volatile u32* axilite = (u32*)mmap(NULL, AXILITE_MEM_SIZE, PROT_READ | PROT_WRITE,
                            MAP_SHARED, AXILITE_FD, base_addr);
  if (axilite == MAP_FAILED) err(errno, "could not map memory for axilite bus");

  return axilite;
}

void release_axilite_bus(volatile u32* axilite) {
  // Release AXILITE memory region (unmap)
  int retval = munmap((void*)axilite, AXILITE_MEM_SIZE);
  if (retval < 0) err(errno, "could not unmap memory region for axilite bus");
  
  // release file handle
  retval = close(AXILITE_FD);
  if (retval < 0) err(errno, "could not release /dev/mem file handle");

  // set file handle variable s.t. we know it's closed
  AXILITE_FD = -1;
}

bool axilite_open() {
  
  // Check that it's not yet open
  if (AXILITE_FD > -1) {
    printf("axilite bus already open!\n"); 
    return false;
  }
  
  // Memory Map Axilite Bus
  AXILITE_BUS = map_axilite_bus(AXILITE_BASE_ADDR);
  
  // Make sure the file handle is really set
  return (AXILITE_FD > -1);
}

/* Write one 32bit Word */
void axilite_write(u32 byte_addr, u32 value) {
  AXILITE_BUS[byte_addr / 4] = value;
}

/* Read one 32bit Word */
u32 axilite_read(u32 byte_addr) {
  return AXILITE_BUS[byte_addr / 4];
}

bool axilite_close() {
  
  // Check that memory file is really open
  if (AXILITE_FD == -1) {
    printf("axilite bus not open!\n"); 
    return false;
  }
  
  // Release Memory Region and File handle
  release_axilite_bus(AXILITE_BUS);
  
  // Make sure file was correctly released
  return (AXILITE_FD == -1);
}
