/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "free",
    /* First member's full name */
    "sealde",
    /* First member's email address */
    "sealde@foxmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* 基础定义和宏 */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)   // 每次扩展的大小

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* 打包 size 和 是否分配 */
#define PACK(size, alloc) ((size) | (alloc))

/* 对地址 p 读和写 */
#define GET(p)      (*(unsigned int *)(p))   // 强制转换很重要
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* 读取 size 和 是否分配字段 */
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(args) (GET(p) & 0x1)

/* 计算 header 和 footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 计算前一块和后一块的地址 */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))  // 计算上一块的footer中的size

/** 私有全局变量 */
static char *heap_listp;

/** 私有函数 */

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /** 创建初始化heap，放置序言块和结尾块 */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                             // 第一个字用来对齐用，因为结尾块只有一个字
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    // 序言块header
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // 序言块footer
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        // 结尾块
    heap_listp += (2*WSIZE);

    /** 初始化 CHUNKSIZE 个块 */
    if (exte)
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	    return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

/** 用一个新的空闲块扩展堆 */
static void *extend_heap(size_t words)
{
    char  *bp;
    size_t size;

    /** 双字对齐 */
    size = ALIGN(words);
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /** 初始化header,footer和结尾块 */
    PUT(HDRP(bp), PACK(size, 0));           // 空闲块header
    PUT(FTRP(bp), PACK(size, 0));           // 空闲块footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   // 新的结尾块

    /** 合并 */
    return coalesce(bp);
}

