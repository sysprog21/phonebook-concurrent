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
            pHead->dtl = ( pdetail) malloc( sizeof( dtl));
            return pHead;
        }
        pHead = pHead->pNext;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{

    e->pNext = (entry *) malloc(sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;

    return e;
}
