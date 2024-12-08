// Beatriz Torres Archundia
// CPSC 351 - 08
// 12.07.2024
// btorre0@csu.fullerton.edu
// Github Username: Btorre0

#include "xsh.h"

#include "pipe.h"
#include "xsh.h"

// Global variable to store xsh variables
struct xsh_variable *xsh_variables = NULL;

// adding/updating xsh variables
void xsh_setvar(char *name, char *value) {
    struct xsh_variable *var = xsh_variables;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            free(var->value);
            var->value = strdup(value);
            return;
        }
        var = var->next;
    }
    var = malloc(sizeof(struct xsh_variable));
    var->name = strdup(name);
    var->value = strdup(value);
    var->next = xsh_variables;
    xsh_variables = var;
}

// getting xsh variables
char *xsh_getvar(char *name) {
    struct xsh_variable *var = xsh_variables;
    while (var != NULL) {
        if (strcmp(var->name, name) == 0) {
            return var->value;
        }
        var = var->next;
    }
    return NULL;
}

// removing xsh variables
void xsh_unsetvar(char *name) {
    struct xsh_variable **var = &xsh_variables;

    while (var != NULL) {
        if (strcmp((*var)->name, name) == 0) {
            struct xsh_variable *tmp = *var;
            *var = (*var)->next;
            free(tmp->name);
            free(tmp->value);
            free(tmp);
            return;
        }
        var = &(*var)->next;
    }
}

// printing xsh variables (maybe delete)
void xsh_printvars(void) {
    struct xsh_variable *var = xsh_variables;
    while (var != NULL) {
        printf("%s=%s\n", var->name, var->value);
        var = var->next;
    }
}

// freeing xsh variables
void xsh_freevars(void) {
    struct xsh_variable *var = xsh_variables;
    while (var != NULL) {
        struct xsh_variable *next = var->next;
        free(var->name);
        free(var->value);
        free(var);
        var = next;
    }
    xsh_variables = NULL;
}

// expanding xsh variables
// !!!!!!!COME BACK!!!!!!!!!!
char *xsh_expandvars(char *line) {
    char *expanded = malloc(strlen(line) * 2 + 1);
    if (expanded == NULL) {
        perror("malloc");
        exit(1);
    }
    size_t resulting = 0;
    size_t len = strlen(line);

    for (size_t ix = 0; ix < len; ix++) {
        if (line[ix] == '$') {
            ix++;
            size_t start = ix;
            while (ix < len &&
                   (isalnum((unsigned char)line[ix]) || line[ix] == '_')) {
                ix++;
            }
            size_t var_len = ix - start;
            char *name = strndup(line + start, var_len);
            char *value = xsh_getvar(name);
            free(name);
            if (value) {
                size_t vlen = strlen(value);
                memcpy(expanded + resulting, value, vlen);
                resulting += vlen;
            }
            ix--;
        } else {
            expanded[resulting++] = line[ix];
        }
    }
    expanded[resulting] = '\0';
    return expanded;
}
