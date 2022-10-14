# Simple Shell
### Authors: Nivedita Amanjee and Erol Bostan

# Design Choices in Main

Our main function idea was to create a system that parses the user's command line input into subcategories based on the presence of pipes, redirects, and built in commands. We implemented this idea through string searching and tokenizing using the _memchr()_ and _strtok()_ built-in functions. First, we check for basic built in commands such as pwd and exit, taking in the entire command line input as an argument. The user input that include further arguments are handling in the built-in commands portion of the code, such as the cd function. The pwd and cd command implementation is explained in "PWD and CD functions" below.  

Then we called the findPipe() function, setting it equal to an integer that reflects the type of subcategory of commands found. The subcategories and their implementation are listed below under "Finding the Type of Command". 

Once that block of code has found the type of character reflecting pipe, input redirection, output redirection, or built in command, it continues to execute functions based on the subcategory of command. Each of them are explained below in "Pipe Implementation", "Input and Output Redirect Implementation", and "Built In Command Implementation". Our code currently handles a single pipe, single redirection, and piping and redirection separately. 

Error handling is also explained in the "Error Handling" section. Furthermore, testing is explained in the "Testing" section. 

## PWD and CD Functions


### PWD 
1. The condition that searches for "pwd" in the user input calls the _pwdFunction()_.

2. _The pwdFunction()_ returns void, as its purpose is to simply print the current directory path. 

3. This function calls the *gnu_getcwd()* function, which returns a string of the current working directory. This string is then printed by the _pwdFunction()_.
⋅⋅⋅The *gnu_getcwd()* function has been adopted from the GNU C Library. The ⋅⋅⋅citation is provided below. 

### CD
1. The _cdFunction()_ is called when the initial command line string search matches "cd".

2. This function uses the _chdir()_ function to change the current directory byt taking in the second element of the command line input array as an argument.

3. The function returns an error if the user cannot cd into the specific directory. Usually this error exists if the user inputs a "/" in front of the directory. The directory has to be names without any slashes in the input. 

## Finding The Type of Command
Type of commands are found using _memchr_ with the delimeters _"|,>,<, "_ in the function _findPipe()_. The return integer is set to the cmdType variable in main. 
1. cmdType = 1, means the command is a pipe.

2. cmdType = 2, means the command is a output redirection.

3. cmdType = 3, means the command is a builtin function.

4. cmdType = 4, means the command is an input redirection


### Pipe Implementation
 1.  First the command line is parsed using the _spaceRemover()_ function, which parses the string input based on the space delimiter and tokenizes the commands, returning an array of strings of commands. 

 2. Then each error is handled, whether it be missing command or mislocated input/output redirection.

 3. Then the _pipeParser()_ function is called, taking in a pre-declared array of two elements which will hold the commands to the left and right of the pipe.

 4. _pipespaceRemover()_ function is called twice to strip each command.

 5. Then the _Pipe()_ function is called with each stripped pipe. This function implements the built-in functions _pipe()_, _fork()_, _dup2()_,_execvp()_ and _waitpid()_ to implement a pipe of two commands using two forks. 

 
 ### Input and Output Redirection
 1. The output and input redirection blocks of code in the main function first checks for errors such as too many process arguments, missing commands, or no output files. 

 2. Then it parses the command line using the _redirectHandler()_ function, which separates the input string based on spaces and redirect symbols, such as < >.

 3. Lastly, this function implements the built-in functions _fork()_, _dup2()_,_execvp()_ and _waitpid()_ to implement a redirect from one file to another. 

### Built In Command Implementation 

1. First, the command line is parsed into an array of string tokens using the space character as a delimiter, using the _spaceRemover()_ function. 

2. Then the following errors are searched: "too many process arguments" or "Command not found". These errors are written into the stderr. 

3. Lastly, standard Commands like ls and cat are forked and executed via _execvp()_ with cmdType = 3 in _main()_.
⋅⋅1. Cd, pwd functions are handled seperately within by calling functions.
⋅⋅2. All of the other command line functions are handled by the same structure.

## Error Handling
1. Error handling has been executed based on specific errors as are further elucidated below: 

⋅⋅1. "Missing Command" 

⋅⋅⋅This error is handled when searching the command line arguments for a lack ⋅⋅⋅of command in the beginning of the input or surrounding any pipes or ⋅⋅⋅redirects. If a command doesn't exist when it is supposed to, the error ⋅⋅⋅handler block prints "Error: missing command" to the stderr. 

⋅⋅2. "Too many process arguments"

⋅⋅⋅This error is handled when the command line argument array length exceeds ⋅⋅⋅16. The error is printed to the stderr. 

⋅⋅3. "no input file" or "cannot open input file"

⋅⋅⋅The no input file error is handled by checking for arguments in the array ⋅⋅⋅before and after the presence of a redirect. If no arguments exist, then the ⋅⋅⋅input does not contain an input file. Therefore the error is written to ⋅⋅⋅stderr. The cannot open input file error is handled by checking if the file ⋅⋅⋅exists using the _open()_ built-in function as such: *if((fd=open(args[1],⋅⋅⋅O_WRONLY,0644))==-1)* and then writing the error to stderr.

⋅⋅4. "Mislocated output redirection"

⋅⋅⋅This error is returned when there exists a redirect within the pipe ⋅⋅⋅commands in the block of code where cmdType equals 1 (pipe matching). Our ⋅⋅⋅code doesn't currently handle chaining of pipes and redirects. 

2. Other errors in code
⋅⋅⋅Other errors, such as from using built-in functions like _strcmp()_ and ⋅⋅⋅_waitpid()_ have been handled using a variable to store the status of the ⋅⋅⋅operation. The status of the exit of the operation is then returned using ⋅⋅⋅the following code: *printf("+completed'%s'[%d]\n",cmd,WEXITSTATUS(status));* 

## Testing 

Testing has been implemented using the test cases in the project description and from CSIF using tester.sh. 

## Citation

1. Citation for gnu_getcwd() function: 
⋅⋅⋅<u>Authors</u>: Sandra Loosemore, Richard M. Stallman, Roland McGrath Andrew 
⋅⋅⋅Oram, and Ulrich Drepper
⋅⋅⋅<u>Date Last Updated</u>: August 1, 2022
⋅⋅⋅<u>Title of Program/Source Code</u>: Working Directory. Function used: char *getcwd(char *buffer, size_t size). Function name: char* gnu_getcwd(). 
⋅⋅⋅<u>Code Version</u>: c99
⋅⋅⋅<u>Type</u>: GNU Reference Manual Website. 
⋅⋅⋅<u>Web Address or Publisher</u>: [Website] (https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_node/libc_258.html)

