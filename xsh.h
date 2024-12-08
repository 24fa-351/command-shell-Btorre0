#ifndef XSH_H
#define XSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <ctype.h>

struct xsh_variable {
    char *name;
    char *value;
    struct xsh_variable *next;
};

static struct xsh_variable *xsh_variables = NULL;
static void xsh_setvar(char *name, char *value);
static char *xsh_getvar(char *name);
static void xsh_unsetvar(char *name);

static void xsh_printvars(void);
static xsh_freevars(void);
static *xsh_expandvars(char *line);

#endif  // XSH_H