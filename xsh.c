#include "xsh.h"

// adding/updating xsh variables
static void xsh_setvar(char *name, char *value) {
    struct xsh_variable *var = xsh_variables;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            free(var->value);
            var->value = strdup(value);
            return;
        }
        var = var->next;
    }

    struct xsh_variable *var = malloc(sizeof(struct xsh_variable));
    var->name = strdup(name);
    var->value = strdup(value);
    var->next = xsh_variables;
    xsh_variables = var;
}

// getting xsh variables
static char *xsh_getvar(char *name) {
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
static void xsh_unsetvar(char *name) {
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
static void xsh_printvars() {
    struct xsh_variable *var = xsh_variables;
    while (var != NULL) {
        printf("%s=%s\n", var->name, var->value);
        var = var->next;
    }
}

// freeing xsh variables
static void xsh_freevars() {
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
static char *xsh_expandvars(char *line) {
    char *expanded = malloc(strlen(line) * 2 + 1);
    if (expanded == NULL) {
        perror("malloc");
        exit(1);
    }
    size_t resulting = 0;

    for (size_t ix = 0; ix < strlen(line); ix++) {
        if (line[ix] == '$') {
            ix++;
            size_t jx = ix;
            while (line[jx] != '\0' && line[jx] != ' ') {
                jx++;
            }
            char *name = strndup(line + ix, jx - ix);
            char *value = xsh_getvar(name);
            free(name);
            if (value != NULL) {
                strcpy(expanded + resulting, value);
                resulting += strlen(value);
            } else {
                expanded[resulting] = '$';
                resulting++;
            }
            ix = jx - 1;
        } else {
            expanded[resulting] = line[ix];
            resulting++;
        }
    }
    expanded[resulting] = '\0';
    return expanded;
}

static void xsh_execute(char *line) {
    char *expanded = xsh_expandvars(line);
    system(expanded);
    free(expanded);
}