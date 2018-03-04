

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <getopt.h>
#include <mcrypt.h>

//Variables for encryption and shell program
char* encryption_key;
char* encryption_key_file;
MCRYPT td;
int KEY_LEN = 16;
int kfd;
char* IV;
int to_shell_const = 1;
int to_socket_const = 0;
int to_shell[2]; //parent to child pipe
int from_shell[2]; // child to parent pipe
pid_t child;
char* arg[] = {NULL};

//Print error message                                                                  
void error_message(int num) {
  fprintf(stderr, "Error: %s\n", strerror(num));
  exit(1);
}

//Setup the encryption 
void setup_encryption() {
  td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  int status0 = errno;
  if(td == MCRYPT_FAILED) {
    error_message(status0);
  }

  kfd = open(encryption_key_file, O_RDONLY);
  int status1 = errno;
  if(kfd == -1) {
    error_message(status1);
  }

  encryption_key = malloc(KEY_LEN);
  int status2 = errno;
  if(encryption_key == NULL) {
    error_message(status2);
  }

  int iv_size = mcrypt_enc_get_iv_size(td);
  IV = malloc(iv_size);
  int status3 = errno;
  if(IV == NULL) {
    error_message(status3);
  }

  memset(encryption_key, 0, KEY_LEN);
  
  int bytes_read = read(kfd, encryption_key, KEY_LEN);
  int status4 = errno;
  if(bytes_read != KEY_LEN) {
    error_message(status4);
  }

  int test5 = close(kfd);
  int status5 = errno;
  if(test5 == -1) {
    error_message(status5);
  }

  int i;
  for(i = 0; i < iv_size; i++) {
    IV[i] = 0;
  }

  int test6 = mcrypt_generic_init(td, encryption_key, KEY_LEN, IV);
  int status6 = errno;
  if(test6 == -1) {
    error_message(status6);
  }

  free(encryption_key);
  free(IV);
}

//Print correct usage message                                                     
void correct_message() {
  fprintf(stderr, "Correct Usage: ./lab1b-client --port=portnumber [--log=logfile] [--encrypt=keyfile]\n");
  exit(1);
}

//Read from buffer                                                                
int read_bytes(int file_descriptor, char* buf, int bytes) {
  int num_bytes = read(file_descriptor, buf, bytes);
  int status = errno;
  if(num_bytes == -1) {
    error_message(status);
  }

  return num_bytes;
}

//Write to buffer                                                                 
int write_bytes(int file_descriptor, char* buf, int bytes) {
  int num_bytes = write(file_descriptor, buf, bytes);
  int status = errno;
  if(num_bytes == -1) {
    error_message(status);
  }

  return num_bytes;
}

//Write char by char                                                              
void write_char_by_char(char* buf, int output_file_descriptor, int bytes, int input_file_descriptor) {
  int i;
  for(i = 0; i < bytes; i++) {
    char ch = *(buf + i);

    switch(ch) {
      //^C
    case 0x03: if(input_file_descriptor == to_shell_const) {
	//Kill the shell process, but keep on processing input in transit
	int test0 = kill(child, SIGINT);
	int status0 = errno;
	if(test0 == -1) {
	  error_message(status0);
	}
      }
      break;
    
      //^D
    case 0x04: if(input_file_descriptor == to_shell_const) {
	int test1 = close(to_shell[1]);
	int status1 = errno;
	if(test1 == -1) {
	  error_message(status1);
	}
      }
      break;
    default: write_bytes(output_file_descriptor, buf + i, 1);
    }
  }
}

//Function to wait until shell closes
void shell_wait() {
  int status;
  pid_t waited = waitpid(child, &status, 0);
  int test = errno;
  if(waited == -1) {
    error_message(test);
  }

  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
  exit(0);
}

//Signal handler
void handler(int signum) {
  if(signum == SIGPIPE || signum == SIGINT || signum == SIGTERM) {
    shell_wait();
  }
}

int main(int argc, char** argv) {
  int option;
  int port_number;
  int port_flag = 0;
  int encrypt_flag = 0;
  int debug_flag = 0;
  int sockid;
  int newsockid;

  //Array of options structs                                                      
  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"encrypt", required_argument, 0, 'e'},
    {"debug", no_argument, 0, 'd'},
    {0, 0, 0, 0}
  };

  while((option = getopt_long(argc, argv, "p:l:e:d", long_options, NULL)) != -1) \
    {
      switch(option) {
      case 'p': port_number = atoi(optarg); port_flag = 1; break;
      case 'e': encrypt_flag = 1; encryption_key_file = optarg; break;
      case 'd': debug_flag = 1; break;
      default: correct_message();
      }
    }

  //Check for debug flag
  if(debug_flag) {
    exit(0);
  }


  //Check if port_flag is set
  if(port_flag == 0) {
    correct_message();
  }

  //Check if encryption flag set
  if(encrypt_flag) {
    setup_encryption();
  }
  

  //Register signal handlers for shell
  signal(SIGINT, handler);
  signal(SIGPIPE, handler);
  signal(SIGTERM, handler);

  //Set up network connection
  struct sockaddr_in serv_addr, cli_addr;
  sockid = socket(AF_INET, SOCK_STREAM, 0);
  int status17 = errno;
  if(sockid == -1) {
    error_message(status17);
  }

  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_number);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  
  int test18 = bind(sockid, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  int status18 = errno;
  if(test18 == -1) {
    error_message(status18);
  }

  int test19 = listen(sockid, 5);
  int status19 = errno;
  if(test19 == -1) {
    error_message(status19);
  }

  unsigned int clilen = sizeof(cli_addr);

  newsockid = accept(sockid, (struct sockaddr*) &cli_addr, &clilen);
  int status20 = errno;
  if(newsockid == -1) {
    error_message(status20);
  }

  //Set up pipes to shell
  int test0 = pipe(to_shell);
  int status0 = errno;
  if(test0 == -1) {
    error_message(status0);
  }

  int test1 = pipe(from_shell);
  int status1 = errno;
  if(test1 == -1) {
    error_message(status1);
  }

  //Fork child process
  child = fork();
  int status2 = errno;
  if(child == -1) {
    error_message(status2);
  }

  //In child process
  if(child == 0) {
    //Close unused pipes
    int test3 = close(to_shell[1]);
    int status3 = errno;
    if(test3 == -1) {
      error_message(status3);
    }

    int test4 = close(from_shell[0]);
    int status4 = errno;
    if(test4 == -1) {
      error_message(status4);
    }

    int test5 = dup2(to_shell[0], STDIN_FILENO);
    int status5 = errno;
    if(test5 == -1) {
      error_message(status5);
    }

    int test6 = close(to_shell[0]);
    int status6 = errno;
    if(test6 == -1) {
      error_message(status6);
    }

    int test7 = dup2(from_shell[1], STDOUT_FILENO);
    int status7 = errno;
    if(test7 == -1) {
      error_message(status7);
    }

    int test8 = dup2(from_shell[1], STDERR_FILENO);
    int status8 = errno;
    if(test8 == -1) {
      error_message(status8);
    }

    int test9 = close(from_shell[1]);
    int status9 = errno;
    if(test9 == -1) {
      error_message(status9);
    }

    int test10 = execvp("/bin/bash", arg);
    int status10 = errno;
    if(test10 == -1) {
      error_message(status10);
    }

    

    
  } else {
    //In parent process
    
    int test11 = close(to_shell[0]);
    int status11 = errno;
    if(test11 == -1) {
      error_message(status11);
    }

    int test12 = close(from_shell[1]);
    int status12 = errno;
    if(test12 == -1) {
      error_message(status12);
    }

    //Create poll structs to listen for inputs from shell and network socket
    struct pollfd poll_struct[2];
    
    //Set up poll_struct for listening to input from socket
    poll_struct[0].fd = newsockid;
    poll_struct[0].events = POLLIN | POLLHUP | POLLERR;
    
    //Set up poll_struct for listening to input from shell
    poll_struct[1].fd = from_shell[0];
    poll_struct[1].events = POLLIN | POLLHUP | POLLERR;


    //While loop to listen for events
    while(1) {
      int test13 = poll(poll_struct, 2, 0);
      int status13 = errno;
      if(test13 == -1) {
	error_message(status13);
      }


      //Check for timeout of zero ms
      if(test13 == 0) {
	continue;
      }


      //Check for input coming from the socket; write to shell
      if(poll_struct[0].revents & POLLIN) {
	char from_socket[256];
	int bytes_read = read(newsockid, from_socket, 256);

	//Check if EOF or read error system call
	if(bytes_read == 0 || bytes_read == -1) {
	  int test15 = close(to_shell[1]);
	  int status15 = errno;

	  if(test15 <= -1) {
	    error_message(status15);
	  }

	  //wait for shell to close and log error message to STDERR
	  shell_wait();
	  break;
	}

	//Check for encryption flag
	if(encrypt_flag) {
	  //Decrypt before writing to the shell
	  int k;
	  for(k = 0; k < bytes_read; k++) {
	    if(from_socket[k] != '\n' && from_socket[k] != '\r') {
	      int test17 = mdecrypt_generic(td, &from_socket[k], 1);
	      int status17 = errno;
	      if(test17 == -1) {
		error_message(status17);
	      }
	    }
	  }
	}
	
	//Else, write to the shell whatever what was read from the socket
	write_char_by_char(from_socket, to_shell[1], bytes_read, to_shell_const);
      }


      //Check for input from shell pipe; write it to the socket
      if(poll_struct[1].revents & POLLIN) {
	char from_shell_buffer[256];
	int bytes_read0 = read_bytes(from_shell[0], from_shell_buffer, 256);
	//Check for EOF from shell
	if(bytes_read0 == 0) {
	  shell_wait();
	  break;
	}


	//Check for encryption flag
	if(encrypt_flag) {
	  int test18 = mcrypt_generic(td, &from_shell_buffer, bytes_read0);
	  int status18 = errno;
	  if(test18 != 0) {
	    error_message(status18);
	  }
	}
	
	
	//Else, write to socket
	write_char_by_char(from_shell_buffer, newsockid, bytes_read0, to_socket_const);
      }

      //Check for error events coming from shell
      if(poll_struct[1].revents & (POLLHUP | POLLERR)) {
	int test16 = close(poll_struct[1].fd);
	int status16 = errno;
	if(test16 == -1) {
	  error_message(status16);
	}

	shell_wait();
	break;
      }
      //While loop ends
    }
  }
  //Exit successfully
  exit(0);
}
