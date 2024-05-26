/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * Dynamic Allcator is implemented as explicit list with first fit. 
 * All block has header and footer. 
 * Blocks can be coalesced and reused.
 * Free block has pointers to next free block, previous free block additionally.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4 /* word size */
#define DSIZE 8 /* double word size */
#define CHUNKSIZE (1<<12) /* default size of expanding the heap */

#define MAX(x, y) ((x) > (y) ? (x) : (y)) /* Return greater one */

#define PACK(size, alloc) ((size) | (alloc)) /* Pack size and allocation information of block */

#define GET(p)      (*(unsigned int *) (p)) /* Get a value from address p */
#define PUT(p, val) (*(unsigned int *) (p) = (val)) /* Write a value at address p */

#define GET_SIZE(p) (GET(p) & ~0x7) /* Get a size information from value at p */
#define GET_ALLOC(p) (GET(p) & 0x1) /* Get a allocation information from value at p */

#define HDRP(bp)    ((char *)(bp) - WSIZE) /* Get a header pointer from block pointer */
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) /* Get a footer pointer from block pointer */

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) /* Get a pointer to next block from block pointer */
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) /* Get a pointer to previous block from block pointer */

#define NEXT_PTR(bp) (*(char **)(bp)) /* Get a pointer to next free block from block pointer */
#define PREV_PTR(bp) (*(char **)((char *)(bp) + WSIZE)) /* Get a pointer to previous free block from block pointer */

#define PUT_NEXT_PTR(bp, ptr) (*(char **)(bp) = (char*)(ptr)) /* set a address value at space for pointer to next free block */
#define PUT_PREV_PTR(bp, ptr) (*(char **)((char *)(bp) + WSIZE) = (char*)(ptr)) /* set a address value at space for pointer to next free block */

/* global variables  */
static char* heap_ptr; /* pointer to prologue block */
static char* free_ptr; /* pointer to first free block */

/* static function declaration */
static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *coalesce(void *bp);
static void insert_to_free_list(void *bp);
static void delete_from_free_list(void *bp);
static void modify_free_list(void *prev_ptr, void *next_ptr, void *next);

/* mm_check declaration */
int mm_check(void);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_ptr = mem_sbrk(8*WSIZE)) == (void *)-1) 
        return -1;

    PUT(heap_ptr, 0);
    /* PUT a prologue block*/
    PUT(heap_ptr + (1*WSIZE), PACK(DSIZE, 1));
    PUT(heap_ptr + (2*WSIZE), PACK(DSIZE, 1));
    /* PUT a first free block of (2 * DSIZE) size */
    PUT(heap_ptr + (3*WSIZE), PACK(2 * DSIZE, 0));
    PUT(heap_ptr + (4*WSIZE), (int)NULL);
    PUT(heap_ptr + (5*WSIZE), (int)NULL);
    PUT(heap_ptr + (6*WSIZE), PACK(2 * DSIZE, 0));
    /* PUT a epilogue block*/
    PUT(heap_ptr + (7*WSIZE), PACK(0, 1));
    
    heap_ptr += (2*WSIZE); /* pointer to prologue block */
    free_ptr = heap_ptr + 2*WSIZE; /* pointer to first free block */
    
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) { /* if extend heap failed */
        return -1;
    }
    
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize = ALIGN(size + SIZE_T_SIZE); /* (request size + header + footer) rounds up to the nearest multiple of ALIGNMENT */
    size_t extendsize; /* size to extend */
    char *bp;

    if (size == 0) { /* if request size is zero */
        return NULL;
    }

    /* find free block for request */
    if ((bp = find_fit(asize)) != NULL) { /* if there exists free block for request */
        place(bp, asize);
        return bp;
    } 

    /* if there is no free block for request */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;

    place(bp, asize);

    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size; /* size of freed block */

    if (ptr == NULL) { /* if ptr is NULL */
        return;
    }

    size = GET_SIZE(HDRP(ptr));
    
    if (GET_ALLOC(HDRP(ptr)) == 0) { /* if the memory is not allocated */
        return;
    }

    /* set allocation as 0 of header & footer */
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    
    /* coalescing */
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    char *nextptr = NEXT_BLKP(ptr); /* pointer to next block */
    void *oldptr = ptr; /* copy of ptr */
    void *newptr; /* pointer to newly allocated block */
    size_t copySize; /* size to copy memory */
    size_t newsize = ALIGN(size + DSIZE); /* size needed for request */
    size_t oldsize = GET_SIZE(HDRP(oldptr)); /* size of original block */
    size_t nextsize = GET_SIZE(HDRP(nextptr)); /* size of next block of original block */

    if (ptr == NULL) {  /* if ptr is NULL, the call is equivalent to mm_malloc(size) */
        return mm_malloc(size);
    }

    if (size == 0) {    /* if size is equal to zero, the call is equivalent to mm_free(ptr) */
        mm_free(ptr);
        return NULL;
    }
    
    if (newsize <= oldsize) { /* if new size <= allocated size */
        return oldptr;
    }

    if ((GET_ALLOC(HDRP(nextptr)) == 0) && (newsize <= (oldsize + nextsize))) { /* If next block is free & (original block size + next block size) is sufficient for the request */
        delete_from_free_list(nextptr); /* delete next block from free list */
        /* Set header & footer of newly allocated block(original block + next block) */
        PUT(FTRP(nextptr), PACK((oldsize + nextsize), 1)); 
        PUT(HDRP(oldptr), PACK((oldsize + nextsize), 1)); 
        return oldptr;
    }

    /* allocate new block */
    if ((newptr = mm_malloc(size)) == NULL) { /* if mm_malloc failed */
        return newptr;
    }
    
    /* memory copy */
    copySize = ((oldsize - DSIZE) > size) ? size : (oldsize - DSIZE); 
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr); /* free original block */
    
    return newptr;
}

/* mm_check & additinoal static function definition */
/* 
 * mm_check - check heap consistency
 */
int mm_check(void) {
    char *start;
    int error = 0;
    char *heap_start = (char *)mem_heap_lo();
    char *heap_end = (char *)mem_heap_hi();

    /* iterate in free block list */
    for (start = free_ptr; start != NULL; start = NEXT_PTR(start)) {
        if (GET_ALLOC(HDRP(start)) != 0) { /* if there is free block that is not marked as free in free list */
            printf("[ERROR] free block(%p) is not marked as free\n", start);
            error = 1;
        }
    }

    /* iterate in all blocks */
    for (start = heap_ptr; GET_SIZE(HDRP(start)) > 0; start = NEXT_BLKP(start)) {
        if (start < heap_start || start > heap_end) { /* if the block pointer lies in out of heap range */
            printf("[ERROR] The block pointer(%p) is out of heap range(%p~%p)\n", start, mem_heap_lo(), mem_heap_hi());
            error = 1;
        }

        if (GET_ALLOC(HDRP(start)) == 1 && GET_ALLOC(HDRP(NEXT_BLKP(start))) == 1) {
            if (FTRP(start) >= HDRP(NEXT_BLKP(start))) { /* if there is a overlap between contigous allocated block */
                printf("There is overlap between allocated blocks(%p~%p, %p~%p)", HDRP(start), FTRP(start), HDRP(NEXT_BLKP(start)), FTRP(NEXT_BLKP(start)));
                error = 1;
            }
        }
    }
    return !error; /* if there is no error, return 1. Otherwise, return 0 */
}

/*
 * extend_heap - extend heap
 */
static void *extend_heap(size_t words) {
    char *bp; /* pointer to start of extended heap */
    size_t size; /* size to request expansion of heap */

    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; /* for double alignment */
    if((bp = mem_sbrk(size)) == (void *)-1) { /* if mem_sbrk failed */
        return NULL;
    }

    /* set header & footer of newly extended block */
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    /* set header of new epilogue block */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    
    return coalesce(bp); /* coalescing */
}

/*
 * find_fit - find free block that has sufficient size for malloc request in the free block list
 */
static void *find_fit(size_t asize) {
    void *bp;

    if (free_ptr == NULL) { /* If there is no free block */
        return NULL;
    }
    
    for (bp = free_ptr; bp != NULL; bp = NEXT_PTR(bp)) { /* find in the free block list */
        if ((GET_ALLOC(HDRP(bp)) == 0) && (asize <= GET_SIZE(HDRP(bp)))) { /* If the block has sufficient size for the request */
            return bp;
        }
    }
    
    return NULL; /* If there is no free block that has sufficient size for request */
}

/*
 * place - place the request block at the beginning of the free block
 */
static void place(void *bp, size_t asize) { 
    size_t csize = GET_SIZE(HDRP(bp)); /* size of the free block */
    char *prev_ptr = PREV_PTR(bp); /* pointer to previous free block */
    char *next_ptr = NEXT_PTR(bp); /* pointer to next free block */
    char *next;

    if ((csize - asize) >= (3*DSIZE)) { /* if the remainder is greater or equal than minimum block size(split the block) */
        /* change header & footer of newly allocated block*/
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));

        /* pointer to splitted free block */
        next = NEXT_BLKP(bp); 
        
        /* change header & footer of splitted free block */
        PUT(HDRP(next), PACK(csize-asize, 0));
        PUT(FTRP(next), PACK(csize-asize, 0));
        
        /* modify free block list */
        modify_free_list(prev_ptr, next_ptr, next); 
    } else { /* if the remainder is smaller than minimum block size(do not split the block) */
        /* change header & footer of newly allocated block */
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        
        /* delete the block from free block list */
        delete_from_free_list(bp); 
    }
}

/*
 * coalesce - coalescing free block in order to prevent false fragment
 */
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); /* Get a allocation information of previous block */
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); /* Get a allocation information of next block */
    size_t size = GET_SIZE(HDRP(bp)); /* Get a size of the freed block */

    if (prev_alloc && next_alloc) {         /* previous block is allocated & next block is allocated */
        insert_to_free_list(bp); /* insert the block to free block list */
    } else if (prev_alloc && !next_alloc) { /* previous block is allocated & next block is free */
        delete_from_free_list(NEXT_BLKP(bp)); /* delete next block from free block list */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        /* change header & footer of coalesced block */
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_to_free_list(bp); /* insert the block to free block list */
    } else if (!prev_alloc && next_alloc) { /* previous block is free & next block is allocated */
        delete_from_free_list(PREV_BLKP(bp)); /* delete previous block from free block list */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        /* change header & footer of coalesced block */
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        insert_to_free_list(bp); /* insert the block to free block list */
    } else {                                /* previous block is free & next block is free */
        delete_from_free_list(PREV_BLKP(bp)); /* delete previous block from free block list */
        delete_from_free_list(NEXT_BLKP(bp)); /* delete next block from free block list */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        /* change header & footer of coalesced block */
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        insert_to_free_list(bp); /* insert the block to free block list */
    }
    return bp;
}

/*
 * insert_to_free_list - insert free block in the start of free block list (LIFO)
 */
static void insert_to_free_list(void *bp) {
    char *start_ptr = free_ptr;
    if (free_ptr == NULL) { /* if there was no free block */
        /* insert bp in the start of free block list */
        free_ptr = bp;
        /* Set previous ptr & next ptr of bp as NULL */
        PUT_PREV_PTR(bp, NULL);
        PUT_NEXT_PTR(bp, NULL);
    } else { /* if there exists free block */
        if (start_ptr != bp) { /* if first free block in free block list is different from new free block */
            /* set previous ptr & next ptr */
            PUT_PREV_PTR(start_ptr, bp);
            PUT_NEXT_PTR(bp, start_ptr);
            PUT_PREV_PTR(bp, NULL);            
        }
        /* insert bp in the start of free block list */
        free_ptr = bp;
    }
}

/*
 * delete_from_free_list - delete free block from free block list
 */
static void delete_from_free_list(void *bp) {
    char *prev_ptr = PREV_PTR(bp); /* pointer to previous free block */
    char *next_ptr = NEXT_PTR(bp); /* pointer to next free block*/

    if (prev_ptr == NULL && next_ptr == NULL) {         /* bp is a head && bp is a tail */
        free_ptr = NULL;
    } else if (prev_ptr == NULL && next_ptr != NULL) {  /* bp is a head && bp is not a tail*/
        PUT_PREV_PTR(next_ptr, NULL);
        free_ptr = next_ptr;
    } else if (prev_ptr != NULL && next_ptr == NULL) {  /* bp is not a head && bp is a tail */
        PUT_NEXT_PTR(prev_ptr, NULL);
    } else {                                            /* bp is not a head && bp is not a tail */
        PUT_PREV_PTR(next_ptr, prev_ptr);
        PUT_NEXT_PTR(prev_ptr, next_ptr);
    }
}

/*
 * modify_free_list - modify free block list after splitting free block
 */
static void modify_free_list(void *prev_ptr, void *next_ptr, void *next) {

    if (prev_ptr == NULL && next_ptr == NULL) {         /* original block is a head && original block is a tail */
        PUT_PREV_PTR(next, NULL);
        PUT_NEXT_PTR(next, NULL);
        free_ptr = next;
    } else if (prev_ptr == NULL && next_ptr != NULL) {  /* original block is a head && original block is not a tail*/
        PUT_PREV_PTR(next, NULL);
        PUT_NEXT_PTR(next, next_ptr);
        PUT_PREV_PTR(next_ptr, next);
        free_ptr = next;
    } else if (prev_ptr != NULL && next_ptr == NULL) {  /* original block is not a head && original block is a tail */
        PUT_PREV_PTR(next, prev_ptr);
        PUT_NEXT_PTR(next, NULL);
        PUT_NEXT_PTR(prev_ptr, next);
    } else {                                            /* original block is not a head && original block is not a tail */
        PUT_NEXT_PTR(prev_ptr, next);
        PUT_PREV_PTR(next, prev_ptr);
        PUT_NEXT_PTR(next, next_ptr);
        PUT_PREV_PTR(next_ptr, next);
    }
}