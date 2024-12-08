#include "exec.h"

void exec_cd(char *path) {
    if (path == NULL || *path == '\0') {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
        } else {
            if (chdir(home) == -1) {
                perror("cd");
            }
        }
    } else {
        if (chdir(path) == -1) {
            perror("cd");
        }
    }
}

void exec_pwd(void) {
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("pwd");
        return;
    } else {
        printf("%s\n", cwd);
        free(cwd);
    }
}