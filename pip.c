#include "pipe.h"
#include "exec.h"

static char **split_line(char *line) {
    char **tokens = malloc(64 * sizeof(char *));
    if (tokens == NULL) {
        perror("malloc");
        exit(1);
    }

    char *token = strtok(line, " ");
    int i = 0;
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return tokens;
}


static xsh_pipeline *parse_pipeline(char **pipeline) {
    xsh_pipeline *p = malloc(sizeof(xsh_pipeline));
    if (p == NULL) {
        perror("malloc");
        exit(1);
    }

    p->commands = malloc(64 * sizeof(char *));
    if (p->commands == NULL) {
        perror("malloc");
        exit(1);
    }

    int i = 0;
    while (pipeline[i] != NULL) {
        p->commands[i] = pipeline[i];
        i++;
    }
    p->commands[i] = NULL;

    p->infile = NULL;
    p->outfile = NULL;
    p->background = 0;

    return p;
}


static void free_xsh_pipeline(xsh_pipeline *pipeline){
    free(pipeline->commands);
    free(pipeline);
}
