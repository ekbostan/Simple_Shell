#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CMDLINE_MAX 512

int arrsize = 0;

void Pipe(char** firstPipe, char** secondPipe,char* cmd)
{
        int pipes[2];
        int status1;
        pid_t p1,p2;
	 pipe(pipes);
        
        p1 = fork();
        if(p1 == 0)
        {
                close(pipes[0]);
                dup2(pipes[1],STDOUT_FILENO);
                close(pipes[1]);
                execvp(firstPipe[0],firstPipe);
                fprintf (stderr,"Error: Command not found\n");
                exit(1);

        }
        else if(p1 != 0 )
        {
                p2 = fork();
                if(p2 == 0)
                {
                        close(pipes[1]);
                        dup2(pipes[0],STDIN_FILENO);
                        close(pipes[0]);
                        execvp(secondPipe[0],secondPipe);
                        fprintf (stderr,"Error: Command not found\n");
                        exit(0);
                }

                else if (p2 != 0)
                {
                        waitpid(p1, &status1,0);
                        //waitpid(p2, &status2,0);
                        printf("+ completed '%s' [%d][%d]\n",cmd,
                        WEXITSTATUS(status1), WEXITSTATUS(status1));
                }


	}
}	

void pipespaceRommever(char* parsedPipes,char** firstPipe){

        int parsedPipecounter = 0;
        char* delim = " ";
        char *indivToken;
        indivToken = strtok(parsedPipes,delim);
	while(indivToken !=NULL){
		firstPipe[parsedPipecounter] = indivToken;
		parsedPipecounter++;
		indivToken = strtok(NULL,delim);
	}
	firstPipe[parsedPipecounter] = NULL;
}

void pipeParser(char** parsedPipes, char* cmd )
{
        char* dupCmd;
        dupCmd = strdup(cmd);
        int parseCounter = 0;
        for (parseCounter = 0; parseCounter < 2; parseCounter++)
        {
                parsedPipes[parseCounter] = strsep(&dupCmd,"|");
                if(parsedPipes[parseCounter] == NULL)
                break;       
        }
}

int cdFunction(char* dirFiles)
{
	if(chdir(dirFiles)!=0)
	{
	fprintf(stderr,"Error: cannot cd into directory\n");
	return 1;
	}
return 0;
}
int findpipe(char* userInput)
{
    char* ret;
    ret = memchr(userInput, '|', strlen(userInput));
    if (ret != NULL)
    {
        return 1 ;/* Returns 1 if its a pipe*/
    }
     ret = memchr(userInput, '>', strlen(userInput));
    if (ret != NULL)
    {
        return 2 ;/* Returns 2 if its a redirect*/
    }
   ret = memchr(userInput, '<', strlen(userInput));
    if (ret != NULL)
    {
        return 4 ;/* Returns 2 if its a redirect*/
    }
    else 
    return 3 ; /* Returns 3 if its built-in cmd*/
}
char **spaceRemover(char* strCmd)
{
	int bufferSize = CMDLINE_MAX;
	char **tokenArray= malloc(bufferSize * sizeof(char*));
	char *indivToken;
	 int posIndex=0;	
	char* delim = " ";
	char* dupCmd;
	dupCmd = strdup(strCmd);
	if (tokenArray == NULL){
	perror("malloc");
	exit(2);	
			}	
	indivToken = strtok(dupCmd,delim);
	while(indivToken !=NULL){
		tokenArray[posIndex] = indivToken;
		posIndex++;
		indivToken = strtok(NULL,delim);
	}
	tokenArray[posIndex] = NULL;
	arrsize = posIndex;
	return tokenArray;
}
char **redirectHandler(char* strCmd)
{
        int bufferSize = CMDLINE_MAX;
        char **tokenArray= malloc(bufferSize * sizeof(char*));
        char *indivToken;
        int posIndex=0;
        char* delim = " > <";
        char* dupCmd;
        dupCmd = strdup(strCmd);
        if (tokenArray == NULL){
        perror("malloc");
        exit(2);
                        }
        indivToken = strtok(dupCmd,delim);
        while(indivToken !=NULL){
                tokenArray[posIndex] = indivToken;
                posIndex++;
                indivToken = strtok(NULL,delim);
        }
        tokenArray[posIndex] = NULL;
        arrsize = posIndex;
        return tokenArray;
}
char* gnu_getcwd()
{
	size_t size = 100;
	while (1)
   	 {
  	 	char *buffer = malloc(sizeof(char*)*size);
     		 if (getcwd (buffer, size) == buffer)
       			 return buffer;
     		 free (buffer);
     		 if (errno != ERANGE)
       		 return 0;
     		 size *= 2;
   	}
}
void pwdFunction()
{
	char* dirPath = NULL;
	dirPath = gnu_getcwd();
	printf("%s\n", dirPath);
}

int main(void)
{
	char **args;
        char* parsedPipes[2];
        char* firstPipe[CMDLINE_MAX]; 
        char* secondPipe[CMDLINE_MAX];
        char cmd[CMDLINE_MAX];
	pid_t pid;
        while (1) {
                char *nl;
                /*int retval;*/
                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);
                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);
                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                /* Builtin command */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
			int status =0;
                        printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
                        break;
                }
		if(!strcmp(cmd,"pwd")){
			pwdFunction();
			 int status = 0;
                        printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
			continue;

		}

		int cmdType = findpipe(cmd);
		 if(cmdType == 1)
        	{
			args = spaceRemover(cmd);
           		 if(*args[0] == '|')
			 {
			 fprintf(stderr,"Error: missing command\n");
			 continue;		 
			}
			 char* ret;
   			 ret = memchr(cmd, '>', strlen(cmd));
   			 if (ret != NULL)
  			  {
  			  	 fprintf(stderr,"Error: mislocated output redirection\n");
                        	 continue;
 			   }
			 char* ret1;
                         ret1 = memchr(cmd, '<', strlen(cmd));
                         if (ret1 != NULL)
                          {
                                 fprintf(stderr,"Error: mislocated input redirection\n");
                                 continue;
			  }
                         pipeParser(parsedPipes,cmd);
                         pipespaceRommever(parsedPipes[0],firstPipe);
                         pipespaceRommever(parsedPipes[1],secondPipe);
			 if(secondPipe[0] == NULL )
			 {
				fprintf(stderr,"Error: missing command\n");
                                 continue;
			 }
                         Pipe(firstPipe,secondPipe,cmd);
       		 }
       		 if (cmdType == 2)
       		 {
           		args = spaceRemover(cmd);
			if(arrsize > 16)
			{
                        	fprintf(stderr,"Error: too many process arguments\n");
                        	continue;
                        }
			if(*args[0] == '>')
			{
				fprintf(stderr,"Error: missing command\n");
				continue;
			}
			if(*args[arrsize-1] == '>')
			{
				fprintf(stderr,"Error: no output file\n");
				continue;
			}
			args = redirectHandler(cmd);
			int fd;
 			if ((fd = open(args[arrsize-1],O_WRONLY | O_CREAT |O_TRUNC , 0644)) == -1)
			{   
                                fprintf(stderr,"Error: cannot open output file\n");
                                continue;
			}
			 pid = fork();
                         if(pid == 0) 
			 {
				dup2(fd, STDOUT_FILENO);
				close(fd);
				args[arrsize-1] = NULL;
				execvp(args[0],args);
                                fprintf (stderr,"Error: Command not found\n");
                                exit(1);
       			} 
			else if (pid > 0)
			{
                                /* Parent */
                                int status;
                                waitpid(pid, &status,0);
                                printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
                        } 
			else 
			{
                                perror("fork");
                                 exit(1);
                         }
		 }

		if (cmdType == 4)
		{
			args = spaceRemover(cmd);
			if(arrsize > 16)
			{
                        	fprintf(stderr,"Error: too many process arguments\n");
                        	continue;
                        }
                        if(*args[0] == '<')
                        {
                        	fprintf(stderr,"Error: missing command\n");
                        	continue;
                        }

                       if(*args[arrsize-1] == '<')
                       {

                        	fprintf(stderr,"Error: no input file\n");
                                continue;
			}
			args = redirectHandler(cmd);
                        int fd;
			if(!strcmp(args[0],"grep"))
			{
                        	if ((fd = open(args[2],O_WRONLY , 0644)) == -1)
				{
                                	fprintf(stderr,"Error: cannot open input file\n");
                                	continue;
				}
			}

			if ((fd = open(args[1],O_WRONLY , 0644)) == -1)
			{
                                fprintf(stderr,"Error: cannot open input file\n");
                                continue;
			}

                         pid = fork();
                         if(pid == 0) 
			 {
                                dup2(fd, STDIN_FILENO);
                                close(fd);
                                execvp(args[0],args);
                                fprintf (stderr,"Error: Command not found\n");
                                 exit(1);
                	} 
			else if (pid > 0)
			{
                                /* Parent */
                                int status;
                                waitpid(pid, &status,0);
                                printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
                        } 
			else 
			{
                                perror("fork");
                                 exit(1);
                         }
		}

       		 if(cmdType == 3 )
       		 {
           		args =spaceRemover(cmd);
			if(arrsize > 16)
			{
				fprintf(stderr,"Error: too many process arguments\n");
				continue;
			}
			if(!strcmp(args[0],"cd"))
			{
				int status = cdFunction(args[1]);		
				printf("+ completed '%s' [%d]\n",cmd,status);
		 		continue;	
		
               		 }


			if(!strcmp(args[0]," "))
			{	 	
				fprintf (stderr,"Error: Command not found\n");
                       		exit(1);
               		 }
			 pid = fork();
               		 if(pid == 0) 
			 {
                        	/* Child*/
                       		execvp(args[0],args);
                       		fprintf (stderr,"Error: Command not found\n");
                       		exit(1);
               		 } 
			 else if (pid > 0)
			 {
                       		/* Parent */
                        	int status;
                       		waitpid(pid, &status,0);
                        	printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
                	} 
			else 
			{
                        	perror("fork");
                       		 exit(1);
                	}
       		 }
        }

        return EXIT_SUCCESS;
}
