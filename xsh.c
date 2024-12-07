#include "xsh.h"

static void xsh_setvar(char *name, char *value) {
    struct xsh_variable *var = malloc(sizeof(struct xsh_variable));
    if (var == NULL) {
        perror("malloc");
        exit(1);
    }

    var->name = strdup(name);
    var->value = strdup(value);
    var->next = xsh_variables;
    xsh_variables = var;
}

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

static void xsh_unsetvar(char *name) {
    struct xsh_variable *var = xsh_variables;
    struct xsh_variable *prev = NULL;
    while (var != NULL) {
        if (strcmp(var->name, name) == 0) {
            if (prev == NULL) {
                xsh_variables = var->next;
            } else {
                prev->next = var->next;
            }
            free(var->name);
            free(var->value);
            free(var);
            return;
        }
        prev = var;
        var = var->next;
    }
}

static void xsh_printvars() {
    struct xsh_variable *var = xsh_variables;
    while (var != NULL) {
        printf("%s=%s\n", var->name, var->value);
        var = var->next;
    }
}


static void xsh_freevars() {
    struct xsh_variable *var = xsh_variables;
    while (var != NULL) {
        struct xsh_variable *next = var->next;
        free(var->name);
        free(var->value);
        free(var);
        var = next;
    }
}

static char *xsh_expandvars(char *line) {
    char *expanded = malloc(strlen(line) + 1);
    if (expanded == NULL) {
        perror("malloc");
        exit(1);
    }

    char *p = line;
    char *e = expanded;
    while (*p != '\0') {
        if (*p == '$') {
            p++;
            char *name = p;
            while (*p != '\0' && *p != ' ') {
                p++;
            }
            char save = *p;
            *p = '\0';
            char *value = xsh_getvar(name);
            if (value != NULL) {
                strcpy(e, value);
                e += strlen(value);
            }
            *p = save;
        } else {
            *e = *p;
            e++;
        }
        p++;
    }
    *e = '\0';
    return expanded;
}

static void xsh_execute(char *line) {
    char *expanded = xsh_expandvars(line);
    system(expanded);
    free(expanded);
}