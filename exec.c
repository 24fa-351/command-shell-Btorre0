#include "exec.h"

void exec_cd(char *path) {
    if (chdir(path) == -1) {
        perror("cd");
    }
}

void exec_pwd() {
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("pwd");
    } else {
        printf("%s\n", cwd);
        free(cwd);
    }
}