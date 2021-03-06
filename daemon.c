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
#include <errno.h>
#include "download_manager.h"
#define _GNU_SOURCE

/*
	Parses URL, download directory and time modified from the 
	given string.
	If time modified is "0", it is assumed that we dont
	want to update the file. If is it anything but "0", we will
	look for its updates and download only if the update
	is avalible.

	@return {
				ret[0] = array_of_urls
				ret[1] = array_of_directories
				ret[2] = array_of_time_modified
				ret[3] = NULL
			}
	Each of the sub-arrays retured (i.e ret[i]) will will NULL terminated.
	That is ret[ret.size()-1] == NULL
*/
char*** parse_string(char * c){
	int total = 0;
	sscanf(c,"%d\t",&total);		// get the total number of files
	//printf("Bytes: %d\n",total );
	char * * url = malloc(sizeof(char*)*total);
	char * * dir = malloc(sizeof(char*)*total);
	char * * time_modified = malloc(sizeof(char*)*total);
	url[0] = malloc(1024);
	dir[0] = malloc(1024);
	time_modified[0] = malloc(20);
	int i = 1;
	// scan the odd line into their respective
	// locations. Since the first line contains the number of files, it
	// requires a specail treatement. Rest, can be lumped into one category 
	// and handled in the loop.
	sscanf(c,"%d\t%s\n%s\n%s\n",&total,url[0],dir[0],time_modified[0]);		
	char * temp ;
	while(i<total){
		temp = strchr(c,'\n');		
		temp = strchr(temp+1,'\n');
		temp = strchr(temp+1,'\n');	// gets the last instance of '\n'
		c=c+((temp-c)+1);			// that describes that file							
		url[i] = malloc(1024);		// after this, a new file begins.
		dir[i] = malloc(1024);
		time_modified[i] = malloc(20);

		sscanf(c,"%s\n%s\n%s\n",url[i],dir[i],time_modified[i]);
		i++;
	}
	url[i] = NULL;
	dir[i] = NULL;
	time_modified[i] = NULL;
	char *** ret = malloc(sizeof(char**)*4);
	ret[0] = url;
	ret[1] = dir;
	ret[2] = time_modified;
	ret[3] = NULL;
	return ret;
	
}
/*
	download_durations is a NULL terminated array of double
	pointers containing the time taken for each download operation.
	Each double is followed by a new line character ('\n').
*/

void send_download_duration(int sock_fd,double ** download_durations){
	int i = 0;
	size_t bytes_written = 0;
	int w = 0;
	while(download_durations[i]){
		//printf("download_wrapper: %lf\n",*download_durations[i] );
		while(1){
			w = write(sock_fd,download_durations[i],sizeof(double));
			if(w < 0){
				return;
			}
			if(w > 0)
				bytes_written+=w;
			if(bytes_written==sizeof(double)){
				write(sock_fd,"\n",1);
				goto next_;
			}
		}
		next_:
		i++;
	}
	shutdown(sock_fd,SHUT_WR);
}

/*
	Forks a daemon process that takes in download requests from the 
	user.
	We assume that the string written to the socket will be no longer
	than 2048 bytes - make changes as necessary.

*/

int download_server(){
	pid_t sid = 0;
	pid_t child = fork();
			
	if(child >0){
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
		int server_sock = socket(AF_INET, SOCK_STREAM, 0);
		int option = 1;
		int ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
		if(server_sock==-1){
			perror("The file can not be downloaded.socket\n");
			perror("error code: socket() failed\n");
			exit(EXIT_FAILURE);
		}
		struct sockaddr_in  server_addr;
		memset(&server_addr, 0, sizeof(struct sockaddr_in));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(1234);
		(server_addr.sin_addr).s_addr = htons(INADDR_ANY);
		int is_sock_bind = bind(server_sock,(struct sockaddr*)(&server_addr), sizeof(struct sockaddr_in));
		if(is_sock_bind == -1){
			fprintf(stdout, "exiting at bind\n");
			perror("The file can not be downloaded.bind\n");
			perror("error code: bind() failed\n");
			exit(EXIT_FAILURE);
		}

		int can_listen = listen(server_sock,128);
		if(can_listen == -1){
			fprintf(stdout, "exiting at listen\n");
			perror("The file can not be downloaded.listen\n");
			perror("error code: listen() failed\n");
			exit(EXIT_FAILURE);
		}
		struct sockaddr_in incoming;
		socklen_t addr_len = sizeof(struct sockaddr);
		int is_accepted = accept(server_sock, (struct sockaddr*)&incoming, &addr_len);
		if(is_accepted < 0){
			fprintf(stdout,"accept has problems\n");
			perror("The file can not be downloaded.accept\n");
			perror("error code: accept() failed\n");
			exit(EXIT_FAILURE);
		}
	   	while(is_accepted >= 0){
			char buf[1024];
		  	ssize_t r = read(is_accepted, buf, 1024);
			while(r > 0){
				r = read(is_accepted, buf, 1024);
			}				
					
			char **array_folders;
			char **array_urls;
			char **array_times; 
			char *** info = parse_string(buf);
			array_urls = info[0];
			array_folders = info[1];
			array_times = info[2];
			// array containing the times taken for each download operation
			double ** download_durations = download_wrapper(array_folders,array_urls,array_times);
			// send the durations back to the GUI
			// so that it can we displayed to the
			// user
			send_download_duration(is_accepted,download_durations);
			if(is_accepted < 0){
				perror("The file can not be downloaded.\n");
				perror("error code: accept() failed\n");
				exit(EXIT_FAILURE);
			}
			is_accepted = accept(server_sock, (struct sockaddr*)&incoming, &addr_len);
	  	}
		close(server_sock);
	}		
	return 0;
}//main ends


int main(){
	download_server();
	return 0;
}


