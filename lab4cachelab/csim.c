#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

static bool verbose = false;

/******** 队列begin *************/
typedef int QElemType;
typedef int Status;

typedef struct QNode {
    QElemType data; /* 数据 */
    struct QNode *next; /* 指针 */
} QNode, *QueuePtr;;

typedef struct {
    QueuePtr front;
    QueuePtr rear;
    int size;
    int cap; /* 容量 */
} LinkQueue;

// 初始化
Status initQueue(LinkQueue *Q, int cap) {
    Q->front = Q->rear = (QueuePtr) malloc(sizeof(QNode));
    if (!Q->front) exit(-1);
    Q->front->next = NULL;
    Q->size = 0;
    Q->cap = cap;
    return 0;
}

// 销毁
Status destroyQueue(LinkQueue *Q) {
    while (Q->front)
    {
        Q->rear = Q->front->next;
        free(Q->front);
        Q->front = Q->rear;
    }
   return 0;
}

// 入队
Status enQueue(LinkQueue *Q, QElemType e) {
    QueuePtr p = (QueuePtr)malloc(sizeof(QNode));
    if (!p) exit(-1);
    //插入数据
    p->data = e;
    p->next = NULL;
    //Q.rear一直指向队尾
    Q->rear->next = p;
    Q->rear = p;
    Q->size += 1;
    // 超过容量删掉头部
    if (Q->size > Q->cap) {
        QNode *q = Q->front;
        free(Q->front);
        Q->front = q;
        return 1;
    } 
    return 0;
 }

// 查找并移动到尾部
Status loadQueue(LinkQueue *Q, QElemType e) {
    QNode *cur = Q->front;
    while (cur) {
        QNode *q = cur->next;
        if (q->data == e) {
            // 删除当前节点
            
            // 入队到尾部
            return enQueue(Q, e);
        }
        cur = q;
    }
    return 0;
}

/******** 队列end *************/

// 计算 Set 和 Tag
void calst(unsigned long long int addr, int S, int B, int *Set, int *Tag) {
    // x/B/S == Tag
    // x/B%S == Set
    *Tag = addr/B/S;
    *Set = addr/B%S;
}

// 计算 hits, misses, evictions
// 如果是 L，判断 Tag 是否相等，如果相等则 hit，如果 -1 则 miss，否则 miss+eviction
// 如果是 S，判断 Tag，如果相等则 hit，如果 -1 则 miss，如果不是则 miss+eviction
// 如果是 M，在 S 的基础上加 hit
void calhme(int Set, int Tag, char action, int store[], unsigned int *hits, unsigned int *misses, unsigned int *evictions) {
    if (action == 'L') {
        if (store[Set] == Tag) *hits = *hits + 1; 
        else if (store[Set] == -1) *misses = *misses + 1;
        else {
            *misses = *misses + 1;
            *evictions = *evictions + 1;
        }
    } else if (action == 'S') {
        if (store[Set] == Tag) *hits = *hits + 1;
        else if (store[Set] == -1) *misses = *misses + 1;
        else {
            *misses = *misses + 1;
            *evictions = *evictions + 1;
        }
    } else {
        if (store[Set] == Tag) *hits = *hits + 2;
        else if (store[Set] == -1) {
            *misses = *misses + 1;
            *hits = *hits + 1;
        } else {
            *misses = *misses + 1;
            *evictions = *evictions + 1;
            *hits = *hits + 1;
        }

    }
}

int main(int argc, char *argv[])
{
    int opt;
    int s, E, b;
    char *t;

    // 解析参数
    // -s set 的数量，-E 每个 set 中 line 的数量，-b block 的数量，-t 使用的 trace file
    while ((opt = getopt(argc, argv, "s:E:b:t:v")) != -1) {
        switch (opt) {
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            t = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }
    printf("params. s:%d E:%d b:%d t:%s\n", s, E, b, t);
    int S = 1 << s;
    int B = 1 << b;

    // 存储数据结构部分。使用 S 大小的数组，里面存 int 表示 tag，初始 tag 为 -1
    int store[S];
    for (int i = 0; i < S; i++) {
        store[i] = -1;
    }

    // 逐行读取数据
    char buf[1000];
    unsigned int len, hits = 0, misses = 0, evictions = 0;
    unsigned long long int addr;
    FILE *full_trace_fp = fopen(t, "r");
    int Set, Tag;

    while (fgets(buf, 1000, full_trace_fp) != NULL) {
        /* 不关心指令 I 部分 */
        if (buf[0]==' ' && buf[2]==' ' &&
            (buf[1]=='S' || buf[1]=='M' || buf[1]=='L' )) {
            // 读取 addr 和 len
            sscanf(buf+3, "%llx,%u", &addr, &len);
            // printf("%llx,%u\n", addr, len);

            // 计算 Set 和 Tag
            calst(addr, S, B, &Set, &Tag);
            // printf("%d,%d\n", Set, Tag);

            // 计算 hits, misses, evictions
            calhme(Set, Tag, buf[1], store, &hits, &misses, &evictions);
            store[Set] = Tag;
        }
    }
    fclose(full_trace_fp);

    // hit_count, miss_count, eviction_count 命中，未命中，驱逐
    printSummary(hits, misses, evictions);
    return 0;
}

