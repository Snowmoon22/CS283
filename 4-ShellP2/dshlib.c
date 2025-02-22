#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(sizeof(char) * SH_CMD_MAX);

    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    cmd_buff->argc = 0;

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    free(cmd_buff->_cmd_buffer);
	for (int i = 0; i < CMD_ARGV_MAX; i++) {
        if(cmd_buff->argv[i]) free(cmd_buff->argv[i]);
	}
	return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        if(cmd_buff->argv[i]) {
            free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;
        }
    }

    cmd_buff->argc = 0;
    
    return OK;
}

// Helper functions for build_cmd_buff
int is_space(char c) {
    return isspace((unsigned char)c);
}

int is_quote(char c) {
    return c == '"' || c == '\'';
}

void trim_whitespace(char *str) {
    while (*str == SPACE_CHAR) {
        str++;
    }
    
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == SPACE_CHAR) {
        str[len - 1] = '\0';  
        len--;
    }
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
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
    char *start = cmd_line;

    while(*start != '\0') {
        // Eliminate duplicate whitespace
        while (is_space(*start)) {
            start++;
        }

        if (is_quote(*start)) {
            in_quotes = 1;
            start++;
        }

        char *end = start;
        while (*end && (in_quotes || !is_space(*end))) {
            if (is_quote(*end)) {
                in_quotes = 0;
                break;
            }
            end++;
        }

        if (argc >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        int len = end - start;
        cmd_buff->argv[argc] = malloc(sizeof(char) * len - 1);
        if (!cmd_buff->argv[argc]) {
            return ERR_MEMORY;
        }

        strncpy(cmd_buff->argv[argc], start, len);
        cmd_buff->argv[argc][len] = '\0';
        argc++;

        start = end;
        if (is_quote(*start)) start++; 
    }

    cmd_buff->argc = argc;
    cmd_buff->argv[argc] = NULL;
    return OK;
}

Built_In_Cmds match_command(const char *input) {

    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    } 
    if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }

    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);

    switch (cmd_type) {
        case BI_CMD_EXIT:
            exit(0);
        case BI_CMD_DRAGON:
            print_dragon();
            return BI_EXECUTED;
        case BI_CMD_CD:
            if(cmd->argc == 1) {
                break; // do nothing
            }
            if(cmd->argc > 2) {
                return ERR_EXEC_CMD;;
            }
            if(chdir(cmd->argv[1])) {
                return ERR_EXEC_CMD;;
            }
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }

    return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd) {
    if (cmd == NULL || cmd->argc < 1) {
        return ERR_CMD_ARGS_BAD;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return ERR_EXEC_CMD;
    }

    // Child process
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp failed");
        exit(ERR_EXEC_CMD); 
    } 

    // Parent process
    int status; 
	waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        if (exit_status == 0) {
            return OK; // Command executed successfully
        } else {
            return ERR_EXEC_CMD; // Command failed
        }
    } 

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
    char *cmd_buff = malloc(sizeof(char) * SH_CMD_MAX);;
    int rc = 0;
    cmd_buff_t cmd;

    if (alloc_cmd_buff(&cmd) != OK) {
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

        rc = build_cmd_buff(cmd_buff, &cmd);
        switch (rc) {
            case OK:
                Built_In_Cmds cmdType = match_command(cmd.argv[0]); 
                if(cmdType == BI_NOT_BI) {
                    exec_cmd(&cmd);
                } else {
                    exec_built_in_cmd(&cmd);
                }
                break;
            case WARN_NO_CMDS:
                printf(CMD_WARN_NO_CMD);
                break;
            case ERR_TOO_MANY_COMMANDS:
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
