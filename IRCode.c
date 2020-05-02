#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "IRCode.h"

InterCode gInterCodeHead = NULL;
InterCode gInterCodeTail = NULL;

void InsertCode(InterCode ptr)
{
    assert(ptr != NULL);

    ptr->next = NULL;
    ptr->prev = NULL;
    if(gInterCodeHead == NULL)
    {
        gInterCodeHead = ptr;
        gInterCodeTail = ptr;
    }
    else
    {
        gInterCodeTail->next = ptr;
        ptr->prev = gInterCodeTail;
        gInterCodeTail = ptr;
    }
}

void DeleteCode(InterCode ptr)
{
    assert(ptr != NULL);

    if(ptr == gInterCodeTail)
    {
        gInterCodeTail = ptr->prev;
        gInterCodeTail->next = NULL;
    }
    else
    {
        ptr->next->prev = ptr->prev;
    }
    
    if(ptr == gInterCodeHead)
    {
        gInterCodeHead = ptr->next;
        gInterCodeHead->prev = NULL;
    }
    else
    {
        ptr->prev->next = ptr->next;
    }
    
    free(ptr);
}
