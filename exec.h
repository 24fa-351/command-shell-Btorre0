#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


// mplement "cd" and "pwd". (directly). cd must take a full or relative path.

void exec_cd(char *path);
void exec_pwd(void);

#endif  // EXEC_H