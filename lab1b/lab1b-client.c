

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <getopt.h>
#include <mcrypt.h>


//Variables for server/encryption
char* encryption_key;
char* encryption_key_file;
char* IV;
MCRYPT td;
int KEY_LEN = 16;
int kfd;
int TO_SERVER = 10;
int FROM_SERVER = 5;
int log_file_file_descriptor;

//Variables for terminal and reading/writing to buffer
char buf[256];
pid_t child;
struct termios saved_state;
struct termios current_state;

//Print error message
void error_message(int num) {
  fprintf(stderr, "Error: %s\n", strerror(num));
  exit(1);
}

//Print correct usage message
void correct_message() {
  fprintf(stderr, "Correct Usage: client --port=portnumber [--log=logfile] [--encrypt=keyfile]\n");
  exit(1);
}

//Save terminal settings
void save_terminal_settings() {
  int test = tcgetattr(STDIN_FILENO, &saved_state);
  int status = errno;

  if(test == -1) {
    error_message(status);
  }
}

//Restore to original terminal settings
void restore_terminal_settings() {
  int test = tcsetattr(STDIN_FILENO, TCSANOW, &saved_state);
  int status = errno;
  if(test == -1) {
    error_message(status);
  }
}

//Set terminal to non-canonical no echo mode
void set_terminal_settings() {
  int test0 = tcgetattr(STDIN_FILENO, &current_state);
  int status0 = errno;

  if(test0 == -1) {
    error_message(status0);
  }

  current_state.c_iflag = ISTRIP;
  current_state.c_oflag = 0;
  current_state.c_lflag = 0;

  int test1 = tcsetattr(STDIN_FILENO, TCSANOW, &current_state);
  int status1 = errno;

  if(test1 == -1) {
    error_message(status1);
  }
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
void write_char_by_char(char* buf, int file_descriptor,int bytes) {
  int i;
  for(i = 0; i < bytes; i++) {
    char ch = *(buf + i);

    switch(ch) {
    case '\r':
    case '\n': if(file_descriptor == STDOUT_FILENO) {
	char tmp[2];
	tmp[0] = '\r';
	tmp[1] = '\n';
	write_bytes(file_descriptor, tmp, 2);
      } else {
	char tmp[1]; 
	tmp[0] = '\n';
	write_bytes(file_descriptor, tmp, 1);
      }
      break;
    default: write_bytes(file_descriptor, buf + i, 1);
    }
  }
}

//Write to the log file                                                                
void write_log_file(char* buf, int bytes, int target) {
  if(target == TO_SERVER) {
    int test0 = dprintf(log_file_file_descriptor, "SENT %d bytes: ", bytes);
    int status0 = errno;
    if(test0 == -1) {
      error_message(status0);
    }

    write_char_by_char(buf, log_file_file_descriptor, bytes);
    write_bytes(log_file_file_descriptor, "\n", 1);
  }

  if(target == FROM_SERVER) {
    int test1 = dprintf(log_file_file_descriptor, "RECEIVED %d bytes: ", bytes);
    int status1 = errno;
    if(test1 == -1) {
      error_message(status1);
    }

    write_char_by_char(buf, log_file_file_descriptor, bytes);

    write_bytes(log_file_file_descriptor, "\n", 1);
  }
}


//Set up the encryption process
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


int main(int argc, char** argv) {
  char* log_file;
  int option;
  int port_number;
  int port_flag = 0;
  int log_flag = 0;
  int encrypt_flag = 0;
  int debug_flag = 0;

  //Array of options structs
  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"log", optional_argument, 0, 'l'},
    {"encrypt", required_argument, 0, 'e'},
    {"debug", no_argument, 0, 'd'},
    {0, 0, 0, 0}
  };

  while((option = getopt_long(argc, argv, "p:l:e:d", long_options, NULL)) != -1) {
    switch(option) {
    case 'p': port_number = atoi(optarg); port_flag = 1; break;
    case 'l': log_file = optarg; log_flag = 1; break;
    case 'e': encrypt_flag = 1; encryption_key_file = optarg; break;
    case 'd': debug_flag = 1; break;
    default: correct_message();
    }
  }

  //Check if debug flag set
  if(debug_flag == 1) {
    *log_file = 'c';
    exit(0);
  }


  //Check if user entered port_number
  if(port_flag == 0) {
    correct_message();
  }

  //Check if user enabled encyption flag
  if(encrypt_flag) {
    setup_encryption();
  }

  //Check if user enabled log flag
  if(log_flag) {
    log_file_file_descriptor = creat(log_file, S_IRWXU);
    int status6 = errno;
    if(log_file_file_descriptor == -1) {
      error_message(status6);
    }
  }

  //Save terminal settings
  save_terminal_settings();

  //Upon exiting, call restore_terminal_settings function
  atexit(restore_terminal_settings);

  //Set terminal settings to non-canonical no-echo mode
  set_terminal_settings();  


  //Set up socket connection with server
  struct sockaddr_in serv_addr;
  struct hostent* server = gethostbyname("localhost");
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int status0 = errno;

  if(sockfd == -1) {
    error_message(status0);
  }

  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_number);
  memcpy((char*) &serv_addr.sin_addr.s_addr, (char*) server->h_addr, server->h_length);
 

  //Connect to the server
  int connection = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  int status1 = errno;

  if(connection == -1) {
    error_message(status1);
  }
 

  //Set up poll structs to listen to events coming from keyboard and server
  struct pollfd poll_struct[2];

  //Poll struct for keyboard event
  poll_struct[0].fd = STDIN_FILENO;
  poll_struct[0].events = POLLIN | POLLHUP| POLLERR;

  //Poll struct for server input event
  poll_struct[1].fd = sockfd;
  poll_struct[1].events = POLLIN | POLLHUP | POLLERR;

  //While loop to listen to events
  while(1) {
    int poll_status = poll(poll_struct, 2, 0);
    int status2 = errno;

    if(poll_status == -1) {
      error_message(status2);
    }
    
    //Check for the timeout of zero ms
    if(poll_status == 0) {
      continue;
    }


    //Check for input from keyboard
    if(poll_struct[0].revents & POLLIN) {
      char from_keyboard[256];
      //Read from the keyboard, and put it in the from_keyboard buffer
      int bytes_read = read_bytes(STDIN_FILENO, from_keyboard, 256);
      //Echo back to the keyboard from the keyboard input
      write_char_by_char(from_keyboard, STDOUT_FILENO, bytes_read);
      
      //Check for encryption flag
      if(encrypt_flag) {
	int k;
	//Encrypt char by char
	for(k = 0; k < bytes_read; k++) {
	  if(from_keyboard[k] != '\r' && from_keyboard[k] != '\n') {
	    int test5 = mcrypt_generic(td, &from_keyboard[k], 1);
	    int status5 = errno;
	    if(test5 != 0) {
	      error_message(status5);
	    }
	  }
	}
      }

      //Check for log_file flag
      if(log_flag) {
	write_log_file(from_keyboard, bytes_read, TO_SERVER);
      }
      
      //Write to server from the keyboard
      write_char_by_char(from_keyboard, poll_struct[1].fd, bytes_read);
    }


    //Check for input from server
    if(poll_struct[1].revents & POLLIN) {
      char from_server[256];
      //Read from the socket, and put in the from_server buffer
      int bytes_read = read_bytes(poll_struct[1].fd, from_server, 256);
      
      //^D or ^C 
      if(bytes_read == 0) {
	int test3 = close(sockfd);
      	int status3 = errno;
      	if(test3 == -1) {
      	  error_message(status3);
      	} else {
	  //Exit successfully
        exit(0);
      	}
      }

      //Check for log flag
      if(log_flag) {
	write_log_file(from_server, bytes_read, FROM_SERVER);
      }

      //Check for encryption flag
      if(encrypt_flag) {
	int test6 = mdecrypt_generic(td, &from_server,  bytes_read);
	int status6 = errno;
	if(test6 != 0) {
	  error_message(status6);
	}
      }

      //Write to STDOUT from server
      write_char_by_char(from_server, STDOUT_FILENO, bytes_read);
      
    }

    //Check for error event from stdin
    if(poll_struct[0].revents & (POLLHUP | POLLERR)) {
      int test10 = close(sockfd);
      int status10 = errno;
      if(test10 == -1) {
	error_message(status10);
      }
    }

    
    //Check for error event from server
    if(poll_struct[1].revents & (POLLHUP | POLLERR)) {
      int test4 = close(sockfd);
      int status4 = errno;
      if(test4 == -1) {
	error_message(status4);
      }

      exit(0);
    }

    //End of while loop
  }

  //Successful exit
  exit(0);  
}
