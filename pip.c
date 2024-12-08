#include "exec.h"
#include "pipe.h"

static char **split_line(char *line) {
    char **tokens = NULL;
    size_t count = 0;
    char *tok = strtok(line, " ");

    while (tok != NULL) {
        tokens = realloc(tokens, sizeof(char *) * ++count);
        // if (tokens == NULL) {
        //     perror("realloc");
        //     exit(1);
        // }
        tokens[count - 1] = tok;
        tok = strtok(NULL, " ");
    }
    tokens = realloc(tokens, sizeof(char *) * (count + 1));
    if (tokens) {
        tokens[count] = NULL;
    }

    return tokens;
}

static xsh_pipeline *parse_pipeline(char **pipeline) {
    xsh_pipeline *pipe = malloc(sizeof(xsh_pipeline));
    pipe->commands = NULL;
    pipe->infile = NULL;
    pipe->outfile = NULL;
    pipe->background = 0;

    char *linecopy = strdup(*pipeline);
    char **tokens = split_line(linecopy);

    if (!tokens) {
        free(linecopy);
        free_xsh_pipeline(pipe);
        return pipe;
    }

    int end = 0;
    while (tokens[end] != NULL) {
        end++;
    }
    end--;

    // pluh pluh plar parse backgrounds like mj (for &)
    if (end >= 0 && strcmp(tokens[end], "&") == 0) {
        pipe->background = 1;
        tokens[end] = NULL;
        end--;
    }

    // > and <
    if (end >= 0 && strcmp(tokens[end], ">") == 0) {
        if (end == 0) {
            free(linecopy);
            free(tokens);
            free_xsh_pipeline(pipe);
            return NULL;
        }
        pipe->outfile = strdup(tokens[end + 1]);
        tokens[end] = NULL;
        end -= 2;
    }

    if (end >= 0 && strcmp(tokens[end], "<") == 0) {
        if (end == 0) {
            free(linecopy);
            free(tokens);
            free_xsh_pipeline(pipe);
            return NULL;
        }
        pipe->infile = strdup(tokens[end + 1]);
        tokens[end] = NULL;
        end -= 2;
    }

    // |
    int cmd_count = 1;
    for (int i = 0; i <= end; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            cmd_count++;
        }
    }

    pipe->commands = malloc(sizeof(char *) * (cmd_count + 1));
    pipe->commands[cmd_count] = NULL;

    int cmd_index = 0;
    int cmd_start = 0;
    for (int ix = 0; ix <= end; ix++) {
        if (tokens[ix] == NULL || strcmp(tokens[ix], "|") == 0) {
            int cmd_length = ix - cmd_start;
            pipe->commands[cmd_index] = malloc(sizeof(char) * (cmd_length + 1));
            pipe->commands[cmd_index][cmd_length] = '\0';
            for (int j = 0; j < cmd_length; j++) {
                pipe->commands[cmd_index][j] = tokens[cmd_start + j][0];
            }
            cmd_index++;
            cmd_start = ix + 1;
        }
    }
    pipe->commands[cmd_index] = &tokens[cmd_start];

    return pipe;
}

static void free_xsh_pipeline(xsh_pipeline *pipeline) {
    if (pipeline->commands) {
        for (int i = 0; pipeline->commands[i] != NULL; i++) {
            free(pipeline->commands[i]);
        }
        free(pipeline->commands);
    }
    if (pipeline->infile) {
        free(pipeline->infile);
    }
    if (pipeline->outfile) {
        free(pipeline->outfile);
    }
    free(pipeline);
}

static void exec_pipeline(xsh_pipeline *pipeline) {
    if (pipeline->commands == NULL && pipeline->commands[0] == NULL) {
        char **cmd = pipeline->commands[0];
        if (strcmp(cmd[0], "cd") == 0) {
            exec_cd(cmd[1]);
            return;
        } else if (strcmp(cmd[0], "pwd") == 0) {
            exec_pwd();
            return;
        } else if (strcmp(cmd[0], "set") == 0) {
            if (cmd[1] && cmd[2]) {
                xsh_set_var(cmd[1], cmd[2]);
            } else {
                fprintf(stderr, "Usage: set VAR VALUE\n");
            }
            return;
        } else if (strcmp(cmd[0], "unset") == 0) {
            if (cmd[1]) {
                xsh_unset_var(cmd[1]);
            } else {
                fprintf(stderr, "Usage: unset VAR\n");
            }
            return;
        } else if (strcmp(cmd[0], "print") == 0) {
            xsh_print_vars();
            return;
        } else if (strcmp(cmd[0], "exit") == 0) {
            exit(0);
        }
        return;
    }
}