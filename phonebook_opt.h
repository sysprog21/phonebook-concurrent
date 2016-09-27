#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#include <pthread.h>

#define MAX_LAST_NAME_SIZE 16

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */
#define OPT 1

typedef struct _detail {
    char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
} detail;

typedef detail *pdetail;

typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    struct __PHONE_BOOK_ENTRY *pNext;
    pdetail dtl;
} entry;

entry *findName(char lastname[], entry *pHead);
//entry *append(char lastName[], entry *e);

/*Pthread*/
pthread_mutex_t mux;

typedef struct _append_a {
    FILE* fp;
    entry* pHead;
    entry* pLast;
} append_a;

append_a* new_append_a( FILE* fp);

void append( void* arg);

#endif
