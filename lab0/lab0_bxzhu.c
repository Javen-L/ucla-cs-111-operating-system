#include <sys/stat.h>
#include <string.h>
#include <fcntl.h> // used for open function
#include <unistd.h> // file descriptor
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>

/*
exit是一个函数，进程退出时会有一个值，exit函数的参数就是指明进程退出的返回值，
操作系统根据这个值来判断是否是正常退出。你也可以通过GetExitCodeProcess来获取这个值（windows下），
一般情况下退出值是0表示正常(exit(0))，其它情况都是不正常的。
*/
void handler(int sigNum)
{
  if(sigNum == SIGSEGV) {
    int errorNumber = errno;
    fprintf(stderr, "Error: %s\n", strerror(errorNumber));
    fprintf(stderr,"SIGSEGV signal %d: Segmentation Fault! SIGSEGV signal!\n", arg);
  // int fprintf( FILE *stream, const char *format, ... );  stderr is the standard output file 
    exit(4);  
  }   
}

int main (int argc, char** argv)
{
  char c, // input  
  ifd, ofd; // inout file descriptor
  bool segFault = false; // segment fault
  /* 
  struct option
  {
     constchar *name; //name represents long argument name
     inthas_arg； //has_arg has three values: no_argument(or 0)，means follows no argument
                //required_argument(or 1), means must have arguments 
                //optional_argument(or 2), means could have arguments
     int*flag;   //used to determine the return value  
     int val;     
   }；
  */
  static struct option long_options[] = {
    {"input", 1, 0, 'i'}, 
    {"output", 1, 0, 'o'},
    {"segfault", 0, 0, 's'},
    {"catch", 0, 0, 'c'}
  };
  while((choice = getopt_long(argc, argv, "i:o:sc", long_options, NULL)) != -1) { 
      // c is the option that depends on the input 
      switch(c) {
      case 'i': // if is the input file
        //  int open(const char * pathname, int flags);
        ifd = open(optarg, O_RDONLY); // O_RDONLY read only to open the file return value is -1 means failed opening
          // if opened successfully
        if (ifd >= 0) {
          close(0);   //close stdin and let ifd to do standard input
          dup(ifd);
          close(ifd);
        } else { 
          int errorNumber = errno;
          fprintf(stderr, "Error: %s\n", strerror(errorNumber));
          fprintf(stderr, "Error: Not able to find input file %s\n", optarg);
          exit(2);
        }
        break;
      case 'o':
        ofd = creat(optarg, S_IRWXU);
        if (ofd >= 0) {
          close(1);
          dup(ofd);
          close(ofd);
        } else {
          int errorNumber = errno;
          fprintf(stderr, "Error: %s\n", strerror(errorNumber));
          fprintf(stderr, "unable to create the specified output file %s\n", optarg); 
          exit(3);
        }
        break;
      case 's': segFault = true;
        break;
      case 'c': signal(SIGSEGV, handler);
        break;
      }
    }
    if (segFault) {
      char *fault = NULL;
      *fault = 'a';
    }
    char buff[512];
    int size = read(0, buff, 512);
    while (size>0)
    {
      write(1, buff, size);
      size = read(0, buff, 512);
    }
    return 0; // if no errors should return 0
}
