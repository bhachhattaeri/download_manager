/*The purpose of this program is create a daemon process.I will use fork to create another process and then end the original process. Before we ends the parent, process we need to make the child process leader ofthe */
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
#include "extract.c"

#define _GNU_SOURCE

int download_server(){
    pid_t sid = 0;
	pid_t child = fork();
	
	if(child > 0){
		//parent process		
		exit(EXIT_SUCCESS);
	}
	else if(child < 0){
		perror("Download Manager failed.\n");
		perror("error code: fork failed");
		exit(EXIT_FAILURE);
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

			printf("My id is %d and my parent's Process id is %d.\n",getpid(),getppid());
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			//Daemon is created by the above codes. The following codes should read from the incoming TCP connection and do appropriate procedure.
				int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
				if(server_sock==-1){
					perror("The file can not be downloaded.\n");
					perror("error code: socket failed\n");
					exit(EXIT_FAILURE);
				}

				struct sockaddr_in  server_addr;
				server_addr.sin_family = AF_UNIX;
				server_addr.sin_port = htons(2017);
				(server_addr.sin_addr).s_addr = htons(INADDR_ANY);
				
				int is_sock_bind = bind(server_sock,(struct sockaddr*)&(server_addr.sin_addr), sizeof(struct sockaddr_un));
				if(is_sock_bind == -1){
					perror("The file can not be downloaded.\n");
					perror("error code: bind failed\n");
					exit(EXIT_FAILURE);
				}

				int can_listen = listen(server_sock,128);
				if(can_listen == -1){
					perror("The file can not be downloaded.\n");
					perror("error code: listen failed\n");
					exit(EXIT_FAILURE);
				}

				struct sockaddr_in incoming;
				socklen_t addr_len = sizeof(struct sockaddr_in);
				int is_accepted = accept(server_sock, (struct sockaddr_in *)&incoming, &addr_len);

				if(is_accepted < 0){
					perror("The file can not be downloaded.\n");
					perror("error code: accept failed\n");
					exit(EXIT_FAILURE);
				}
				
			   	FILE *in = fopen("input.txt", w+);
				if(in == NULL){
					perror("extract_information failed.\n");
					perror("error code: fopen\n");
					exit(EXIT_FAILURE);
				}

			   	while(is_accepted > 0){
					char buf[1024];
				    	ssize_t r = read(is_accepted, buf, 1024);
		
					while(r > 0){
						fwrite(in, buf, r);
						r = read(is_accepted, buf, 1024);
					}						
					

					char **array_folders;
					char **array_urls;
					char **array_times;
					close(is_accepted);	
					
					int did_extract = extract_information(array_folders, array_urls, array_times, input);
			   		//at the end
					if(did_extract < 0){
						perror("The file can not be downloaded.\n");
						exit(EXIT_FAILURE);
					}

					is_accepted = accept(server_sock, incoming, &addr_len);			
			   }//while ends


			}//else-ends
		
	
	return 0;
}//main ends

