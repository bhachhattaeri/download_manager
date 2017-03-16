/*The purpose of this program is create a daemon process.I will use fork to create ano 
 *ther process and then end the original process. Before we ends the parent, process we need to make the child process leader ofthe 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>

#define _GNU_SOURCE
int main(){
    pid_t sid = 0;
	printf("My id is %d and my parent's Process id is %d.\n",getpid(),getppid());

	pid_t child = fork();
	
	if(child > 0){
		//parent process
		exit(EXIT_SUCCESS);
	}
	else if(child < 0){
		perror("Download Manager failed.\n");
	}
	else{	
  			umask(0);

			pid_t sid;
			sid = setsid();
			if(sid < 0){
				exit(EXIT_FAILURE);
			}
            
			if(chdir("/") < 0){
				exit(EXIT_FAILURE);
			}

			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			//Daemon is created by the above codes. The following codes should read from the incoming TCP connection and do appropriate procedure.

			while(1){
				int server_sock = socket(AF_UNIX,SOCK_STREAM,0);
				if(server_sock==-1){
					perror("The file can not be downloaded.\n");
					continue;
				}

				struct sockaddr_in  server_addr;
				server_addr.sin_family = AF_UNIX;
				server_addr.sin_port = htons(2017);
				(server_addr.sin_addr).s_addr = htons(INADDR_ANY);
				
				int is_sock_bind = bind(server_sock,(struct sockaddr*)&(server_addr.sin_addr), sizeof(struct sockaddr_un));
				if(is_sock_bind == -1){
					perror("The file can not be downloaded.\n");
					continue;
				}

				int can_listen = listen(server_sock,128);
				if(can_listen == -1){
					perror("The file can not be downloaded.\n");
					continue;
				}

				struct sockaddr *incoming = NULL;
				socklen_t addr_len = sizeof(struct sockaddr_un);
				int is_accepted = accept(server_sock, incoming, &addr_len);

				if(is_accepted < 0){
					perror("The file can not be downloaded.\n");
					continue;
				}

			}
		}
	
	return 0;
}
