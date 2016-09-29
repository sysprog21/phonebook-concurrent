#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#include IMPL
//#ifdef OPT
//	#define  _GNU_SOURCE
//#endif

#define DICT_FILE "./dictionary/words.txt"

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
#ifndef OPT
    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
#else
    struct timespec mid;
    double cpu_time_mid;
#endif
    struct timespec start, end;
    double cpu_time1, cpu_time2;

#ifndef OPT

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }
#else

#include "file.c"
#include "debug.h"
#include <fcntl.h>
#define ALIGN_FILE "align.txt"
    file_align( DICT_FILE, ALIGN_FILE, MAX_LAST_NAME_SIZE);
    int fd = open( ALIGN_FILE, O_RDONLY | O_NONBLOCK);

    off_t fs = fsize( ALIGN_FILE);

    char* map = mmap( NULL, fs, PROT_READ, MAP_SHARED, fd, 0);

    assert( map && "mmap error");
#endif

    /* build the entry */
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif

#if defined(OPT)

#ifndef THREAD_NUM
#define THREAD_NUM 4
#endif

    pthread_setconcurrency( THREAD_NUM + 1);//  _GNU_SOURCE

    pthread_t *tid = ( pthread_t*) malloc( sizeof( pthread_t) * THREAD_NUM);
    append_a	**app = ( append_a**)	 malloc( sizeof( append_a*)	* THREAD_NUM);
    for( int i = 0; i < THREAD_NUM; i++)
        app[i] = new_append_a( map + MAX_LAST_NAME_SIZE * i, map + fs, i, THREAD_NUM);

    clock_gettime(CLOCK_REALTIME, &start);
    /*while (fgets(line, sizeof(line), fp))
        e = append(line, e);*/
    for( int i = 0; i < THREAD_NUM; i++)
        pthread_create( &tid[i], NULL, (void*) &append, ( void*) app[i]);

    clock_gettime(CLOCK_REALTIME, &mid);
    for( int i = 0; i < THREAD_NUM; i++)
        pthread_join( tid[i], NULL);

    entry* etmp;
    pHead = pHead->pNext;
    for( int i = 0; i < THREAD_NUM; i++) {
        if( i == 0) {
            pHead = app[i]->pHead->pNext;
            dprintf("Connect %d head string %s %p\n", i, app[i]->pHead->pNext->lastName, app[i]->ptr);
        } else {
            etmp->pNext = app[i]->pHead->pNext;
            dprintf("Connect %d head string %s %p\n", i, app[i]->pHead->pNext->lastName, app[i]->ptr);
        }

        etmp = app[i]->pLast;
        dprintf("Connect %d tail string %s %p\n", i, app[i]->pLast->lastName, app[i]->ptr);
        printf("round %d\n", i);
        show_entry( pHead);

    }
    //etmp->pNext = NULL;//it is NULL

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);
    cpu_time_mid = diff_in_second( mid, end);

#else
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

#endif

#ifndef OPT
    /* close file as soon as possible */
    fclose(fp);
#endif

    e = pHead;

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
#if defined(OPT)
    output = fopen("opt.txt", "a");
#else
    output = fopen("orig.txt", "a");
#endif
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
#ifdef OPT
    printf("execution time of append() exclude thread_create : %lf sec\n", cpu_time_mid);
#endif
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);

    return 0;
}
