#include <stdlib.h>
#include <stdio.h>
#include <string.h>
char*** copy_string(FILE *input){
	char * line = malloc(256);
	size_t size;
	char * init_pos;
	char ** first = malloc(sizeof(char*)*1024);
	char ** second = malloc(sizeof(char*)*1024);
	char ** third = malloc(sizeof(char*)*1024);
	int count = 0;
	while(getline(&line,&size,input)>-1){

		init_pos = line;
		char * tab = strchr(line,'\t');
		if(!tab) break;
		int s = tab - line +1;
		char * no_of_files = malloc(tab-line+1);
		strncpy(no_of_files,line,tab-line);
		no_of_files[tab-line] = '\0';
		first[count] = no_of_files;
		line=line+((tab-line)+1);
		char * n_line = strchr(line,'\n');
		char * url = malloc(n_line-line+1);
		strncpy(url,line,n_line-line);
		url[n_line-line] = '\0';
		second[count] = url;
		line = init_pos;

		getline(&line,&size,input);
		n_line = strchr(line,'\n');
		char * dir = malloc(n_line-line+1);
		strncpy(dir,line,n_line-line);
		dir[n_line-line] = '\0';
		third[count] = dir;
		line = init_pos;
		getline(&line,&size,input);
		getline(&line,&size,input);

		//
		// numberofinputs\t<url>\ntime modified\ndirectory\n

		count++;
	}

	first[count] = NULL;
	second[count] = NULL;
	third[count] = NULL;
	char *** ret = malloc(sizeof(char**)*4);
	ret[0] = first;
	printf("In extract count is %d\n",count );
	ret[1] = second;
	ret[2] = third;
	ret[3] = NULL;
	return ret;
}
