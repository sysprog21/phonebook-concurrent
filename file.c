#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 1000

void file_align(char *org, char *mod, int pad)
{
    FILE *fd0 = fopen(org, "r");
    FILE *fd1 = fopen(mod, "w+");

    char rbuf[MAX_BUFF_SIZE];
    int suffix;

    char *wbuf = (char *) malloc(sizeof(char) * pad);

    while (fgets(rbuf, sizeof(rbuf), fd0)) {
        memset(wbuf, '\0', pad);

        if ((suffix = (pad - strlen(rbuf))) != 0)
            strcpy(wbuf, rbuf);

        fwrite(wbuf, pad, 1, fd1);
    }

    fclose(fd0);
    fclose(fd1);
}

off_t fsize(char *path)
{
    struct stat st;
    stat(path, &st);
    return st.st_size;
}
