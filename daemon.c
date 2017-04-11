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
#include <errno.h>
#include "download_manager.h"
#include "extract_test.c"
#define _GNU_SOURCE

int download_server(){
				pid_t sid = 0;
				pid_t child = fork();
				
				if(child >0){
					//execlp("sleep","sleep",)
					exit(EXIT_SUCCESS);
				}
				else if(child < 0){
					exit(EXIT_SUCCESS);
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


				//close(STDIN_FILENO);
				//close(STDIN_FILENO);
				//close(STDERR_FILENO);
				FILE* output = fopen("output.txt", "ab+");
			//	printf("Hererere\n");
			//	int i = 0;
				//while(1){
					//fflush(stdout);
					//printf("In the xloop\n");
				//	sleep(1);
					//fprintf(stdout,"%d\n",i++ );
			//	}

				int server_sock = socket(AF_INET, SOCK_STREAM, 0);
				//printf("The socket number is %d\n",server_sock );
				int option = 1;
				int ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
				//if(ret<0)
				//	printf("return val is %d\n",ret);
				if(server_sock==-1){
				//	fprintf(stdout, "exiting at socket\n");
					perror("The file can not be downloaded.socket\n");
					perror("error code: socket() failed\n");
				//	printf("an error: %d\n",errno);
					//fclose(output);
					exit(EXIT_FAILURE);
				}


				struct sockaddr_in  server_addr;
				memset(&server_addr, 0, sizeof(struct sockaddr_in));
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(9110);
				(server_addr.sin_addr).s_addr = htons(INADDR_ANY);
				
				int is_sock_bind = bind(server_sock,(struct sockaddr*)(&server_addr), sizeof(struct sockaddr_in));
				if(is_sock_bind == -1){
					fprintf(stdout, "exiting at bind\n");
					fclose(output);
					perror("The file can not be downloaded.bind\n");
					perror("error code: bind() failed\n");
					exit(EXIT_FAILURE);
				}

				int can_listen = listen(server_sock,128);
				if(can_listen == -1){
					fprintf(stdout, "exiting at listen\n");
					fclose(output);
					perror("The file can not be downloaded.listen\n");
					perror("error code: listen() failed\n");
					exit(EXIT_FAILURE);
				}
				
				struct sockaddr_in incoming;
				socklen_t addr_len = sizeof(struct sockaddr);
				printf("pid = %d\n",getpid());
			//	printf("Hello World!\n");
				fprintf(stdout, "before accept\n");

				int is_accepted = accept(server_sock, (struct sockaddr*)&incoming, &addr_len);
			//	printf("hmjgakshgldfadhjkwq\n");
			//	fprintf(stdout, "after accept\n");
				if(is_accepted < 0){
					fprintf(stdout,"accept has problems\n");
					fclose(output);
					perror("The file can not be downloaded.accept\n");
					perror("error code: accept() failed\n");
					exit(EXIT_FAILURE);
				}
				fclose(output);	
					
					
			   	while(is_accepted > 0){
				   	FILE *in = fopen("/Users/Bhawesh/Desktop/input.txt", "w+");
					if(in == NULL){
						perror("extract_information failed.\n");
						perror("error code: fopen\n");
						exit(EXIT_FAILURE);
					}

					char buf[1024];
				    ssize_t r = read(is_accepted, buf, 1024);
					// printf("Bytes read = %zu",r);		
					 while(r > 0){
			
					 	ssize_t c = 0;
					    while(c < r){
							fputc(buf[c],in);
							c++;
						}

					 	r = read(is_accepted, buf, 1024);
			    	}				
					//buf = "THIS IS LINE ONE";		
					//fprintf(in, "%s",buf );
					char **array_folders;
					char **array_urls;
					char **array_times; 
					close(is_accepted);	
				//	printf("Before extract\n");
				//	printf("In daemon, buffer is %s\n",buf );
				//	char * str = malloc(1024);
				//	size_t siz;
				//	getline(&str,&siz,in);
				//	printf("THE LINE IS %s\n",str);
					//int did_extract = extract_information(&array_folders,&array_urls, &array_times,in);
					//					printf("First is %p\n",array_folders );
					fseek(in,0,SEEK_SET);
					char *** info = copy_string(in);
					array_urls = info[1];
					array_folders = info[2];
					array_times = NULL;
					printf("IS it NULL? %d\n",array_folders==NULL );
					//printf("The LINE IS: %s\n",str );
					//printf("After extrssssssssssact\n");
					//printf("%d\n",__LINE__ );
					printf("In daemon, file is %s\n", array_folders[0]);
					//printf("In daemon, time is %s\n", array_times[0]);
					printf("In daemon, url is %s\n", array_urls[0]);
					// int i = 0;
					// while(array_folders[i]){
					// 	printf("Dir: %s\n", array_folders[i]);
					// 	printf("URL: %s\n", array_urls[i]);
					// 	i++;
					// }
					printf("Succefully parse the input!\n" );
			   		//at the end
					download_wrapper(array_folders,array_urls,array_times);
					// if(did_extract < 0){
					// 	perror("The file can not be downloaded.\n");
					// 	exit(EXIT_FAILURE);
					// }
					//printf("Directory is %s\n",array_folders[0] );
					//printf("URL in daemon %s\n",array_urls[0] );
					//download_wrapper(array_folders,array_urls,1);
		//		fclose(test);
					if(is_accepted < 0){
						perror("The file can not be downloaded.\n");
						perror("error code: accept() failed\n");
						exit(EXIT_FAILURE);
					}
						is_accepted = accept(server_sock, (struct sockaddr*)&incoming, &addr_len);

			    }//while ends
					   	close(server_sock);

				//close(server_sock);
			}	//		}//else-ends
		
	
	return 0;
}//main ends
int main(){
	download_server();
	return 0;
}
//Contact GitHub API Training Shop Blog About
