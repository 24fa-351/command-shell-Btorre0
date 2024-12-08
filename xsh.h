#ifndef XSH_H
#define XSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

struct xsh_variable {
    char *name;
    char *value;
    struct xsh_variable *next;
};

void xsh_setvar(char *name, char *value);
char *xsh_getvar(char *name);
void xsh_unsetvar(char *name);

void xsh_printvars(void);
void xsh_freevars(void);
char *xsh_expandvars(char *line);

#endif // XSH_H