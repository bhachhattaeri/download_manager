#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//set fd to the begining of the input filr
int set_file_to_begin(FILE *input){

					//set the file_descriptor to the intial position
					int tries = 0;					
					while(fseek(input, 0, SEEK_SET) != 0 && tries <5){
						tries++;
					}

					if(tries == 5){
						perror("extract_information failed.\n");
						perror("error code: feseek\n");
						return -1;
					}

					return 0;
}

//uses current position of the file descriptor. Input must not be NULL. input file must be open. 
int get_num_request(FILE *input){
					//extract total number of requests
					    if(input == NULL)
							return -1;

					    int c, count = 0; 
						char temp[64];

						while((c = fgetc(input)) != '\t' && c != EOF){
								temp[count] = c;
								count++;
						}

						temp[count] = '\0';
						int ret = atoi(temp);	
						return ret;
}


//free all the memory hold by the arrays. returns 0. 
int free_memory(char **first, char **second, char **third, int num){
	int count = 0;

	if(first != NULL){

		while(count < num){
			if(first[count] != NULL){
				free(first[count]);
			}

			count++;
		}

		free(first);
	
	}

	count = 0;
	if(second != NULL){

		while(count < num){
			if(second[count] != NULL){
				free(second[count]);
			}

			count++;
		}
	
		free(second);
	}
	
	count = 0;
	if(third != NULL){

		while(count < num){
			if(third[count] != NULL){
				free(third[count]);
			}

			count++;
		}

		free(third);
	}

	return 0;
}


//allocate memory for first, second, and third. return -1 on failure and return 0 on success.
int allocate_memory(char **first, char **second, char **third, int num){

					first = malloc(sizeof(char*) * (num + 1));
					second = malloc(sizeof(char*) * (num + 1));
					third =  malloc(sizeof(char*) * (num + 1));
	
					//if we can't allocate memory successfuly, just return -1.
					if(first == NULL || second == NULL || third == NULL){
							perror("extract_information failed.\n");
							perror("error code: allocate-array\n");
							free_memory(first, second, third, num);
							return -1;
					}

					return 0;

}

char* copy_next_string(FILE *input){
	char * line = malloc(256);
	size_t size;
	char * init_pos;
	while(getline(&line,&size,input)>0){
		init_pos = line;
		char * tab = strchr(line,'\t');
		char * no_of_files = malloc(tab-line+1);
		strncpy(no_of_files,line,tab-line);
		no_of_files[tab-line] = '\0';
		line+=(line-tab)+1;
		char * n_line = strchr(line,'\n');
		char * url = malloc(n_line-line+1);
		strncpy(url,line,n_line-line);
		url[n_line-line] = '\0';
		line = init_pos;
		getline(&line,&size,input);
		n_line = strchr(line,'\n');
		char * dir = malloc(n_line-line+1);
		strncpy(dir,line,n_line-line);
		dir[n_line-line] = '\0';
	}


		char * url_no_of_file = malloc(1024);
						char temp[256];
						
						int count = 0;
						int c = fgetc(input);

						while(c != '\t' && c != EOF){
								temp[count] = c;
								count++;
								c = fgetc(input);
						}

						temp[count++] = '\0';
						char * t = malloc(count + 1);

						if(t == NULL){
							perror("extract_information failed.\n");
							perror("error code: malloc-copy_string\n");
							return NULL;
						}

						strncpy(t,temp,count);
						t[count] = '\0';
						return t;
}

int extract_information(char ***first, char ***second, char ***third, FILE *input){
					int count = 0;
					int c = 0;
					int  num_request = 0;
					
					int set_begin = set_file_to_begin(input);
					if(set_begin < 0){
						return -1;
					}

					num_request = get_num_request(input);
					if(num_request == -1){
						return -1;
					}

					*first = malloc(sizeof(char*) * 1024);
					*second = malloc(sizeof(char*) * 1024);
					*third =  malloc(sizeof(char*) * 1024);					
					int f_count =0 , s_count = 0, t_count = 0, total = 0;
					printf("First is %p\n",*first );
					fseek(input,0,SEEK_SET);
					//I assumed that urls or folder_name or timestap won't be of more than 256 bytes length

					do{
						char *t = copy_next_string(input);
						
						if(total%2 == 0){
							printf("About to copy to first %s\n", t);
							(*first)[s_count] = t;
							s_count++;
						}
						else if(total%3 == 0){
							printf("About to copy to second\n");
							(*second)[t_count] = t;
							t_count++;
						}
						else{
							printf("About to copy to third %s\n",t);
							(*third)[f_count] = t;
							f_count++;
						}//else-if
						
						total++;
					}while((c = fgetc(input)) != EOF);//while ends
					printf("The val of total is %d\n",total );
					(*first)[f_count] = NULL;
					(*second)[s_count] = NULL;
					(*third)[t_count] = NULL;
					
					return num_request;
}

