#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt;
    // int s, E, b;
    // char *t;

    // 解析参数
    // -s set 的数量，-E 每个 set 中 line 的数量，-b block 的数量，-t 使用的 trace file
    while ((opt = getopt(argc, argv, "s:E:b:t:") != -1)) {
        printf("opt = %c\t\t", opt);
        printf("optarg = %s\t\t",optarg);
        printf("optind = %d\t\t",optind);
        printf("argv[optind] = %s\n",argv[optind]);
        /** switch (opt) { */
        /** case 's': */
        /**     s = atoi(optarg); */
        /**     break; */
        /** case 'E': */
        /**     E = atoi(optarg); */
        /**     break; */
        /** case 'b': */
        /**     b = atoi(optarg); */
        /**     break; */
        /** case 't': */
        /**     t = optarg; */
        /**     break; */
        /** default: */
        /**     exit(EXIT_FAILURE); */
        /** } */
    }
    // printf("params. s:%d E:%d b:%d \n", s, E, b);
    // hit_count, miss_count, eviction_count 命中，未命中，驱逐
    printSummary(0, 0, 0);
    return 0;
}
