#ifndef EXEC_H
#define EXEC_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void exec_cd(char *path);
void exec_pwd(void);

#endif  // EXEC_H
