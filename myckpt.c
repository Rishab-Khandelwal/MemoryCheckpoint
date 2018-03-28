#include "myckpt.h"

__attribute__((constructor))
void myconstructor()
{
  signal(SIGUSR2,signalhandler);
}
void signalhandler(int number) {
int f = 0;
int input_fd = open("/proc/self/maps",O_RDONLY);
int output_fd = open("myckpt",O_CREAT|O_WRONLY|O_APPEND,S_IRWXU);

if (input_fd == -1)
{
  perror("error in opening file");
}
if (output_fd == -1)
{
  perror("error in opening file");
}
ucontext_t mycontext;
getcontext(&mycontext);
if (f == 1){
  return;
}
f++;
write_context_to_ckpt_header(&mycontext,sizeof(mycontext),output_fd);
write_struct_and_data(input_fd,output_fd);
close(output_fd);
close(input_fd);
}

void write_struct_and_data(int input_fd,int output_fd) {
char buffer[1000];
char c;
int read_fd = read(input_fd,&c,sizeof(c));
if(read_fd == -1)
{
  perror("Error reading file");
}
int i = 0;
while(read_fd != 0)
{
  if (c == '\n')
  {
    buffer[i] = '\0';
    parseline(buffer,output_fd);
    i = 0;
  }
  buffer[i] = c;
  i++;
  read_fd = read(input_fd,&c,sizeof(c));
}
struct MemoryRegion meme = {0};
write(output_fd,&meme,sizeof(struct MemoryRegion));
}
void parseline(char *buffer , int output_fd)
{
  char start_Addr[1000],end_Addr[1000],rwxc[5];
  memset(rwxc,'\0',sizeof(rwxc));
    if (strstr(buffer,"[vsyscall]") == NULL)
    {
    int k = 0,j = 0;
    while(buffer[k] != '-')
    {
      start_Addr[j] = buffer[k];
      j++;
      k++;
    }
    start_Addr[j] = '\0';
    k++;
    j = 0;
    while(buffer[k] != ' ')
    {
      end_Addr[j] = buffer[k];
      j++;
      k++;
    }
    end_Addr[j] = '\0';
    j = 0;
    k++;
    while(buffer[k] != ' ')
    {
      rwxc[j] = buffer[k];
      k++;
      j++;
    }
    writetostruct(start_Addr,end_Addr,rwxc,output_fd);
  }
}
void writetostruct(char * start_Addr,char * end_Addr,char * rwxc , int output_fd){
    struct MemoryRegion Memr;
    Memr.startAddress = (char*)strtol(start_Addr,NULL,16);
    Memr.endAddress = (char*)strtol(end_Addr,NULL,16);
    if(rwxc[0] == 'r')
    {
      Memr.isReadable = 1;
    }else{
      Memr.isReadable = 0;
    }
    if(rwxc[1] == 'w')
    {
      Memr.isWriteable = 1;
    }else{
      Memr.isWriteable = 0;
    }
    if(rwxc[2] == 'x')
    {
      Memr.isExecutable = 1;
    }else{
      Memr.isExecutable = 0;
    }
    if (Memr.isReadable == 1)
    {
    write(output_fd,&Memr,sizeof(struct MemoryRegion));
    size_t sz = Memr.endAddress - Memr.startAddress;
    write(output_fd, Memr.startAddress,sz);
    }
  }
void write_context_to_ckpt_header(ucontext_t *context, int len,int fd)
{
  if(write(fd,context,len) == -1 )
  {
    printf("Failed to write buffer");
  }
}
