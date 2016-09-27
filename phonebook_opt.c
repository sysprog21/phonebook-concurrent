#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "phonebook_opt.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */
entry *findName(char lastname[], entry *pHead)
{
    /* TODO: implement */
    size_t len = strlen( lastname);

    while (pHead != NULL) {
        /*this because the assert test, we can't modify the strcut*/
        /*v*/
        if (strncasecmp(lastname, pHead->lastName, len) == 0 && (pHead->lastName[len] == '\n' || pHead->lastName[len] == '\0')) {
            pHead->lastName[len] = '\0';
            pHead->dtl = ( pdetail) malloc( sizeof( detail));
            return pHead;
        }
        pHead = pHead->pNext;
    }
    return NULL;
}



/*entry *append(char lastName[], entry *e)
{

    e->pNext = (entry *) malloc(sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;

    return e;
}*/

append_a* new_append_a( FILE* fp)
{
    append_a* app = ( append_a*) malloc( sizeof( append_a));

    app->fp = fp;
    entry* tmp = ( entry*) malloc( sizeof( entry));

    app->pHead = (app->pLast = tmp);

    return app;
}

void append( void* arg)
{
    append_a* app = ( append_a*) arg;
    char line[ MAX_LAST_NAME_SIZE];

    while( 1) {
        pthread_mutex_lock( &mux);

        if( !fgets( line, sizeof( line), app->fp)) {
            pthread_mutex_unlock( &mux);
            return;//pthread_exit is better
        }

        pthread_mutex_unlock( &mux);

        app->pLast->pNext = (entry *) malloc(sizeof(entry));
        app->pLast = app->pLast->pNext;

        strcpy(app->pLast->lastName, line);
        app->pLast->pNext = NULL;
    }
}
