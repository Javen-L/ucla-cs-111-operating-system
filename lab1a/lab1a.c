// NAME:Bingxin Zhu
// EMAIL:bingxinzhu95@gmail.com
// ID:704845969

#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int shell_flag = 0;
int tempo = 0;
struct termios term_old;
struct termios term_new;
struct pollfd fds[2];
const char crlf[2] = {'\r', '\n'};
pid_t child_pid;
int pipe_ter_shell[2];	 
int pipe_shell_ter[2]; 


void rrestore();
//void read_write(int, int);
void read_write();

int main(int argc, char** argv)
{	
	tcgetattr(0, &term_old);	
	tcgetattr(0, &term_new);
	struct option opts[] =
    {
        {"shell", no_argument, NULL, 's'},
        {0, 0, 0, 0}
    };
    int choice;
    choice = getopt_long(argc, argv, "s", opts, &tempo);

    if(choice == 's')
    {
        shell_flag = 1;
    }
    
	else
	{
		if(argc > 1)
		{
			fprintf(stderr, "Error: you just input invaild argument!\n");
			exit(1);
		}
	}

	atexit(rrestore);
    
    
	char charbuf[1];
	term_new.c_lflag &= ~(ICANON | ECHO);
	term_new.c_cc[VMIN] = 1;
	term_new.c_cc[VTIME] = 0; 
	tcsetattr(0, TCSANOW, &term_new);
	
	if(shell_flag)
	{
		int tmp1 = pipe(pipe_ter_shell);
		int tmp2 = pipe(pipe_shell_ter);

		if (tmp1 < 0)
		{
			perror("Error: Failed construct pipe from terminal to shell\n");
			exit(1);
		}
		if (tmp2 < 0)
		{
			perror("Error: Failed construct pipe from shell to terminal\n");
			exit(1);
		}

		child_pid = fork();
		fds[0].fd = 0;
		fds[1].fd = pipe_shell_ter[0]; 
		fds[0].events = POLLIN | POLLHUP | POLLERR;
		fds[1].events = POLLIN | POLLHUP | POLLERR;

		if(child_pid == -1)
		{
			fprintf(stderr,"Error: can't fork!\n");
		}

		if (child_pid == 0)		//child process
		{
			close(pipe_shell_ter[0]);
			close(pipe_ter_shell[1]);
			dup2(pipe_ter_shell[0], 0);
			dup2(pipe_shell_ter[1], 1);
            dup2(pipe_shell_ter[1], 2);
            execl("/bin/bash", "bash", 0, (char *)0);
		}

		else 				//parent process
		{
			close(pipe_ter_shell[0]);
			close(pipe_shell_ter[1]);

			while(1)
			{
				poll(fds, 2, 0);
				if(fds[1].revents & (POLLHUP + POLLERR)) 	 //disconnected or error
				{
	       			fprintf(stderr, "Error: error in shell!\n");
	       			break;
	     		}
				if(fds[0].revents & (POLLHUP + POLLERR)) 	//disconnected or error
				{
	       			fprintf(stderr, "Error: error in terminal!\n");
	       			break;
	     		}

				if(fds[0].revents & POLLIN) //terminal typing
				{
					while(1)
					{
						if (read(0, charbuf, 1) < 0)
						{
							fprintf(stderr, "Error: can't read from keyboard!");
							tcsetattr(0, TCSANOW, &term_old); 	
							exit(1);
						}
						else
							write(pipe_ter_shell[1], charbuf, 1);

						if (charbuf[0] == '\n' || charbuf[0] == '\r')
						{	
							write(1, crlf, 2); 
							break;
						}
						else if (charbuf[0] == 0x03)	// ^C
						{
							kill(child_pid, SIGINT);
							tcsetattr(0, TCSANOW, &term_old); 	
						}

						else if (charbuf[0] == 0x04)	// ^D
						{
							close(pipe_ter_shell[1]);
							kill(child_pid, SIGHUP); 
							tcsetattr(0, TCSANOW, &term_old);
							exit(0);
						}
						else							//one at a time as normal
							write(1, charbuf, 1);
					}
				}
				if(fds[1].revents & POLLIN)	//shell sending
				{
					while(1)
					{
						if (read(pipe_shell_ter[0], charbuf, 1) > 0)
						{
							if(charbuf[0] == '\n'||charbuf[0] == '\r')	//<cr> or <lf> to <cr><lf>
							{
								write(1, crlf, 2);
								break;
							}
							else										// one at a time as normal
							{
								write(1, charbuf, 1); 
								break;
							}
						}
						else
						{
							exit(1);
						}
					}
				}
			}
		}
	}
	else
	{
        read_write();
	}

	return 0; 
}

void rrestore()
{
	int process_state = 0;

	if(shell_flag == 1) 
	{
		close(pipe_ter_shell[0]);
		close(pipe_ter_shell[1]);
		close(pipe_shell_ter[0]);
		close(pipe_shell_ter[1]);    
		waitpid(child_pid, &process_state, 0);

		if(WIFEXITED(process_state))
			fprintf(stderr, "SHELL EXIT STATUS=%d \n", WEXITSTATUS(process_state));
		else if(WIFSIGNALED(process_state))
			fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d \n", WTERMSIG(process_state), WEXITSTATUS(process_state));
		else
			fprintf(stderr, "SHELL EXIT \n");
	}
	else
		tcsetattr(0, TCSANOW, &term_old);
}

void read_write()
{
    ssize_t nchars;
    char charbuf[1];

    while((nchars= read(STDIN_FILENO, charbuf, 1))>0)
    {
        int i = 0;
        for(i=0; i<nchars; i++)
        {
            char iptchar=charbuf[i];
            
            if(iptchar=='\r' || iptchar=='\n')      // <cr> or <lf> to <cr><lf>
            {
                write(STDOUT_FILENO, crlf, 2);
                break;
            }
            
            else if(iptchar == 0x04)                // ^D:restore normal terminal modes and exit
            {
                tcsetattr(0, TCSANOW, &term_old);
                exit(0);
            }
            
            else if(iptchar == 0x03)                // ^C:use kill to send a SIGINT to the shell process.
            {
                tcsetattr(0, TCSANOW, &term_old);
            }
            
            else
                write(STDOUT_FILENO, charbuf, 1);
        }//for ends
    }
    
    if((read(STDIN_FILENO, charbuf, 1))<0)
    {
        fprintf(stderr, "Error: can't read from keyboard!\n");
        tcsetattr(0, TCSANOW, &term_old);
        exit(1);
    }

    return;
}
