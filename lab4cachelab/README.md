This is the handout directory for the CS:APP Cache Lab. 

************************
Running the autograders:
************************

Before running the autograders, compile your code:
    linux> make

Check the correctness of your simulator:
    linux> ./test-csim

Check the correctness and performance of your transpose functions:
    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 64 -N 64
    linux> ./test-trans -M 61 -N 67

Check everything at once (this is the program that your instructor runs):
    linux> ./driver.py    

******
Files:
******

# You will modifying and handing in these two files
# 修改以下两个文件
csim.c       Your cache simulator
trans.c      Your transpose function

# Tools for evaluating your simulator and transpose function
Makefile     Builds the simulator and tools
README       This file
driver.py*   The driver program, runs test-csim and test-trans
cachelab.c   Required helper functions
cachelab.h   Required header file
csim-ref*    The executable reference cache simulator
test-csim*   Tests your cache simulator
test-trans.c Tests your transpose function
tracegen.c   Helper program used by test-trans
traces/      Trace files used by test-csim.c

# 作业内容

有两个任务内容
- PartA 实现一个 cache 模拟器
- PartB 写一个矩阵转换函数，需要做缓存优化

## 其他说明

traces 文件夹，这个是给 PartA 使用的，以下可以追踪内存访问。valgrind 是内存泄漏检测工具

```
valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l
# 输出结果如下
I 0400d7d4,8    # I 表示 load 一个指令
 M 0421c7f0,4   # M 表示修改一个数据
 L 04f6b868,8   # L 表示 load 一个数据
 S 7ff0005c8,8  # S 表示存储一个数据
```

## PartA 部分

写一个缓存模拟器 csim.c，模拟 hit/miss 行为。这个表现要求跟 csim-ref 的表现一样

csim-ref 的使用，当然 csim.c 不需要输出 verbose 信息

```
./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.traces
L 10,1 miss
M 20,1 miss hit
L 22,1 hit
S 18,1 hit
L 110,1 miss eviction
L 210,1 miss eviction
M 12,1 miss eviction hit
hits:4 misses:5 evictions:3

参数表示如下
-h: Optional help flag that prints usage info 查看帮助
-v: Optional verbose flag that displays trace info 查看更详细的信息verbose
-s <s>: Number of set index bits (S = 2s is the number of sets) 表示 set 的数量
-E <E>: Associativity (number of lines per set) 表示每个 set 中 line 的数量
-b <b>: Number of block bits (B = 2b is the block size) 表示每个 block 的 bit 数量
-t <tracefile>: Name of the valgrind trace to replay 表示使用的 trace file
```

partA 只关注数据的部分，不关注 trace 文件中 I开头(表示加载一个指令)的部分，在 csim.c 中需要调用 printSummary 输出信息。printSummary(hit_count, miss_count, eviction_count); eviction_count 表示驱逐数量的意思

