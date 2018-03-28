#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<ucontext.h>


void write_context_to_ckpt_header(ucontext_t *context, int context_len, int fd);
void writetostruct(char * start_Addr,char * end_Addr,char * rwxc,int);

void write_struct_and_data(int,int);
void parseline(char *buffer,int);

struct MemoryRegion {
  char *startAddress;
  char *endAddress;
  int isReadable;
  int isWriteable;
  int isExecutable;

};
void signalhandler(int num);
