/***
 Header file to provide a way to keep track of allocations
 and frees for heap memory.
 To enable the ability to track all pointers and gaurentee 
 their release, do the following
 1: #define CLEANUP N before using safealloc
 where N is an integer greater than 0 and the number of pointers
 expected to need tracking
 2: call cleanup(0) to release the pointers before program termination
*/

#ifndef SAFEALLOC_H
#define SAFEALLOC_H

#include <stdlib.h>
#include <stdio.h>

#define n0llptr 0

static int allocs = 0;

static int frees = 0;

#ifndef CLEANUP
#define CLEANUP 0
#endif

#if CLEANUP
int cleanup(void * check);

void replace(void* newptr, void* oldptr);

#endif

/** 
 Malloc that checks for failure to allocate memory
 :::param bytes: the number of bytes we need from the system
 ::: return : the allocated pointer or abort if memory allocation fails
*/
void * safe_malloc(size_t bytes);


/** 
 Calloc that checks for failure to allocate memory
 :::param n: the number of items needed
 :::param bytes: the number of bytes we need from the system per item
 ::: return : the allocated pointer or abort if memory allocation fails
*/
void * safe_calloc(size_t n, size_t bytes);

/**
 :::param ptr: the pointer we free  
*/
void safe_free(void* ptr);


/**
 Reallocation of old memory that considers null pointers 
 and failure to allocate new memory
 :::param old: the old pointer location we wish to add or shrink 
 :::param bytes: the size of the return pointer
 ::: return : the newly allocated pointer if possible
*/
void * safe_realloc(void * old, size_t bytes);

/** 
 Checking to make sure allocations equal 
 If not and cleanup is defined, then free all held pointers
 :::return : 0 for no issues, 1 for an issue with frees
*/
int leakcheck(void);

#endif