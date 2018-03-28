#include<stdio.h>
#include<sys/mman.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<ucontext.h>


struct MemoryRegion {
  char *startAddress;
  char *endAddress;
  int isReadable;
  int isWriteable;
  int isExecutable;
};


struct MemoryRegionUn {
  char  startAddr[100];
  char  endAddr[100];
};

struct MemoryRegionUn unmap_stack();
void restore_memory();
void get_context_from_image();
void restore_memory_from_image(int);
int getPermissions(struct MemoryRegion *);
char ckpt_image[1000];
ucontext_t context_to_recover;
