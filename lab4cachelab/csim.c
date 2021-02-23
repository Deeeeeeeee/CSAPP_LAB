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
#define EVICTION 1

typedef struct QNode {
    QElemType data; /* 数据 */
    struct QNode *next; /* 指针 */
} QNode, *QueuePtr;;

typedef struct LinkQueue {
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
        QueuePtr q = Q->front->next;
        e = q->data;
        Q->front->next = q->next;
        if (Q->rear == q) Q->rear = Q->front; // 如果只剩一个元素，则为空队列
        free(q);
        Q->size -= 1;
        return EVICTION;
    } 
    return 0;
 }

// 查找并删除
QElemType rmQueue(LinkQueue *Q, QElemType e) {
    QueuePtr pre = Q->front;
    QueuePtr cur = Q->front->next;
    while (cur) {
        if (cur->data == e) {
            // 删除当前节点
            pre->next = cur->next;
            if (cur == Q->rear) Q->rear = pre; // 如果删的是最后一个元素，rear 往前移动
            free(cur);
            Q->size -= 1;
            return e;
        }
        cur = cur->next;
        pre = pre->next;
    }
    return -1;
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
void calhme(int Set, int Tag, char action, LinkQueue store[], unsigned int *hits, unsigned int *misses) {
    int rmResult = rmQueue(&store[Set], Tag);
    if (rmResult == Tag) {
        *hits = *hits + 1;
        if (verbose) printf(" hit");
    } else {
        *misses = *misses + 1;
        if (verbose) printf(" miss");
    }
    if (action == 'M') {
        *hits = *hits + 1;
        if (verbose) printf(" hit");
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
    // printf("params. s:%d E:%d b:%d t:%s\n", s, E, b, t);
    int S = 1 << s;
    int B = 1 << b;

    // 存储数据结构部分。使用 S 大小的数组，里面存 int 表示 tag，初始 tag 为 -1
    struct LinkQueue store[S];
    for (int i = 0; i < S; i++) {
        struct LinkQueue Q;
        initQueue(&Q, E);
        store[i] = Q;
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
            if (verbose) printf("%c %llx,%u", buf[1], addr, len);

            // 计算 Set 和 Tag
            calst(addr, S, B, &Set, &Tag);
            // printf("%d,%d\n", Set, Tag);

            // 计算 hits, misses, evictions
            calhme(Set, Tag, buf[1], store, &hits, &misses);
            int enResult = enQueue(&store[Set], Tag);
            if (enResult == EVICTION) {
                evictions += 1;
                if (verbose) printf(" eviction");
            }
            printf("\n");
        }
    }
    fclose(full_trace_fp);

    // 清除数据
    for (int i = 0; i < S; i++) {
        destroyQueue(&store[i]);
    }

    // hit_count, miss_count, eviction_count 命中，未命中，驱逐
    printSummary(hits, misses, evictions);
    return 0;
}

