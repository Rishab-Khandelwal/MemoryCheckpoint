
#include "restart.h"

int main(int argc , char *argv[])
{
  if (argc != 2)
  return -1;
  char *stack_ptr = (char*)0x5300000;
  char *new_sp = (char*)0x5301000;
  strcpy(ckpt_image,argv[1]);
  mmap(stack_ptr,4096,PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  asm volatile ("mov %0,%%rsp" : : "g" (new_sp) : "memory");
  restore_memory();
}

void restore_memory()
{
  struct MemoryRegionUn mr = unmap_stack();
  long len = mr.endAddr - mr.startAddr;
  int fd = open(ckpt_image,O_RDONLY);
  if (fd == -1)
  {
    perror("cannot open the file");
  }
  int flag = 0;
  get_context_from_image(&context_to_recover,fd);
  restore_memory_from_image(fd);
  close(fd);
  setcontext(&context_to_recover);
}

void restore_memory_from_image(int fd)
{
  struct MemoryRegion memR;
  int t = 0;
  while(read(fd,&memR,sizeof(struct MemoryRegion)) > 0 ){
    if (memR.startAddress == NULL && memR.endAddress == NULL)
    {
      return;
    }
    size_t len = memR.endAddress - memR.startAddress;
    if (len == 0)
    {
      return;
    }
    char *buffer = (char*)malloc(len);
    if (read(fd,buffer,len) < 0)
    {
      printf("Error is coming \n");
      return;
    }
    mmap(memR.startAddress,len , getPermissions(&memR),MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED , -1 ,0);
    memcpy(memR.startAddress,buffer,len);
  }
}

int getPermissions(struct MemoryRegion *memR){
  int permissions = PROT_WRITE;
  if (memR->isReadable == 1)
  {
    permissions = permissions | PROT_READ;
  }
  if(memR->isExecutable == 1)
  {
    permissions = permissions | PROT_EXEC;
  }
return permissions;
}

void get_context_from_image(ucontext_t *context, int fd)
{
  int t = read(fd , context,sizeof(ucontext_t));
  if (t == -1)
  {
    perror("Cannot read context");
  }
}
struct MemoryRegionUn unmap_stack()
{
  struct MemoryRegionUn memR;
  int input_res = open("/proc/self/maps",O_RDONLY);
  if(input_res == -1){
     perror("Error in opening file");
  }
  char c,start_Addr[1000],end_Addr[1000],buffer[1000];
  int i = 0;
  int readres = read(input_res,&c, sizeof(c));
  if (readres == -1){
    perror("Error in reading");
  }
  while(readres != 0){
      if (c == '\n'){
        buffer[i] = '\0';
        if(strstr(buffer,"[stack]")){
          int k = 0;
          int j = 0;
          while(buffer[k] != '-'){
            start_Addr[j] = buffer[k];
            j++;
            k++;
          }
          strcpy(memR.startAddr, start_Addr);
          k++;
          j = 0;
          while(buffer[k] != ' '){
            end_Addr[j] = buffer[k];
            j++;
            k++;
          }
          strcpy(memR.endAddr,end_Addr);
          close(input_res);
          return memR;
        }
        else{
          i = 0;
        }
      }
      buffer[i] = c;
      i++;
      readres = read(input_res,&c,sizeof(c));
    }
  }
