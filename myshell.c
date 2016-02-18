/* RCS information: $Id: myshell.c,v 1.2 2006/04/05 22:46:33 elm Exp $ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

extern char **get_line();

void sig_handler(int signo)                            //signal handler for "^C"
{
	if( signo == SIGINT)                               //SIGINT refers to "^C"
	{
		printf("\nRecieved SIGINT\n");
	}
}

int normal( char ** args )                             //function for execution of commands without any I/O redirection and pipeing
{                                                      //args - arguments list
	int pid;
	pid = fork();                                      //creating child process
	if ( pid < 0 ) {
	    perror("\nChild not created.\n");
	    fflush(stdout);
	    return -1;
	}
	else if( pid == 0 )                                //child process
	{
		if(strcmp(args[0], "cd") == 0)
		{
			exit(0);
		}
		execvp(args[0], args);                         //executing command
		perror("\ncommand not working\n");
		fflush(stdout);
		exit(0);
	}
	else                                               //parent process
	{
		if(signal(SIGINT, sig_handler ) == SIG_ERR )   //call "^C" signal handler
			printf("\nCouldn't handle sigint\n");
		if(strcmp(args[0], "cd") == 0)
		{
			chdir(args[1]);                            //executing cd command
		}
		wait(NULL);
	}

	return 0;
}

int output_redirection( char ** args , int i , int n)  //args - arguments list, i - index of output redirection,n - no. of arguments
{
	int j;
	int out;
	int pid;
	pid_t pid1;
	char *new_args[i+1];
	for( j = 0 ; j < i ; j++ )                         //separating command to be executed.
	{
		new_args[j] = args[j];
	}
	new_args[i] = NULL;
	n = n - ( i + 2 );
	printf(" Remaining arguments %d ",n );
	pid = fork();                                      //forking
	if ( pid <0 ) {
	    perror("\nChild not created.\n");
	    fflush(stdout);
	    return -1;
	}
	else if( pid == 0 )                                //child process
	{
		out = open(args[i+1] , O_CREAT|O_WRONLY|O_TRUNC , 0644);//opening output file
		close(1);                                      //closing standard output
		dup(out);                                      //duplicate out
		execvp(args[0],new_args);
		perror("\ncommand not working\n");
		fflush(stdout);
		exit(0);
	}
	else                                               //parent process
	{
		wait(NULL);
	}
	return 0;
}

int input_redirection( char ** args , int i, int n )
{
	int j;
	char *name;
	int in;
	int pd[2];
	pid_t pid,pid1;
	int out;
	char *new_args[i+1];
	int status;
	status = pipe(pd);
	if ( status < 0 ) {
	    perror("\nPipe not created.\n");
	    fflush(stdout);
	    return -1;
	}
	for( j = 0 ; j < i ; j++ )
	{
		new_args[j] = args[j];
	}
	new_args[i] = NULL;
	char *new_arg[n-i];
	printf("n is %d",n);
	for( j = i+1 ; j < n ; j++ )
	{
		new_arg[j-i-1] = args[j];
		printf("\n new args %d is %s\n",j,new_arg[j-i-1]);
		fflush(stdout);
	}
	n = n - (i+2);
	printf(" Remaining arguments %d ",n );
	pid = fork();
	if ( pid < 0 ) {
	    perror("\nChild not created.\n");
	    fflush(stdout);
	    return -1;
	}
	else if( pid == 0 )
	{
		printf("print");
		fflush(stdout);
		if ( n != 0 )
		{
			printf("Entering");
			int decision = deciding(new_arg);
			int position = place(new_arg);
			printf("decision is %d",decision);
			printf("position is %d",position);
			fflush(stdout);
			if( decision == 2 )
			{
				if( position == 1 )
				{
					pid1 = fork();
					if ( pid <0 ) {
	                    perror("\nChild not created.\n");
	                    fflush(stdout);
	                    return -1;
	                }
					else if( pid1 == 0 )
					{
						in = open(args[i+1] ,  O_RDONLY );
						close(0);
						dup(in);
						dup(pd[0]);
						close(pd[0]);
						out = open( args[i+3] , O_CREAT|O_WRONLY|O_TRUNC , 0644 );
						close(1);
						dup(out);
						execvp(args[0],new_args);
						perror("\ncommand not working\n");
						fflush(stdout);
						exit(0);
					}
					else
					{
						close(pd[0]);
						wait(pid1);
					}
				}
			}
		}
		else
		{
			in = open(args[i+1] ,  O_RDONLY );
			close(0);
			dup(in);
			execvp(args[0],new_args);
			perror("\ncommand not working\n");
			fflush(stdout);
			exit(0);
		}
	}
	else
	{
		wait(NULL);
	}
	return 0;
}
int pipes( char ** args , int i, int n )

{

	int j;

	int start = 0;

	int decision = 3;

	int store = 0;
	int status;
	int pid[10];                                    // for different process
	int count = 0;
	int old_i = -1;
	int flag = 0;

	int fd[10][2];                                  // different pipes
	while( i > 0 )

	{
		char *left[i-old_i];

		char *right[n-i];

		status = pipe(fd[count]);                   // status of the pipe

		for( j = old_i+1 ; j < i ; j++ )            // seperating the left side and right side arguments

		{

			left[j-old_i-1] = args[j];
			printf("\n Left args %d is %s\n",j,left[j-old_i-1]);

		}

		left[i] = NULL;                             // assigning the last pointer to NULL

		printf("n is %d",n);
		if( flag != 1 )                             // for last argument pipe
		{

			for( j = i+1 ; j < n ; j++ )

			{

			right[j-i-1] = args[j];                 // seperating the right side arguments

			printf("\n Right args %d is %s\n",j,right[j-i-1]);

			fflush(stdout);

			}

			right[n-i-1] = NULL;
			old_i = i;
			i = place(right);
			i = i + old_i + 1;
		}
		if( flag == 1 )                             // falg tells that LAST command became left side argument
		{
			i = -1;
		}
		if( i == n )
		{
			flag = 1;
		}
		//printf("\n decision is %d\n",decision);

		pid[count] = fork();                           //create a child

		if( pid[count] == 0 )

		{
			if( count == 0 )                            // for opening command
			{
				close(fd[0][0]);
			}
			else                                        // for middle pipes
			{
				close(0);
				dup(fd[count-1][0]);
				close(fd[count-1][0]);
			}

			if( i == -1 )                                  // for last commands
			{
				close(fd[count][1]);
			}
			else                                            // for middle commands
			{
				close(1);
				dup(fd[count][1]);
				close(fd[count][1]);
			}

			for( j = 0 ; j <= count ; j++ )                 // for closing all the pipes
			{
				close(fd[j][0]);
				close(fd[j][1]);
			}

			execvp(left[0],left);                           // executing the command


		}

		else

		{
			if( i != -1 )
			{
				count++;
				continue;
			}

			for( j = 0 ; j <= count ; j++ )
			{
				close(fd[j][0]);
				close(fd[j][1]);
			}
			for( j = 0 ; j <= count ; j++ )
			{
				waitpid(pid[j],NULL,0);
			}

		}

	}

	return 0;

}
int deciding( char ** args )
{
	int decision = 0;
	int i = 0;
	for(i = 0; args[i] != NULL; i++)                   //traverse the arguments list
    {
      if( *args[i] == '<' )
      {
      	decision = 1;                                  //input redirection
      	break;
      }
      else if( *args[i] == '>' )
      {
      	decision = 2;                                  //output redirection
      	break;
      }
      else if( *args[i] == '|' )
      {
      	decision = 3;                                  //pipeing
      	break;
      }
    }
    return decision;
}

int place( char ** args )
{
	int decision = 0;
	int i = 0;
	for(i = 0; args[i] != NULL; i++)                   //traverse the arguments list
    {
      if( *args[i] == '<' )
      {
      	decision = 1;                                  //input redirection
      	break;
      }
      else if( *args[i] == '>' )
      {
      	decision = 2;                                  //output redirection
      	break;
      }
      else if( *args[i] == '|' )
      {
      	decision = 3;                                  //pipeing
      	break;
      }
    }
    return i;
}

int main() {
  int i;
  char **args;
  int j;
  while(1)                                             //infinite loop ends with key combination "^C" or exit command
  {
  	printf("\nShell$ ");
    args = get_line();
    int decision = 0;                                  //function decider
    int n = 0;                                         //number of arguments
    for(i = 0; args[i] != NULL; i++)                   //printing arguments
    {
    	printf("Argument %d: %s\n", i, args[i]);
    	n++;
    }
    if( args[0] == NULL )
    {
    	continue;
    }
    printf("\n The number of arguments is %d\n",n);
    if( strcmp(args[0],"exit") == 0 )                  //if command is exit
    {
    	exit(0);
    }
    decision = deciding(args);                         //decides if command contains file input or output redirection or pipeing operations
    i = place(args);                                   //and returns its index of occurrence
    printf("The place is %d\n",i);
    if( decision  == 0 )                               //call normal function
    {
    	printf("\nEntering normal\n");
    	normal(args);
    }
    else if( decision == 1 )                           //call input redirection function
    {
    	printf("\nEntering input\n");
    	input_redirection(args,i,n);
    }
    else if( decision == 2 )                           //call output redirection function
    {
    	printf("\nEntering output\n");
    	output_redirection(args,i,n);
    }
    else if( decision == 3 )                           //call pipeing function
    {
    	pipes(args,i,n);
    }
  }

}
