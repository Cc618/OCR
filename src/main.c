#include <stdio.h>
#include "error.h"
#include "tools.h"

int main(int argc, char** argv) {
    printf("Hello world !\n");

    ASSERT(1 == 1, "1 != 1");
    ASSERT(2 == 1, "2 != 1");

    if (1 == 1)
        ERROR("OMG fatal error");

    return 0;
}

