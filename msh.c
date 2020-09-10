// REQ 020
/*
    Joshua Tran
    1001296598
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size
// REQ 009
#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports ten arguments
// REQ 15
#define MAX_NUM_HISTORY 50      // allows shell to support 50 history items
#define clear() printf("\033[H\033[J") // helps keep things tidy in the shell on initialization()


// Not necessary, but cleans up view in terminal
// so that you can clearly see output made just by this mav shell
void initialization()
{
    clear();
}

// given from msf.c file from git hub
// takes in the max command size and token which is used to
// parse the cmd_str and turn it into an array of singular words/files/commands.
// then depending on what the first command taken in is, it will return:
// -1: used to deal with new lines if no input is given
// 0: exits program
// 1: changes directory
// 2: supposed to background a process
// 3: supposed to list process Ids of last 15 processes
// 4: supposed to list upto last 50 commands inputed
// 5: starts to execute commands in the bin directory
int takeInput(char *cmd_str, char **token)
{
    // REQ 001, 004
    // prints msh> and takes in input
    printf ("msh> ");
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
    int token_count = 0;                       
    char *arg_ptr;                                        
    char *working_str  = strdup( cmd_str );                
    char *working_root = working_str;
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
            (token_count<MAX_NUM_ARGUMENTS))
    {
        token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
        if( strlen( token[token_count] ) == 0 )
        {
            token[token_count] = NULL;
        }
        token_count++;
    }

    // REQ 006
    // handles taking in newlines
    if(token[0] == NULL)
    {
        free( working_root ); 
        return -1;
    }
    // REQ 005
    // handles quit and exit to leave shell
    else if((strcmp(token[0],"exit") == 0) || (strcmp(token[0], "quit")) == 0)
    {
        
        free( working_root ); 
        return 0;
    }
    // REQ 013
    // handles change of directory
    else if((strcmp(token[0],"cd")) == 0)
    {
        
        free( working_root ); 
        return 1;
    }
    // REQ 008 
    // supposed to background a process
    //// NOT FINISHED ////
    else if((strcmp(token[0],"bg")) == 0)
    {
        free( working_root ); 
        return 2;
    }
    // REQ 014 
    // supposed to list the process IDs of last 15
    //// NOT FINISHED ////
    else if((strcmp(token[0],"listpids")) == 0)
    {
        free( working_root ); 
        return 3;
    }
    // REQ 015
    // supposed to list last 50 commands
    //// NOT FINISHED ////
    else if((strcmp(token[0],"history")) == 0)
    {
        free( working_root ); 
        return 4;
    }
    // REQ 002, 003, 010
    else
    {
        free( working_root ); 
        return 5;
    }

    // just in case for some wierd reason it doesnt return above
    free( working_root );   
}

// REQ 002, 003, 010
// takes in the input from user as token, and forks to preform 
// the command taken in by using execvp to run commands from the bin directory.
// then waits for the child process to finish and returns nothing
void ExecuteCommand(char *token[])
{
    // REQ 011
   pid_t pid = fork();
   if(pid == 0) // for the child process to take action
   {
       // REQ 011
      if(execvp(token[0], token) == -1) // child process taking action from actions
      {                                 // from processes in the bin directories
         perror(token[0]); // error handeling
      }
      exit(EXIT_FAILURE);
   } 
   else if (pid < 0) // error handeling
   {
       perror(token[0]);
   }
   else 
   {
       // REQ 011
      wait(NULL); // waits for child process to finish
      return;
   }
}


void SignalCatcher(int sigNumber )
{
    // displays the signal number used for debug
    // printf("Signal: %d\nmsh> ", sigNumber);
}



int main()
{
    // clears everything from bash before the program
    initialization();
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    char *token[MAX_NUM_ARGUMENTS];
    
    // REQ 007, 012
    // dealing with signals
    struct sigaction act; // used to get signal handlers
    memset(&act, '\0', sizeof(act) ); // sets aside some memory for sigaction act
    act.sa_handler = &SignalCatcher; // helps grab which signal is hit, mainly
                                     // primary funciton was to help with debug for me
    if( sigaction (SIGINT, &act, NULL ) < 0)  // interupts signal, CTRL+C
    {
        perror(""); // error handeling  
    }
    if( sigaction (SIGTSTP, &act, NULL ) < 0)  // suspends process, CTRL+Z
    { 
        perror(""); // error handeling
    }
    
    while( 1 )
    {
        // takes in input, checks which commands were passed through,
        // then stores which command into Option
        int Option = takeInput(cmd_str,token);
        
        // REQ 005
        // Option 0 exits with status 0
        if(Option == 0)
        {
            printf("\nLeaving msh\n");
            exit(0);
        }
        // REQ 013
        // Option 1 changes directory
        else if(Option == 1)
        {
            chdir(token[1]);
            //printf("change directory hit\n");
        }
        // REQ 008
        // Option 2 throws a process into the background
        //// NOT FINISHED ////
        else if(Option == 2)
        {
            //printf("background process hit\n");
        }
        // REQ 014 
        // Option 3 supposed to list the process IDs of last 15
        //// NOT FINISHED ////
        else if(Option == 3)
        {
            //printf("listpids hit\n");
        }
        // REQ 015
        // Option 4 supposed to list last 50 commands
        //// NOT FINISHED ////
        else if(Option == 4)
        {
            //printf("history hit\n");
        }
        // REQ 010, 003,
        // Option 5 runs commands in the /bin
        else if(Option == 5)
        {
            ExecuteCommand(token);
        }
        
    }
    return 0;
}
