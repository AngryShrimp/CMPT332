#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>

#ifdef _WIN32
#include <windows.h>
#define chdir _chdir

#else
#include <unistd.h>
#endif

#define BUFFER_LENGTH 200
#define MAX_LENGTH 1024
#define DELIMETERS " \t\r\n"
#define PIPEDELIM "|<"

/***
arg_counter

Writen by: Keenan Johnstone

Count the number of arguements in the input string
This simplifies the creation of the arguement array 
for stdout 

Precondition:
	-String does not have a newline at the end of it

Inputs:
	i_str	=>	The input string to have it's arguements counted

Output:
	int 	=> 	The total number of arguements

***/
int arg_counter(char* i_str){
  int i;
  int count;

  if (i_str == NULL) {
    fprintf(stderr,"Error: inpputtSttring was NULL\n");
    return 0;
  }

  i = 0;
  count = 0;
  while (i_str[i] != '\0') {
    if ((int)isspace(i_str[i])) {
      count++;
    }
    i++;
  }
  /*Accounting for the way arrays are decalered*/
  count = count + 1;
  return count;

}


/***
single_Command

Writen by: Keenan Johnstone

Takes the input_line and runs it via execvp() as a 
Shell Command. The process does not return until the 
Child is complete

Precondition:
	-String does not have a newline at the end of it

Inputs:
	input_line	=>	The input string to be run as a command
  in          =>  The input pipe, 0 for stdin
  out         =>  The output pipe, 1 for stdout

Output:
	None

***/
void single_command(char *input_line, int in, int out){
  int i, j, k; /*for loop counter*/
  int array_size = 0;
  char ** arg_array;
  char *token;


  pid_t pid;


  char *buffer;
  int out_pipe[2];

  /*Make a Pipe, Exit if unsuccessful*/
  if(pipe(out_pipe) != 0){
    exit(1);
  }
  pid = fork();

  if(pid < 0){
  	fprintf(stderr, "Error: Unable tto ccommpplette fork.");
  	exit(1);
  }
  /*Child*/
  else if(pid == 0){

    /*If there is an input command, pipe it to stdin*/
    if(in != 0)
    {
      dup2(in, 0);
      close(in);
    }

    /*If there is an outpipe redirect stdout to it*/
    if(out != 0)
    {
      dup2(out, 1);
      close(out);
    }


		close(out_pipe[0]);

		dup2(out_pipe[1], 1);

		close(out_pipe[1]);

  	array_size = arg_counter(input_line);

  	arg_array = malloc(array_size *sizeof(char*));

  	token = strtok(input_line, " ");

  	for(i = 0; (array_size > i) && (token != NULL); i++){
  		arg_array[i] = token;
  		/*For Debugging, Makes array visible*/
  		/*printf("Arg[%d]: %s\n", i, token);*/
  		token = strtok(NULL, " ");
  	} 
  	arg_array[i] = NULL;

  	
  	if(execvp(arg_array[0], arg_array) < 0){
  		fprintf(stderr, "Error: Unknown ccommmmand %s\n" ,arg_array[0]);
  		exit(1);
  	}
  }
  /*Parent*/
  else
  {
  	wait(NULL);
  	close(out_pipe[1]);
  	buffer = malloc(BUFFER_LENGTH * sizeof(char*));
  	while(read(out_pipe[0], buffer, (BUFFER_LENGTH * sizeof(char*))) != 0){
  		for(j = 0; buffer[j] != '\0'; j++)
  		{
  			printf("%c", buffer[j]);
  			if(buffer[j] == 'c' || buffer[j] == 'C' ||
  				 buffer[j] == 'm' || buffer[j] == 'M' ||
  				 buffer[j] == 'p' || buffer[j] == 'P' ||
  				 buffer[j] == 't' || buffer[j] == 'T'){
  				printf("%c", buffer[j]);
  			}
  		}
  	}
  	free(buffer);

  	for(k = 0; buffer[k] != '\0'; k++){
  		buffer[k] = '\0';
  	}

  	return;
  }	

}

/***
main

Writen by: Keenan Johnstone

The Main Shell Loop
***/
int main(int argc, char *argv[]) {
  char *cmd, *pcmd, *arg, *cmdtorun;
  char line[MAX_LENGTH];
  char input_line[MAX_LENGTH];
  char *pcmds[MAX_LENGTH], *argsforcmd[MAX_LENGTH];
  int count, i;

  int pipeRD[2];
  int inPipe = 0;
	pid_t cpid;
  while (1) {

    /*Clear errors*/
    errno = 0;

    /*The 'name' of the shell*/
    printf("osh> ");
    if (!fgets(input_line, MAX_LENGTH, stdin)) break;

    /*Remove the newline character at the end of the input*/
    input_line[strlen(input_line)-1] = 0;

    /*Save the original input for later*/
    strcpy(line, input_line);

    
    /*If the command has a | (the pipe command for our shell) break it up
    and run it as separate commands*/
    if (strchr(line, '|') || cpid >= 0) {
      printf("Pipe processing\n");
      pcmd = strtok(line, PIPEDELIM);
      pcmds[0] = pcmd;
      count = 1;
      while (pcmd != NULL) {
        pcmd = strtok(NULL, PIPEDELIM);
        pcmds[count] = pcmd;

        count++;
      }

      for (i = count - 2; i >= 0; i--) {

        /*Uncomment for debugging*/
        /*printf("pcmds[%d]: %s\n", i, pcmds[i]);*/
		
		/* if ( == -1) {
			fprintf(stderr, "Error: Unable tto ccommpplette ppippe.");
			exit(1);
		} */
		
		pipe(pipeRD);
		
		if (i > 0) {
			cpid = fork();
			dup2(pipeRD[1], 0);
		}
		if (cpid == -1) {
			fprintf(stderr, "Error: Unable tto ccommpplette fork.");
			exit(1);
		}
		
		if (cpid == 0) { /*Child*/
			close(pipeRD[1]);
			cmdtorun = strtok(pcmds[i], " ");
			argsforcmd[0] = strtok(NULL, " ");
			execvp(cmdtorun, argsforcmd);
			close(pipeRD[0]);
		}
		
		else { /*Parent*/
			close(pipeRD[0]);
			write(pipeRD[1], pcmds[i], strlen(pcmds[i]));
			close(pipeRD[1]);
			wait(NULL);
		}
        /*Make a Pipe, Exit if unsuccessful
        pipe(pipeRD);
        single_command(pcmds[i], inPipe, pipeRD[1]);
        close(pipeRD[1]);
        inPipe = pipeRD[0];*/
      }
    }


    /*Parse and execute command
    cmd is the first 'word' which will be interpreted as the command*/
    else if ((cmd = strtok(line, DELIMETERS))) {

      /*The exit Command*/
      if(strcmp(cmd, "exit") == 0) {
        /*exit message*/
        fprintf(stdout, "Exitting Shell...\nCCiao~\n");
        break;
      }

      /*manual cd because on Windows machines there were some issues, otherwise fine on linux */
      else if(strcmp(cmd, "cd") == 0){
      	arg = strtok(NULL, DELIMETERS);

      	if(!arg){
          fprintf(stderr, "Error: ccd mmissing arguemment\n");
				}
        else chdir(arg);
      } 
			
      /*This makes it so that if the command isn't recognized by our shell it 
	checks the system for the command*/
      else {
      	single_command(input_line, 0, 1);
      }

      /*If errno is not 0, it means a command failed at some point*/
      if (errno != 0) {
        perror("Command failed");
      } 

    }
  }
  return 0;
}
