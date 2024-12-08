#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "exec.h"
#include "pipe.h"
#include "xsh.h"

// create a shell loop (xsh#) that reads a line from stdin and executes it.
// The loop should exit when the user types "exit".

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while (1) {
        printf("xsh# ");
        linelen = getline(&line, &linecap, stdin);
        if (linelen == -1) {
            break;
        }

        if (linelen > 0 && line[linelen - 1] == '\n') {
            line[linelen - 1] = '\0';
        }

        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            break;
        }

        char *expanded = xsh_expandvars(line);
        if (!expanded[0]) {
            free(expanded);
            continue;
        }

        char *tmpline = expanded;
        xsh_pipeline *p = parse_pipeline(&tmpline);

        exec_pipeline(p);
        free_xsh_pipeline(p);

        free(expanded);
    }

    free(line);
    xsh_freevars();
    return 0;
}