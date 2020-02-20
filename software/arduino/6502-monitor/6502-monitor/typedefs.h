#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

typedef struct {
    void (*short_press)();
    void (*long_press)();
} switch_functions_t;

#endif