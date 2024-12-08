#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exec.h"
#include "pipe.h"
#include "xsh.h"

static char **split_line(char *line) {
    char **tokens = NULL;
    size_t count = 0;
    char *tok = strtok(line, " ");

    while (tok != NULL) {
        tokens = realloc(tokens, sizeof(char *) * ++count);
        if (tokens == NULL) {
            perror("realloc");
            exit(1);
        }
        tokens[count - 1] = tok;
        tok = strtok(NULL, " ");
    }
    tokens = realloc(tokens, sizeof(char *) * (count + 1));
    if (tokens) {
        tokens[count] = NULL;
    }

    return tokens;
}

xsh_pipeline *parse_pipeline(char *pipeline) {
    xsh_pipeline *pipe = malloc(sizeof(xsh_pipeline));

    pipe->commands = NULL;
    pipe->ncmds = 0;
    pipe->infile = NULL;
    pipe->outfile = NULL;
    pipe->background = 0;

    char *linecopy = strdup(pipeline);
    char **tokens = split_line(linecopy);

    if (!tokens || !tokens[0]) {
        free(linecopy);
        free(tokens);
        free(linecopy);
        free(tokens);
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
    if (end > 0 && strcmp(tokens[end - 1], ">") == 0) {
        pipe->outfile = strdup(tokens[end]);
        tokens[end - 1] = NULL;
        tokens[end] = NULL;
        end -= 2;
    }

    if (end > 0 && strcmp(tokens[end - 1], "<") == 0) {
        pipe->infile = strdup(tokens[end]);
        tokens[end - 1] = NULL;
        tokens[end] = NULL;
        end -= 2;
    }

    // |
    int cmd_count = 1;

    for (int ix = 0; ix <= end; ix++) {
        if (strcmp(tokens[ix], "|") == 0) {
            cmd_count++;
        }
    }

    pipe->ncmds = cmd_count;
    pipe->commands = malloc(sizeof(char *) * (cmd_count + 1));
    pipe->commands[cmd_count] = NULL;

    int cmd_index = 0;
    int cmd_start = 0;

    for (int ic = 0; tokens[ic]; ic++) {
        if (strcmp(tokens[ic], "|") == 0) {
            tokens[ic] = NULL;
            pipe->commands[cmd_index] = &tokens[cmd_start];
            cmd_index++;
            cmd_start = ic + 1;
        }
    }
    pipe->commands[cmd_index] = &tokens[cmd_start];

    return pipe;
}

void free_xsh_pipeline(xsh_pipeline *pipeline) {
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

static void execute_command(char **cmd) {
    execvp(cmd[0], cmd);
    fprintf(stderr, "%s: command not found\n", cmd[0]);
    exit(127);
}


void exec_pipeline(xsh_pipeline *pipeline) {
    if (pipeline->ncmds == 1) {
        char **cmd = pipeline->commands[0];
        if (!cmd[0]) return;

        if (strcmp(cmd[0], "cd") == 0) {
            exec_cd(cmd[1]);
            return;
        } else if (strcmp(cmd[0], "pwd") == 0) {
            exec_pwd();
            return;
        } else if (strcmp(cmd[0], "set") == 0) {
            if (cmd[1] && cmd[2]) {
                xsh_setvar(cmd[1], cmd[2]);
            } else {
                fprintf(stderr, "Usage: set VAR VALUE\n");
            }
            return;
        } else if (strcmp(cmd[0], "unset") == 0) {
            if (cmd[1]) {
                xsh_unsetvar(cmd[1]);
            } else {
                fprintf(stderr, "Usage: unset VAR\n");
            }
            return;
        } else if (strcmp(cmd[0], "print") == 0) {
            xsh_printvars();
            return;
        } else if (strcmp(cmd[0], "exit") == 0) {
            xsh_freevars();
            exit(0);
        }
    }

    int pipe_count = pipeline->ncmds - 1;
    int(*fds)[2] = NULL;

    if (pipe_count > 0) {
        fds = malloc(sizeof(int[2]) * pipe_count);
        for (int ix = 0; ix < pipe_count; ix++) {
            if (pipe(fds[ix]) == -1) {
                perror("pipe");
                return;
            }
        }
    }

    pid_t *pids = malloc(sizeof(pid_t) * pipeline->ncmds);

    for (int ic = 0; ic < pipeline->ncmds; ic++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return;
        }
        if (pid == 0) {
            if (ic == 0 && pipeline->infile) {
                int infd = open(pipeline->infile, O_RDONLY);
                if (infd == -1) {
                    perror("open infile");
                    exit(1);
                }
                dup2(infd, STDIN_FILENO);
                close(infd);
            }

            if (ic == pipeline->ncmds - 1 && pipeline->outfile) {
                int outfd =
                    open(pipeline->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (outfd == -1) {
                    perror("open outfile");
                    exit(1);
                }
                dup2(outfd, STDOUT_FILENO);
                close(outfd);
            }

            if (ic > 0) {
                dup2(fds[ic - 1][0], STDIN_FILENO);
            }

            if (ic < pipeline->ncmds - 1) {
                dup2(fds[ic][1], STDOUT_FILENO);
            }
            if (fds) {
                for (int jx = 0; jx < pipe_count; jx++) {
                    close(fds[jx][0]);
                    close(fds[jx][1]);
                }
            }

            execute_command(pipeline->commands[ic]);
        } else {
            pids[ic] = pid;
        }
    }

    if (fds) {
        for (int jc = 0; jc < pipe_count; jc++) {
            close(fds[jc][0]);
            close(fds[jc][1]);
        }
        free(fds);
    }

    if (!pipeline->background) {
        for (int ib = 0; ib < pipeline->ncmds; ib++) {
            waitpid(pids[ib], NULL, 0);
        }
    }
    free(pids);
}