#ifndef __BE_CONSTSTR_H
#define __BE_CONSTSTR_H

#include "be_object.h"

typedef struct {
    const bstring **table;
    int count; /* string count */
    int size;
} bconststrtab;

#endif
