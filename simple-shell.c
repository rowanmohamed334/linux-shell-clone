#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_BUF_SIZE 128
int ctr;
char newString[30][30];
int flag = 0;


//child signal handler
void signal_handler (int sig) {
    FILE *fp;
    fp  = fopen ("data.log", "a");
    fprintf(fp, "\n Child process was terminated  \n");
    fclose(fp) ;
    return ;
}


//input adjustments
void removeStringCarriageReturn(char *str) {
	if (str == NULL)
		return;
	int length = strlen(str);
	if (str[length-1] == '\n')
		str[length-1]  = '\0';
}

//parsing input
char **splitString()
{
	char str1[200];
	int i,j;
	int buf_size = MAX_BUF_SIZE , pos = 0;
	char **command_arr = malloc(buf_size * sizeof(char*));

	printf(" Enter Your Command -> ");
	fgets(str1, sizeof str1, stdin);    
	j=0; ctr=0;
	for(i=0;i<=(strlen(str1));i++)
	{	
	// if space or NULL found, assign NULL into newString[ctr]
		if(str1[i]==' '||str1[i]=='\0')
		{
			newString[ctr][j]='\0';
			ctr++;
			j=0;  
		}
		else
		{
			newString[ctr][j]=str1[i];
			j++;
		}
	}
	
	removeStringCarriageReturn(newString[ctr-1]);

	//make an array for execv.
	if(!command_arr){
		printf("\nAllocation error!");
		exit(0);
		}
	for(int j = 0; j<ctr; j++){
		
		command_arr[pos] = newString[j];
		if(strcmp(command_arr[pos], "&") == 0){
		flag = 1;
		//printf("flag changed");
		}
		pos++;
		if(pos >= buf_size){
			buf_size += MAX_BUF_SIZE;
			command_arr = realloc(command_arr , buf_size * sizeof(char*));

			if(!command_arr){
				//printf("\nAllocation error!");
				exit(0);
					}
		}
	}
	command_arr[pos] = NULL; //make the last argument of the array NULL       
	return command_arr;
}

//commands execution
void executeeCommand(char **executable_command){

	
	pid_t returned_value = fork();
	if(returned_value < 0){
		//error occured
		printf("an error occurred\n");
	}
	else if(returned_value == 0){
		int result = execvp(executable_command[0], executable_command);
		if (result < 0){
			printf("execution failed\n");
			exit(0);
		}
	  	
	}
	else{
		if(flag){
			signal(SIGCHLD , signal_handler);
			flag = 0;
			
		}else{
			signal(SIGCHLD , signal_handler);
			int status;
			waitpid(returned_value , &status, 0);
		}
	}

	
}

//exit and NULL commands 
void testFunction(char **input_cm){
	if(input_cm[0] == NULL){
		exit(0);
	}else if(strcmp(input_cm[0], "exit") == 0){
		exit(0);
	}else{
		executeeCommand(input_cm);
	}
} 

//main function
int main()
{
	char **input_command;
	while(1){
		input_command = splitString();
		testFunction(input_command);
	}	 
	return 0;
}

