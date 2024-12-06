#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec.h"

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

        if (strcmp(line, "exit\n") == 0) {
            break;
        }

        // remove the newline character
        line[linelen - 1] = '\0';

        if (strncmp(line, "cd ", 3) == 0) {
            exec_cd(line + 3);
        } else if (strcmp(line, "pwd") == 0) {
            exec_pwd();
        } else {
            system(line);
        }
    }

    free(line);
    return 0;
}