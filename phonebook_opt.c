#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "phonebook_opt.h"
#include "debug.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */
entry *findName(char lastname[], entry *pHead)
{
    /* TODO: implement */
    size_t len = strlen( lastname);
    //show_entry( pHead);
    while (pHead != NULL) {
        /*this because the assert test, we can't modify the strcut*/
        /*v*/
        if (strncasecmp(lastname, pHead->lastName, len) == 0 && (pHead->lastName[len] == '\n' || pHead->lastName[len] == '\0')) {
            //pHead->lastName[len] = '\0';//FIXME: mmap cant not modify
            pHead->lastName = ( char*) malloc( sizeof(char) * MAX_LAST_NAME_SIZE);
            memset( pHead->lastName, '\0', MAX_LAST_NAME_SIZE);
            strcpy( pHead->lastName, lastname);
            pHead->dtl = ( pdetail) malloc( sizeof( detail));
            return pHead;
        }
        dprintf("find string = %s\n", pHead->lastName);
        pHead = pHead->pNext;
    }
    return NULL;
}

append_a* new_append_a( char* ptr, char* eptr, int tid, int ntd)
{
    append_a* app = ( append_a*) malloc( sizeof( append_a));

    app->ptr = ptr;
    app->eptr  = eptr;
    app->tid = tid;
    app->nthread = ntd;
    entry* tmp = ( entry*) malloc( sizeof( entry));

    app->pHead = (app->pLast = tmp);

    return app;
}

void append( void* arg)
{
    struct timespec start, end;
    double cpu_time;

    clock_gettime( CLOCK_REALTIME, &start);

    append_a* app = ( append_a*) arg;


    entry* eny = (entry *) malloc(sizeof(entry));
    int count = 0;
    for( char* i = app->ptr; i < app->eptr; i += MAX_LAST_NAME_SIZE * app->nthread, count++) {
        //app->pLast->pNext = eny;
        app->pLast->pNext = (entry *) malloc(sizeof(entry));
        app->pLast = app->pLast->pNext;

        //strcpy(app->pLast->lastName, app->ptr + i);//TODO: we could
        app->pLast->lastName = i;
        dprintf("thread %d append string = %s\n", app->tid, app->pLast->lastName);
        app->pLast->pNext = NULL;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time = diff_in_second(start, end);

    printf("thread take %lf sec, count %d\n", cpu_time, count);

    pthread_exit( NULL);
}

void show_entry( entry* pHead)
{
    while( pHead != NULL) {
        printf("lastName = %s\n", pHead->lastName);
        pHead = pHead->pNext;
    }
    return;
}

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
