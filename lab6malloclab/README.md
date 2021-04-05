#####################################################################
# CS:APP Malloc Lab
# Handout files for students
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

***********
Main Files:
***********

mm.{c,h}	
	Your solution malloc package. mm.c is the file that you
	will be handing in, and is the only file you should modify.

mdriver.c	
	The malloc driver that tests your mm.c file

short{1,2}-bal.rep
	Two tiny tracefiles to help you get started. 

Makefile	
	Builds the driver

**********************************
Other support files for the driver
**********************************

config.h	Configures the malloc lab driver
fsecs.{c,h}	Wrapper function for the different timer packages
clock.{c,h}	Routines for accessing the Pentium and Alpha cycle counters
fcyc.{c,h}	Timer functions based on cycle counters
ftimer.{c,h}	Timer functions based on interval timers and gettimeofday()
memlib.{c,h}	Models the heap and sbrk function

*******************************
Building and running the driver
*******************************
To build the driver, type "make" to the shell.

To run the driver on a tiny test trace:

	unix> mdriver -V -f short1-bal.rep

The -V option prints out helpful tracing and summary information.

To get a list of the driver flags:

	unix> mdriver -h

# 文件说明

- mm.c 需要修改的文件
- mdriver.c 测试用，检测性能

# mm.c 文件

- int mm_init(void);
  - 初始化工作。如果有问题返回 -1，否则返回 0
- void *mm_malloc(size_t size);
  - 需要8个字节对齐
- void mm_free(void *ptr);
  - 只能接受 mm_malloc 或 mm_realloc 出来的指针参数
- void *mm_realloc(void *ptr, size_t size);
  - 如果 ptr 为 null，则等价于 mm_malloc
  - 如果 size == 0，则等价于 mm_free(ptr)
  - 如果 ptr 不为 null，ptr 需要为 mm_malloc 或 mm_realloc 出来的指针
  - 新增的 size 部分，为未初始化的，剩余的保持跟旧的一样

# heap 一致性检查器

辅助检查的作用. 可以在 mm.c 中写一个函数 int mm check(void)，但是提交的时候去掉这个，防止影响性能
- 是否 free list 里的 block 都标记了
- 是否有连续的 free block 没有合并
- 每个 free block 是否真的是 free 的
- free list 中的指针是否都是有效地指向 free block
- 分配的 block 是否有重叠
- heap block 指针是否指向有效的 heap 地址

# mdriver.c 测试用

- -t <tracedir> 修改 config.h 里面的 tracefile 路径
- -f <tracefile> 使用具体的一个文件来测试
- -h 输出命令行参数的介绍
- -l
- -v 输出更多的信息。将表现分解输出到一个表中
- -V 输出更多更多的信息。输出额外的诊断信息，当 malloc 失败的时候，可以用来找出哪个 trace file 导致失败

# 一些规则

- 不能调用内存分配的系统调用。如 malloc,free,sbrk 等
- 不能用全局数据结构，array,tree,struct 等，但是可以使用全局的 int,float等
- 8 bytes 对齐

# 建议

- 使用 mdriver -f 选项。short1,2-bal.rep 刚开始的时候，使用少量的 trace file 进行调试和测试
- 使用 -v 和 -V 选项。-v 会总结 trace file 细节，-V 帮助解决 error
- 使用 gcc -g 和 debugger. 方便调试找出错误
- 理解书中的实现。书中的实现是隐式列表分配器
- 使用宏来简化指针操作，参考书中的例子
- 分阶段来实现，先实现 malloc 和 free，使用前 9 个 trace file
- 使用 profiler. gprof 工具很有帮助

