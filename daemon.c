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

#define _GNU_SOURCE

int main(){
    pid_t sid = 0;
	pid_t child = fork();
	
	if(child > 0){
		//parent process
		exit(EXIT_SUCCESS);
	}
	else if(child < 0){
		perror("Download Manager failed.\n");
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
					exit(EXIT_FAILURE);
				}

				struct sockaddr_in  server_addr;
				server_addr.sin_family = AF_UNIX;
				server_addr.sin_port = htons(2017);
				(server_addr.sin_addr).s_addr = htons(INADDR_ANY);
				
				int is_sock_bind = bind(server_sock,(struct sockaddr*)&(server_addr.sin_addr), sizeof(struct sockaddr_un));
				if(is_sock_bind == -1){
					perror("The file can not be downloaded.\n");
					exit(EXIT_FAILURE);
				}

				int can_listen = listen(server_sock,128);
				if(can_listen == -1){
					perror("The file can not be downloaded.\n");
					exit(EXIT_FAILURE);
				}

				struct sockaddr *incoming = NULL;
				socklen_t addr_len = sizeof(struct sockaddr_un);
				int is_accepted = accept(server_sock, incoming, &addr_len);

				if(is_accepted < 0){
					perror("The file can not be downloaded.\n");
					exit(EXIT_FAILURE);
				}
				
			   FILE *in = fopen("input.txt", w+);

			   while(is_accepted){
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

int extract_information(char **first, char **second, char **third, FILE *input){
					int count = 0;
					int c = 0;
					int  num_request = 0;

					//set the file_descriptor to the intial position					
					fseek(input, 0, SEEK_SET);

					//extract total number of requests
					if(r > 0){
						char temp[64];

						while((c = getchar(input)) != '\t' && c != EOF){
								temp[count] = c;
								count++;
						}

						temp[count] = '\0';
						num_request = atoi(temp);	
					}

					first = malloc(sizeof(char*) * (num_request + 1));
					second = malloc(sizeof(char*) * (num_request + 1));
					third =  malloc(sizeof(char*) * (num_request + 1));

					int f_count = 0;
					int s_count = 0;
					int t_count = 0;
					int total = 0;

					//if we can't allocate memory successfuly, just return -1.
					if(first == NULL || second == NULL || third == NULL){
							return -1;
					}

					//I assumed that urls or folder_name or timestap won't be of more than 256 bytes length
					while((c = getchar(input)) != EOF){
						char temp[256];
						count = 0;

						while(c != '\t' && c != EOF){
								temp[count] = c;
								count++;
								c = getchar(input);
						}

						temp[count] = '\0';
						char * t = malloc(count + 1);
						strcpy(t,temp);
							
						if(total%2 == 0){
							first[s_count++] = t;
						}
						else if(total%3 == 0){
							second[t_count++] = t;
						}
						else{
							third[f_count++] = t;
						}//else-if
						
						total++;
					}//while ends

					first[f_count] = NULL;
					second[s_count] = NULL;
					third[t_count] = NULL;
					
					return num_request;
}
