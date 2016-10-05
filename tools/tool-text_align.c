#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../text_align.h"

int main(int argc, char *argv[])
{
    assert(argc == 4 &&
           "Usage: ./text_align <inputFileName> <alignedFileName> <padToLen>");
    int padToLen = atoi(argv[3]);

    /* Argument assertion */
    assert(strlen(argv[1]) > 0 && strlen(argv[2]) > 0 &&
           "The file name cannot be an empty string.");
    assert(padToLen > 0 && "The aligned length must be positive.");

    text_align(argv[1], argv[2], padToLen);

    return 0;
}
