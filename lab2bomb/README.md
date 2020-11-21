This is an x86-64 bomb for self-study students. 

有6个炸弹，每个炸弹用一个字符串解决

# 文件说明

- bomb 炸弹的二进制可执行文件
- bomb.c 炸弹 main 函数的源文件

# 运行

```
> ./bomb psol.txt
```

# 工具

- gdb 调试 bomb
- objdump -t 输出 symbol table。symbol table 包括函数名称，全局变量
- objdump -d 反汇编。可以阅读 bomb 是怎么工作的，但系统调用是看不出来的，还是要用 gdb。比如 sscanf `8048c36: e8 99 fc ff ff call 80488d4 <_init+0x1a0>`
- strings 展示
- man ascii 查看字符编码

## gdb 使用

```
# 缩写
break -> b
step -> s
examine -> x
print -> p
next -> n   下一步不进入函数
step -> s   下一步进入函数
continue -> c   执行到下个断点
list -> l   查看源码
```

```
gdb bumb

b func_name  给函数设置断点
b *address 指定内存设置断点

set 设置变量
    直接修改内存 set {int}0x7fffffffddac = 0x137

x/<n/f/u> <addr>
    n 是正整数，表示向后显示 n 个内存单位的内容。这个单位由 u 决定
    f 表示显示的格式。指定地址 i，字符串 s
        x 按十六进制格式显示变量。
        d 按十进制格式显示变量。
        u 按十六进制格式显示无符号整型。
        o 按八进制格式显示变量。
        t 按二进制格式显示变量。
        a 按十六进制格式显示变量。
        c 按字符格式显示变量。
        f 按浮点数格式显示变量。
    u 表示显示内存单位的大小，默认是 4 bytes。可选 b 单字节；h 双字节；w 四字节；g 八字节
例子: 
    x/15i 0x400ee0 表示查看 0x400ee0 这个地址后面 15 个 4 字节的指令
    x/4s 0x402400 表示查看 0x402400 后面 4 个字符串
    x/20i $rip 查看当前要执行的指令

clear 清除断点
clear func_1 清除函数func_1里面的所有断点

```

# 题解

1. 第一个字符串是具体内存地址里的值
2. 第二个是 for 循环
3. 第三个是 switch
