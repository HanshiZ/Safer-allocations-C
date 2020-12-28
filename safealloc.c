#include <stdlib.h>
#include <stdio.h>
#include "safealloc.h"
#define n0llptr 0

static int allocs = 0;

static int frees = 0;

#if CLEANUP
static int location;
static void* storage[CLEANUP];
/**
 Cleanup function that adds and cleans up pointers in the storage buffer
 valid pointers are added to the next spot while a null pointer triggers
 release of held data
 ::: param check: the pointer we wish to insert into storage
 ::: return : 0 for space still available or a proper release, 1 for out of space
*/
int cleanup(void * check)
{
    
    if (check != n0llptr && location < CLEANUP)
        storage[location++] = check;
    else if (check != n0llptr && location >= CLEANUP)
    {
        printf("%d pointers unable to track", ++location - CLEANUP);
        return 1;
    }
    else
    {
        printf("Cleanup protocol. Releasing pointers...\n");
        for(int i = 0; i < location; ++i)
             if (*(storage+i) != n0llptr)
             {    
                 printf("Release %p\n", *(storage+i) );
                 free(*(storage+i));
                 *(storage+i) = n0llptr;
             }
    }
    return 0;       
} 

/** 
 Replacing pointers in storage
 ::: param newptr: new pointer to go into storage
 ::: param oldptr: old pointer we need to swap out
*/
void replace(void* newptr, void* oldptr)
{
    for(int i= 0; i < location;++i)
        if (*(storage+i) == oldptr)
            *(storage+i) = newptr;
}
#endif

/** 
 Malloc that checks for failure to allocate memory
 :::param bytes: the number of bytes we need from the system
 ::: return : the allocated pointer or abort if memory allocation fails
*/
void * safe_malloc(size_t bytes)
{
    void* ptr = malloc(bytes);
    if (ptr == n0llptr)
        perror("Memory allocation returned a null pointer, no space left!\n");
#if CLEANUP
    cleanup(ptr);
#endif
    if (ptr == n0llptr) abort();
    ++allocs;
    return ptr;
}

/** 
 Calloc that checks for failure to allocate memory
 :::param n: the number of items needed
 :::param bytes: the number of bytes we need from the system per item
 ::: return : the allocated pointer or abort if memory allocation fails
*/
void * safe_calloc(size_t n, size_t bytes)
{
    void* ptr = calloc(n, bytes);
    if (ptr == n0llptr)
        perror("Memory allocation returned a null pointer, no space left!\n");
#if CLEANUP
    cleanup(ptr);
#endif
    if (ptr == n0llptr) abort();
    ++allocs;
    return ptr;

}

/**
 :::param ptr: the pointer we free  
*/
void safe_free(void* ptr)
{
    if (ptr != n0llptr)
    {
        free(ptr);
        ++frees;
        ptr = n0llptr;
    }
}

/**
 Reallocation of old memory that considers null pointers 
 and failure to allocate new memory
 :::param old: the old pointer location we wish to add or shrink 
 :::param bytes: the size of the return pointer
 ::: return : the newly allocated pointer if possible
*/
void * safe_realloc(void * old, size_t bytes)
{
    
    if (!bytes)
    {
        printf("\n!Careful! Using realloc as free. Null pointer returned.\n");
        safe_free(old);
        return n0llptr;
    }
    
    if (old == n0llptr)
    {
        printf("\n!Careful! Using realloc as malloc.\n");
        return safe_malloc(bytes);
    }
    
    void* temp = realloc(old, bytes);
    if (temp == n0llptr)
    {
        printf("Could not reallocate the array, old contents returned.\n");
        return temp;
    }
#if CLEANUP
    if (old != temp)
        replace(temp, old);
#endif
    return temp;
}

/** 
 Checking to make sure allocations equal 
 If not and cleanup is defined, then free all held pointers
 :::return : 0 for no issues, 1 for an issue with frees
*/
int leakcheck(void)
{
    if(frees == allocs)
    {
        printf("Allocations match frees.\n");
        return 0;
    }
    else
    {
         printf("\n!!!%d total free%s and %d total allocation%s!!!\n",frees, frees == 1 ? "" : "s", allocs, allocs == 1 ? "" : "s");
         #ifdef CLEANUP
             cleanup(0);
         #endif
         return 1;
    }
}

#undef CLEANUP
#undef n0llptr
