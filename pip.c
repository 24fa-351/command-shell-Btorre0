#include "exec.h"
#include "pipe.h"

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
    // maybe make this into an if
    tokens = realloc(tokens, sizeof(char *) * (count + 1));
    tokens[count] = NULL;
    return tokens;
}

static xsh_pipeline *parse_pipeline(char **pipeline) {
    xsh_pipeline *p = malloc(sizeof(xsh_pipeline));

    return p;
}

static void free_xsh_pipeline(xsh_pipeline *pipeline) {
    free(pipeline->commands);
    free(pipeline);
}

static void exec_pipeline(xsh_pipeline *pipeline) {}