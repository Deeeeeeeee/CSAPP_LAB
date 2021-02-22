#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void calst(unsigned long long int addr, int S, int B, int *Set, int *Tag);
void calhme(int Set, int Tag, char action, int store[], unsigned int *hits, unsigned int *misses, unsigned int *evictions);

int main(int argc, char *argv[])
{
    int opt;
    int s, E, b;
    char *t;

    // 解析参数
    // -s set 的数量，-E 每个 set 中 line 的数量，-b block 的数量，-t 使用的 trace file
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
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
    unsigned int len, hits, misses, evictions;
    unsigned long long int addr;
    FILE *full_trace_fp = fopen(t, "r");
    int Set, Tag;

    while (fgets(buf, 1000, full_trace_fp) != NULL) {
        /* 不关心指令 I 部分 */
        if (buf[0]==' ' && buf[2]==' ' &&
            (buf[1]=='S' || buf[1]=='M' || buf[1]=='L' )) {
            // 读取 addr 和 len
            sscanf(buf+3, "%llx,%u", &addr, &len);
            printf("%llx,%u\n", addr, len);

            // 计算 Set 和 Tag
            calst(addr, S, B, &Set, &Tag);
            printf("%d,%d\n", Set, Tag);
        }
    }
    fclose(full_trace_fp);


    // hit_count, miss_count, eviction_count 命中，未命中，驱逐
    printSummary(0, 0, 0);
    return 0;
}

// 计算 Set 和 Tag
void calst(unsigned long long int addr, int S, int B, int *Set, int *Tag) {
    // x/B/S == Tag
    // x/B%S == Set
    *Tag = addr/B/S;
    *Set = addr/B%S;
}

// 计算 hits, misses, evictions
// 如果是 L，判断 Tag 是否相等，不相等则 miss，否则 hit
// 如果是 S，判断 Tag，如果相等则 hit，如果 -1 则 miss，如果不是则 miss+eviction
// 如果是 M，在 S 的基础上加 hit
void calhme(int Set, int Tag, char action, int store[], unsigned int *hits, unsigned int *misses, unsigned int *evictions) {
    if (action == 'L') {

    }
}

