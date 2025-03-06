#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

//memory allocation and clean up
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
    cmd_buff->_cmd_buffer = NULL;

	for (int i = 0; i < CMD_ARGV_MAX; i++) {
        if(cmd_buff->argv[i]) {
            free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;
        }
	}

    cmd_buff->argc = 0;

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

int free_cmd_list(command_list_t *cmd_list) {
	for (int i = 0; i < cmd_list->num; i++) {
		free_cmd_buff(&cmd_list->commands[i]);
	}

	cmd_list->num = 0;
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

// Helper functions
char *trim_spaces(char *str) {
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
    return str;
}

int is_pipe(char c) {
    return c == PIPE_CHAR;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) return ERR_CMD_ARGS_BAD;

    // Trim leading and trailing spaces
    cmd_line = trim_spaces(cmd_line);
    if (*cmd_line == 0) return WARN_NO_CMDS;

    // Initialize the command list
    clist->num = 0;

    char *start = cmd_line;
    char *end = cmd_line;
    int in_quotes = 0;
    char quote_char = 0;

    while (*start) {
        // Skip leading spaces
        while (is_space(*start)) start++;

        if (*start == 0) break;

        // Handle quoted strings
        if (is_quote(*start)) {
            in_quotes = 1;
            quote_char = *start;
            start++;
        }

        end = start;
        while (*end && (in_quotes || !is_pipe(*end))) {
            if (is_quote(*end) && *end == quote_char) {
                in_quotes = 0;
                quote_char = 0;
                end++;
                break;
            }
            end++;
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        if (alloc_cmd_buff(&clist->commands[clist->num]) != OK) {
            return ERR_MEMORY;
        }

        // Copy the command into the command buffer
        int len = end - start;
        strncpy(clist->commands[clist->num]._cmd_buffer, start, len);
        clist->commands[clist->num]._cmd_buffer[len] = '\0';

        // Build the command buffer (parse arguments)
        if (build_cmd_buff(clist->commands[clist->num]._cmd_buffer, &clist->commands[clist->num]) != OK) {
            return ERR_CMD_ARGS_BAD;
        }

        clist->num++;
        start = end;
        if (is_pipe(*start)) start++; 
    }

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
            free_cmd_buff(cmd);
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

int execute_pipeline(command_list_t *clist) {
    if (!clist || clist->num == 0) return WARN_NO_CMDS;

    int num_commands = clist->num;

    int pipes[num_commands - 1][2];  // Array of pipes
    pid_t pids[num_commands]; // Array to store process IDs          

    // Create pipes for the pipeline
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    // Create processes for each command
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {  // Child process
            // Set up input pipe for all except first process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            // Set up output pipe for all except last process
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
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
    char *cmd_buff;
    int rc = 0;
    command_list_t clist;

    while (1)
    {
        cmd_buff = malloc(sizeof(char) * SH_CMD_MAX);
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        rc = build_cmd_list(cmd_buff, &clist);
        
        free(cmd_buff);

        switch (rc) {
            case OK:
                if (clist.num == 1) {
                    Built_In_Cmds type = match_command(clist.commands[0].argv[0]);
                    if (type != BI_NOT_BI) {
                        exec_built_in_cmd(&clist.commands[0]);
                    } else {
                        exec_cmd(&clist.commands[0]);
                    }
                } else {
                    execute_pipeline(&clist);
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

        free_cmd_list(&clist);
    }
    return OK;
}
