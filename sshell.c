#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define CMDLINE_MAX 512
void cdFunction(char* dirFiles){
	if(chdir(dirFiles)!=0)
		fprintf(stderr,"Error: cannot cd into directory\n");
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
    else 
    return 3 ; /* Returns 3 if its built-in cmd*/

}
char **spaceRemover(char* strCmd)
{
	int bufferSize = CMDLINE_MAX;
	int posIndex = 0;
	char **tokenArray= malloc(bufferSize * sizeof(char*));
	char *indivToken;
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
	return tokenArray;
}
char* gnu_getcwd(){
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
void pwdFunction(){
char* dirPath = NULL;
dirPath = gnu_getcwd();
printf("%s\n", dirPath);
}
int main(void)
{
	char **args;
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
                        break;
                }
		if(!strcmp(cmd,"pwd")){
			pwdFunction();
			continue;

		}

		int cmdType = findpipe(cmd);
		 if(cmdType == 1)
        	{
           		 printf("1");
       		 }
       		 if (cmdType == 2)
       		 {
           		 printf("2");

       		}
       		 if(cmdType == 3 )
       		 {
           		args =spaceRemover(cmd);
			if(!strcmp(args[0],"cd")){	               		cdFunction(args[1]);
			int status;
                        waitpid(pid, &status,0);
                        printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
		 	continue;	
		
		
			}


		 pid = fork();
                if(pid == 0) {
                        /* Child*/
                        execvp(args[0],args);
                        perror("execvp");
                        exit(1);
                } else if (pid > 0){
                        /* Parent */
                        int status;
                        waitpid(pid, &status,0);
                        printf("+ completed '%s' [%d]\n",cmd,
                                WEXITSTATUS(status));
                } else {
                        perror("fork");
                        exit(1);
                }

       		 }


        }

        return EXIT_SUCCESS;
}
