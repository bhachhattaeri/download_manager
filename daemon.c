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

void kill_parent(pid_t id){
	kill(SIGTERM,id);
}
//What is a session leader?
//What is a group leader?


int main(){
    pid_t sid = 0;
	printf("My id is %d and my parent's Process id is %d.\n",getpid(),getppid());

	pid_t child = fork();
	
	if(child > 0){
		//parent process
		printf("My id is %d and My parent's Process id is %d.\n",getpid(),getppid());
		kill_parent(getpid());
	}
	else if(child < 0){
	    //fork failled.print error message
		perror("Download Manager failed.\n");
	}
	else{
		//fork successful. We created a child process. Once the main process ends, the child process will become daemon.
		
            chdir("/");
		    printf("My id is %d and My parent's Process id is %d.\n",getpid(),getppid());
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			free(command);
			command = NULL;

			while(1){
				int server_sock = socket(AF_LOCAL,SOCK_STREAM,0);			
				
			}
		}
	
	return 0;
}

}i
