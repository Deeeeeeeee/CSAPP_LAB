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
#define GET_ALLOC(p) (GET(p) & 0x1)

/* 计算 header 和 footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 计算前一块和后一块的地址 */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))  // 计算上一块的footer中的size

/** 获得块中记录的后继和前驱地址 */
#define PRED(bp) ((char*)(bp) + WSIZE)  // 注意，这里前驱是放在后继的后面的
#define SUCC(bp) ((char*)(bp))
/** 获得块的后继和前驱地址 */
#define PRED_BLKP(bp) (GET(PRED(bp)))
#define SUCC_BLKP(bp) (GET(SUCC(bp)))

/** 私有全局变量 */
static char *heap_listp;

/** 私有函数 */
static void *extend_heap(size_t words);
static void *imme_coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *add_block(void *bp);
static void *delete_block(void *bp);
static int Index(size_t size);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /** 创建初始化heap，放置序言块和结尾块 */
    if ((heap_listp = mem_sbrk(12*WSIZE)) == (void *)-1)
        return -1;
    /** 空闲块的最小块包括头部、后继、前驱和脚部，一共4个字，16个字节 */
    PUT(heap_listp, NULL);              //{16~31}
    PUT(heap_listp + (1*WSIZE), NULL);  //{32~63}
    PUT(heap_listp + (2*WSIZE), NULL);  //{64~127}
    PUT(heap_listp + (3*WSIZE), NULL);  //{128~255}
    PUT(heap_listp + (4*WSIZE), NULL);  //{256~511}
    PUT(heap_listp + (5*WSIZE), NULL);  //{512~1023}
    PUT(heap_listp + (6*WSIZE), NULL);  //{1024~2047}
    PUT(heap_listp + (7*WSIZE), NULL);  //{2048~4095}
    PUT(heap_listp + (8*WSIZE), NULL);  //{4096~inf}

    PUT(heap_listp + (9*WSIZE), PACK(DSIZE, 1));    // 序言块header
    PUT(heap_listp + (10*WSIZE), PACK(DSIZE, 1));   // 序言块footer
    PUT(heap_listp + (11*WSIZE), PACK(0, 1));       // 结尾块
    heap_listp += (10*WSIZE);

    /** 初始化 CHUNKSIZE 个块 */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc
 */
void *mm_malloc(size_t size)
{
    size_t asize;       // 调整 block 大小
    size_t extendsize;  // 如果没找到合适的，扩展堆
    char *bp;

    /** 忽略无效请求 */
    if (size == 0)
        return NULL;

    /** 调整最小块和对齐 */
    if (size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /** 搜索合适的空闲块 */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);   // 分割&设置已分配
        return bp;
    }

    /** 没找到合适的，申请更多的空间 */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);   // 分割&设置已分配
    return bp;
}

/*
 * mm_free
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));   // 首尾置成未分配
    PUT(FTRP(bp), PACK(size, 0));
    imme_coalesce(bp);
}

/*
 * mm_realloc
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
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /** 初始化header,footer和结尾块 */
    PUT(HDRP(bp), PACK(size, 0));           // 空闲块header
    PUT(FTRP(bp), PACK(size, 0));           // 空闲块footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   // 新的结尾块

    /** 合并 */
    bp = imme_coalesce(bp);
    bp = add_block(bp);
    return bp;
}

/** 合并操作 */
static void *imme_coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));     // 前一个 footer 中是否分配
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));     // 后一个 header 中是否分配
    size_t size = GET_SIZE(HDRP(bp));                       // 当前 block 大小

    if (prev_alloc && next_alloc) {                         // Case 1. 前后都分配了
        return bp;
    }

    else if (prev_alloc && !next_alloc) {                   // Case 2. 后面未分配
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        delete_block(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));                       // 先给 header 写 size，下一步 FTRP 会用到
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc) {                   // Case 3. 前面未分配
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        delete_block(PREV_BLKP(bp));
        PUT(FTRP(bp), PACK(size, 0));                       // 先给 footer 写 size
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));            // 给前一个 block 的 header 写 size
        bp = PREV_BLKP(bp);                                 // 修改 bp 为前一个 block 的 bp
    }

    else {                                                  // Case 4. 前后都未分配
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        delete_block(NEXT_BLKP(bp));
        delete_block(PREV_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

/** 插入到合适大小的空闲链中 */
static void *add_block(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    int index = Index(size);
    void *root = heap_listp + index*WSIZE;

    // LIFO
    return LIFO(bp, root);
    // AddressOrder
    // return AddressOrder(bp, root);
}

/** 前驱块的后继指针 指向 后继块, 后继块的前驱指针 指向 前驱块 */
static void *delete_block(void *bp)
{
    PUT(SUCC(PRED_BLKP(bp)), SUCC_BLKP(bp));
    if (SUCC_BLKP(bp) != NULL)
        PUT(PRED(SUCC_BLKP(bp)), PRED_BLKP(bp));
}

/** 确定在哪个空闲链的下标 */
static int Index(size_t size)
{
    int ind = 0;
    if (size >= 4096)
        return 8;

    size = size >> 5;
    while (size) {
        size = size >> 1;
        ind++;
    }
    return ind;
}

static void *LIFO(void *bp, void *root)
{
    if (SUCC_BLKP(root) != NULL) {
        PUT(PRED(SUCC_BLKP(root)), bp);     // SUCC -> bp
        PUT(SUCC(bp), SUCC_BLKP(root));     // bp -> SUCC
    } else {
        PUT(SUCC(bp), NULL);
    }
    PUT(SUCC(root), bp);                    // root -> bp
    PUT(PRED(bp), root);                    // bp -> root
    return bp;
}

/** 搜索操作 */
static void *find_fit(size_t asize)
{
    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) != 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= asize))
            return bp;
    }
    return NULL;    // 没找合适的
}

/** 分割&设置已分配操作 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2*DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

