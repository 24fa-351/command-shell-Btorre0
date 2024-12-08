#ifndef PIP_H
#define PIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char **commands;
    char *infile;
    char *outfile;
    int background;
} xsh_pipeline;

static char **split_line(char *line);
static xsh_pipeline *parse_pipeline(char **pipeline);
static void free_xsh_pipeline(xsh_pipeline *pipeline);
static void exec_pipeline(xsh_pipeline *pipeline);

#endif  // PIP_H