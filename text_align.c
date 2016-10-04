#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 100

static off_t fsize(char *path);

/**
 * @brief Pad each line of the text file _origiFileName_
 *  to _PadToLen_ characters and write to the file _alignedFileName_.
 */
int main(int argc, char *argv[])
{
    assert(argc == 4 &&
           "./main <originFileName> <alignedFileName> <PadToLen>");

    char *org = argv[1];
    char *mod = argv[2];
    int pad = atoi(argv[3]);

    printf("orginal file size = %ld\n", fsize(org));

    FILE *fd0 = fopen(org, "r");
    FILE *fd1 = fopen(mod, "w+");

    char rbuf[MAX_BUFF_SIZE];
    int suffix;

    char *wbuf = (char *) malloc(sizeof(char) * pad);

    // Read each line from input file and pad to "pad" characters.
    while (fgets(rbuf, sizeof(rbuf), fd0)) {
        memset(wbuf, '\0', pad);

        suffix = pad - strlen(rbuf);
        // The length of input text is longer than the length to padding to.
        // Warn the user, and still write to the output file but with only
        // first "PadToLen" bytes.
        if (suffix < 0)
            printf("Warning:"
                   " The length of %s is longer than %d.\n", rbuf, pad);
        strncpy(wbuf, rbuf, strlen(rbuf));

        fwrite(wbuf, pad, 1, fd1);
    }

    fclose(fd0);
    fclose(fd1);
    free(wbuf);

    return 0;
}

/**
 * @brief Get the size of the file in bytes.
 */
static off_t fsize(char *path)
{
    struct stat st;
    stat(path, &st);
    return st.st_size;
}
