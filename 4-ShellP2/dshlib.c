#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

void trim_whitespace(char *str) {
    // Skip leading whitespaces
    while (*str == SPACE_CHAR) {
        str++;
    }
    
    // Remove trailing whitespaces
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == SPACE_CHAR) {
        str[len - 1] = '\0';  
        len--;
    }
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd)
{
    if (cmd_line == NULL || cmd_buff == NULL) {
        return ERR_CMD_ARGS_BAD;
    }

    trim_whitespace(cmd_line);

    // If the command line is empty after trimming
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    clear_cmd_buff(cmd_buff);
    int argc = 0;
    int in_quotes = 0;
    char *start_of_argument;
    char *destination = cmd_buff->_cmd_buffer;

    while(cmd_line != NULL) {
        //reverse in_quotes flag when encountering quotes
        if (*cmd_line == '"') {
            in_quotes = !in_quotes; 
            cmd_line++;             
            continue;
        }

        // reached end of current argument
        if (isspace((unsigned char)*cmd_line) && !in_quotes) {
            
            //eliminate duplicate spaces
            while (isspace((unsigned char)*cmd_line)) {
                cmd_line++;
            }

            if (*cmd_line) {
                // Check if too many arguments
                if (argc >= CMD_MAX) {
                    return ERR_TOO_MANY_COMMANDS;
                }

                // Parse current argument
                strncpy(destination, start_of_argument, cmd_line - start_of_argument);
                destination[cmd_line - start_of_argument] = '\0';
                cmd_buff->argv[argc++] = destination;

                // Update pointers
                destination += (cmd_line - start_of_argument) + 1;
                start_of_argument = cmd_line;
            }
            continue;
        }

        cmd_line++;
    }

    // Handle any remaining argument
    if (start_of_argument < cmd_line) {
        if (argc >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
    
        // Parse current argument
        strncpy(destination, start_of_argument, cmd_line - start_of_argument);
        destination[cmd_line - start_of_argument] = '\0';
        cmd_buff->argv[argc++] = destination;
    }

    // Check for unbalanced quotes
    if (in_quotes) {
        return ERR_CMD_ARGS_BAD;
    }

    cmd_buff->argc = argc;
    cmd_buff->argv[arc] = NULL;
    return OK;
}


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff =  = malloc(sizeof(char) * SH_CMD_MAX);;
    int rc = 0;
    cmd_buff_t cmd;

    if (alloc_cmd_buff(cmd_buff) != OK) {
        return ERR_MEMORY;
    }

    while (1)
    {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // process exit command
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            exit(OK);
        }
        
        rc = build_cmd_buff(cmd_buff, &cmd);
        switch (rc) {
            case OK:
                Built_In_Cmds cmdType = match_command(cmd->argv[0]); 
                if(cmdType == BI_NOT_BI) {
                    exec_build_in_cmd(&cmd);
                } else {
                    exec_cmd(&cmd);
                }
                break;
            case WARN_NO_CMDS:
                printf(CMD_WARN_NO_CMD);
                break;
            case ERR_TOO_MANY_COMMANDS;:
                printf("error: too many arguments\n");
                break;
            default:
                printf("error: parsing command: %d\n", rc);
                break;
        }
    }

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    free(cmd_buff);
    free_cmd_buff(&cmd);
    return OK;
}
