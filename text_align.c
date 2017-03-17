#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

#include "text_align.h"

#define MAX_BUFF_SIZE 100

/**
 * @brief Pad each line of the text file _origiFileName_
 *  to _PadToLen_ characters and write to the file _alignedFileName_.
 */
int text_align(const char *originFileName,
               const char *alignedFileName,
               const int padToLen)
{
    printf("orginal file size = %ld\n", fsize(originFileName));

    FILE *fd_origin = fopen(originFileName, "r");
    if (!fd_origin) return -1;
    FILE *fd_align = fopen(alignedFileName, "w+");
    if (!fd_align) {
        fclose(fd_origin);
        return -1;
    }

    char rbuf[MAX_BUFF_SIZE];
    int suffix;

    char *wbuf = (char *) malloc(sizeof(char) * padToLen);

    /* Read each line from input file and pad to "padToLen" characters. */
    while (fgets(rbuf, sizeof(rbuf), fd_origin)) {
        memset(wbuf, '\0', padToLen);

        suffix = padToLen - strlen(rbuf);
        /*
         * The length of input text is longer than the length to padding to.
         * Warn the user, and still write to the output file but with only
         * first "PadToLen" bytes.
         */
        if (suffix < 0)
            printf("Warning:"
                   " The length of %s is longer than %d.\n", rbuf, padToLen);
        strncpy(wbuf, rbuf, strlen(rbuf));

        fwrite(wbuf, padToLen, 1, fd_align);
    }

    fclose(fd_align);
    fclose(fd_origin);
    free(wbuf);

    return 0;
}

/**
 * @brief Get the size of the file in bytes.
 */
off_t fsize(const char *path)
{
    struct stat st;
    stat(path, &st);
    return st.st_size;
}
