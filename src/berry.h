#ifndef __BERRY_H
#define __BERRY_H

#include <stddef.h>
#include <stdint.h>

#define BERRY_DEBUG             0

typedef uint8_t         bbyte;
typedef double          breal;
typedef int             bint;
typedef uint32_t        binstruction; /* instruction */

typedef enum {
    bfalse = 0,
    btrue = 1
} bbool;

typedef struct bvm bvm;

#define BERRY_VERSION   "0.0.1"

#endif
