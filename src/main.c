#include <stdio.h>
#include "error.h"

int main() {
    printf("Hello world !\n");

    ASSERT(1 == 1, "1 != 1");
    ASSERT(2 == 1, "2 != 1");

    if (1 == 1)
        ERROR("OMG fatal error");

    return 0;
}

