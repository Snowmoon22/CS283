#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;

    // check for empty command line
    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    // tokenize by pipe
    char *token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Skip leading whitespaces
        while (*token == SPACE_CHAR) {
            token++;
        }

        // Remove trailing whitespaces
        size_t len = strlen(token);
        while (len > 0 && token[len - 1] == SPACE_CHAR) {
            token[len - 1] = '\0';  
            len--;
        }

        if (strlen(token) == 0) {
            return WARN_NO_CMDS;
        }

        // Split the token into the executable and arguments
        char *exe_end = strchr(token, SPACE_CHAR);
        if (exe_end != NULL) {
            *exe_end = '\0';  
            exe_end++;        
        }

        if (strlen(token) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (exe_end != NULL && strlen(exe_end) >= ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        
        // Add to clist
        strncpy(clist->commands[clist->num].exe, token, EXE_MAX);
        if (exe_end != NULL) {
            strncpy(clist->commands[clist->num].args, exe_end, ARG_MAX);
        } else {
            clist->commands[clist->num].args[0] = '\0'; 
        }

        clist->num++;
        token = strtok(NULL, PIPE_STRING);
    }
    return OK;
}