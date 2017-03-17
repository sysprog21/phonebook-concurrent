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

#ifndef OPT
#define OUTPUT_FILE "orig.txt"

#else
#include "text_align.h"
#include "debug.h"
#include <fcntl.h>
#define ALIGN_FILE "align.txt"
#define OUTPUT_FILE "opt.txt"

#ifndef THREAD_NUM
#define THREAD_NUM 4
#endif

#endif

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
#endif
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    /* File preprocessing */
#ifndef OPT
    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (!fp) {
        printf("cannot open the file\n");
        return -1;
    }
#else
    text_align(DICT_FILE, ALIGN_FILE, MAX_LAST_NAME_SIZE);
    int fd = open(ALIGN_FILE, O_RDONLY | O_NONBLOCK);
    off_t file_size = fsize(ALIGN_FILE);
#endif

    /* Build the entry */
    entry *pHead, *e;
    printf("size of entry : %lu bytes\n", sizeof(entry));

#if defined(OPT)
    char *map;
    entry *entry_pool;
    pthread_t threads[THREAD_NUM];
    thread_arg *thread_args[THREAD_NUM];

    /* Start timing */
    clock_gettime(CLOCK_REALTIME, &start);
    /* Allocate the resource at first */
    map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    assert(map && "mmap error");
    entry_pool = (entry *)malloc(sizeof(entry) *
                                 file_size / MAX_LAST_NAME_SIZE);
    assert(entry_pool && "entry_pool error");

    /* Prepare for multi-threading */
    pthread_setconcurrency(THREAD_NUM + 1);
    for (int i = 0; i < THREAD_NUM; i++)
        // Created by malloc, remeber to free them.
        thread_args[i] = createThread_arg(map + MAX_LAST_NAME_SIZE * i, map + file_size, i,
                                          THREAD_NUM, entry_pool + i);
    /* Deliver the jobs to all threads and wait for completing */
    clock_gettime(CLOCK_REALTIME, &mid);
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_create(&threads[i], NULL, (void *)&append, (void *)thread_args[i]);

    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(threads[i], NULL);

    /* Connect the linked list of each thread */
    for (int i = 0; i < THREAD_NUM; i++) {
        if (i == 0) {
            pHead = thread_args[i]->lEntry_head->pNext;
            DEBUG_LOG("Connect %d head string %s %p\n", i,
                      pHead->lastName, thread_args[i]->data_begin);
        } else {
            e->pNext = thread_args[i]->lEntry_head->pNext;
            DEBUG_LOG("Connect %d head string %s %p\n", i,
                      e->pNext->lastName, thread_args[i]->data_begin);
        }

        e = thread_args[i]->lEntry_tail;
        DEBUG_LOG("Connect %d tail string %s %p\n", i,
                  e->lastName, thread_args[i]->data_begin);
        DEBUG_LOG("round %d\n", i);
    }
    /* Stop timing */
    clock_gettime(CLOCK_REALTIME, &end);
#else /* ! OPT */
    pHead = (entry *) malloc(sizeof(entry));
    e = pHead;
    e->pNext = NULL;

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* Start timing */
    clock_gettime(CLOCK_REALTIME, &start);

    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
    }

    /* Stop timing */
    clock_gettime(CLOCK_REALTIME, &end);

    /* close file as soon as possible */
    fclose(fp);
#endif

    cpu_time1 = diff_in_second(start, end);

    /* Find the given entry */
    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* Compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    /* Write the execution time to file. */
    FILE *output;
    output = fopen(OUTPUT_FILE, "a");
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    /* Release memory */
#ifndef OPT
    while (pHead) {
        e = pHead;
        pHead = pHead->pNext;
        free(e);
    }
#else
    /* Free the allocated detail entry */
    e = pHead;
    while (e) {
        free(e->dtl);
        e = e->pNext;
    }

    free(entry_pool);
    for (int i = 0; i < THREAD_NUM; ++i)
        free(thread_args[i]);

    munmap(map, file_size);
    close(fd);
#endif
    return 0;
}
