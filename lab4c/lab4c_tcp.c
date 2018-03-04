
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <time.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

char scale = 'F';
int log_fd;
int period = 1;
int run_flag = 1;
int log_flag = 0;
int begin = 1;

char* id;
char* host;
int port_num;
int sock_fd;

struct timespec start, end;

mraa_aio_context tmpr;
uint16_t value = 0;
float value_float = 0.0;
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k

//mraa_gpio_context button;


// void get_time(int stdout_flag){
// 	char outstr[128];
// 	time_t rawtime;
// 	struct tm *lt;

// 	time(&rawtime);
// 	lt = localtime(&rawtime);
// 	if(lt == NULL){
// 		fprintf(stderr, "localtime error\n" );
// 		exit(1);
// 	}
// 	if(strftime(outstr, sizeof(outstr), "%H:%M:%S", lt) == 0){
// 		fprintf(stderr, "strftime error\n");
// 		exit(1);
// 	}
// 	if(stdout_flag){
// 		if(write(1, outstr, strlen(outstr)) < 0){
// 			fprintf(stderr, "%s\n", strerror(errno));
// 		}
// 	}
// 	if(log_flag){
// 		if(write(log_fd, outstr, strlen(outstr)) < 0){
// 			fprintf(stderr, "%s\n", strerror(errno));
// 			exit(1);
// 		}
// 	}
// }

void get_temp(){
	clock_gettime(CLOCK_MONOTONIC, &end);
	//if it is the first command, run it
	//if end - start > 0 == period, run it
	//otherwise return
	int during_time = end.tv_sec - start.tv_sec;
	float R = 0.0;
	float temperature = 0.0;
	char outstr[256];
	char outstr2[128];
	if(begin || (during_time % period == 0 && during_time > 0)){
		if(run_flag){
			value = mraa_aio_read(tmpr);
			if (value < 0){
				fprintf(stderr, "Error in mraa_aio_read\n" );
			}
			R = 1023.0/value-1.0;
			R = R0*R;
			temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
			if(scale == 'F'){
				temperature = temperature * 1.8 + 32;
			}
			//get_time(1);
			time_t rawtime;
			struct tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			if(timeinfo == NULL){
				fprintf(stderr, "Localtime error\n");
				exit(2);
			}
			if(strftime(outstr, sizeof(outstr), "%H:%M:%S", timeinfo) == 0){
				fprintf(stderr, "strftime error\n");
				exit(2);
			}

			sprintf(outstr2, " %04.1f\n", temperature);
			strcat(outstr, outstr2);

			printf("%s", outstr);

			if(write(sock_fd, outstr, strlen(outstr)) < 0){
				fprintf(stderr, "%s\n" ,strerror(errno));
				exit(2);
			}
			if(log_flag){
				if(write(log_fd, outstr, strlen(outstr)) < 0){
					fprintf(stderr, "%s\n" ,strerror(errno));
					exit(2);
				}
			}
			//sleep(period);
			clock_gettime(CLOCK_MONOTONIC, &start);
			begin = 0;

		}
	
	}
		return;		
}


void shut_down(){
	char outstr[256];
	//get_time(0);
	bzero(outstr,256);

	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	if(timeinfo == NULL){
		fprintf(stderr, "Localtime error\n");
		exit(2);
	}
	if(strftime(outstr, sizeof(outstr), "%H:%M:%S", timeinfo) == 0){
		fprintf(stderr, "strftime error\n");
		exit(2);
	}

	strcat(outstr," SHUTDOWN\n");
	if(log_flag){
		if(write(log_fd, outstr, strlen(outstr)) < 0){
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
	}
	dprintf(sock_fd, "%s", outstr);
	mraa_aio_close(tmpr);
	//mraa_gpio_close(button);
	exit(0);
}

void command(char instr[28]){
	if(strcmp(instr, "STOP\n") == 0){
		if(log_flag){
			dprintf(log_fd, "%s", instr);
		}
		run_flag = 0;
	}
	else if(strcmp(instr, "START\n") == 0){
		if(log_flag){
			dprintf(log_fd, "%s", instr);
		}
		run_flag = 1;
	}
	else if(strcmp(instr, "SCALE=F\n") == 0){
		if(log_fd){
			dprintf(log_fd, "%s", instr);
		}
		scale = 'F';
	}
	else if(strcmp(instr, "SCALE=C\n") == 0){
		if(log_fd){
			dprintf(log_fd, "%s", instr);
		}
		scale = 'C';
	}
	else if(strncmp(instr, "PERIOD=", 7) == 0){
		period = atoi(instr + 7);
		if(log_fd){
			dprintf(log_fd, "%s", instr);
		}
	}
	else if(strcmp(instr, "OFF\n") == 0){
		if(log_fd){
			dprintf(log_fd, "%s", instr);
		}
		shut_down();
	}
	else{
		fprintf(stderr, "Invalid command\n" );
		if(log_fd){
			dprintf(log_fd, "Invalid command\n");
		}
		mraa_aio_close(tmpr);
		//mraa_gpio_close(button);
		exit(1);
	}

}

int main(int argc, char *argv[]){
	int c;
	//int option_index = 0;
	static struct option long_options[] = {
		{"id", required_argument, NULL, 'i'},
		{"host", required_argument, NULL, 'h'},
		{"log", required_argument, NULL, 'l'},
		{0,	0,	0,	0}
	};

	while(1){
		c = getopt_long(argc, argv, "i:h:l:", long_options, NULL);
		if(c == -1)
			break;
		switch(c){
			case 'i':
				if(strlen(optarg) != 9){
					fprintf(stderr, "Invaild ID\n");
					exit(1);
				}
				id = optarg;
				break;
			case 'h':
				host = optarg;
				break;
			case 'l':
				log_flag = 1;
				log_fd = creat(optarg, 0666);
				if(log_fd < 0){
					fprintf(stderr, "%s\n", strerror(errno));
					exit(1);
				}
				break;
			default:
				fprintf(stderr, "usage: \n\t\t[--id]\trequired id number\n\t\t[--host]\trequired host name\n\t\t[--log]\trequired log file name\n");
				exit(1);
		}
	}
	if(argc-optind == 1){
		port_num = atoi(argv[optind]);
	}
	else{
		fprintf(stderr, "Too many argument\n");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	struct hostent *server;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0){
		fprintf(stderr, "Error in socket\n");
		exit(2);
	}

	server = gethostbyname(host);
	if(server == NULL){
		fprintf(stderr, "Error in host\n");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_num);

	 if(connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    	perror("Error connecting");
    	exit(2);
    }

    //char msg[50];
    dprintf(sock_fd, "ID=%s\n", id);

	tmpr = mraa_aio_init(0);
	if(tmpr == NULL){
		fprintf(stderr, "AIO init error\n" );
		mraa_aio_close(tmpr);
		exit(2);
	}

	// button = mraa_gpio_init(3);
	// if(button == NULL){
	// 	fprintf(stderr, "GPIO init error\n" );
	// 	mraa_aio_close(tmpr);
	// 	mraa_gpio_close(button);
	// 	exit(1);
	// }

	// mraa_gpio_dir(button, MRAA_GPIO_IN);

	struct pollfd fd[1];
	fd[0].fd = sock_fd ;
	fd[0].events = POLLIN | POLLHUP | POLLERR;

	//int btn_flag;
	char str[128], buf[28];

	while(1){
		int rec = poll(fd, 1, 0);
		if ( rec == -1){
			fprintf(stderr, "Fail in poll()\n");
			mraa_aio_close(tmpr);
			//mraa_gpio_close(button);
			exit(2);
		}
		//printf("hi");
		//btn_flag = mraa_gpio_read(button);
		//printf("%d", btn_flag);
		// check if buttom is pushed
		// if(btn_flag > 0){
		// 	shut_down();
		// }

		get_temp();
 
		if(fd[0].revents & POLLIN){
			bzero(str, 128);
			bzero(buf, 28);
			int r = read(fd[0].fd, str, 128);
			if( r == 0){
				break;
			}
			int len = strlen(str);
			int i;
			int j = 0;
			for(i = 0; i < len; i++){
				if(str[i] == '\n'){
					buf[j] = str[i];
					command(buf);
					bzero(buf, 28);
					j = 0;
				}
				else if(str[i] == '\0'){
					break;
				}
				else{
					buf[j] = str[i];
					j++;
				}
			}
			//get_temp();
		}

		if(fd[0].revents & POLLERR){
			fprintf(stderr, "Error in reading\n" );
			mraa_aio_close(tmpr);
			//mraa_gpio_close(button);
			exit(2);
		}

		if(fd[0].revents & POLLHUP){
			break;
		}
	
	
}

	mraa_aio_close(tmpr);
	//mraa_gpio_close(button);
	return 0;

}


















