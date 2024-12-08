#ifndef PIPE_H
#define PIPE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>
#include <ctype.h>


#include "exec.h"
#include "xsh.h"
typedef struct {
    char **commands;
    int ncmds;
    char *infile;
    char *outfile;
    int background;
} xsh_pipeline;

xsh_pipeline *parse_pipeline(char *pipeline);
void free_xsh_pipeline(xsh_pipeline *pipeline);
void exec_pipeline(xsh_pipeline *pipeline);

#endif  // PIPE_H