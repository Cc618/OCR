// Error and assertion functions

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

// Fatal error, prints MSG to stderr and exit(-1)
#define ERROR(MSG) { \
    fprintf(stderr, "ERROR %s:%d : %s\n", __FILE__, __LINE__, (MSG)); \
    exit(-1); }

// If false, raises an error
#define ASSERT(PRED, MSG) { if (!(PRED)) ERROR((MSG)); }

#endif // ERROR_H
