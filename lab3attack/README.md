This file contains materials for one instance of the attacklab.

Files:

    ctarget

Linux binary with code-injection vulnerability.  To be used for phases
1-3 of the assignment.

    rtarget

Linux binary with return-oriented programming vulnerability.  To be
used for phases 4-5 of the assignment.

     cookie.txt

Text file containing 4-byte signature required for this lab instance.

     farm.c

Source code for gadget farm present in this instance of rtarget.  You
can compile (use flag -Og) and disassemble it to look for gadgets.

     hex2raw

Utility program to generate byte sequences.  See documentation in lab
handout.

有 5 个攻击

# 文件说明

- ctarget 被攻击二进制文件，3个攻击任务，攻击方式 code-injection
- rtarget 被攻击二进制文件，2个攻击任务，攻击方式 return-oriented
- cookie 这个 lab 的签名认证，可以忽略
- farm.c 用来构建 gadget farm 的 source code
- hex2raw 一个生成攻击字符串的工具

# 其他说明

## ctarget 和 rtarget 读取字符串的方法
```
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

## 运行

```
# 帮助
$ ./ctarget -h

# 不发送结果到 grading server. 直接运行 ctarget 或 rtarget 会校验 cookie 和 host，-q 表示不校验
$ ./ctarget -q

# 从文件中读取，不用输入字符串
$ ./ctarget -i FILE
```

## 注意

- 漏洞的码不能包括 0x0a，因为 0x0a 是 '\n' 的 ASCII 编码，会中断读取字符串
- 使用 hexraw 时，只有单个 hex 值时要补充为两个，比如 0 -> 00。也要注意 little-endian，比如想生成 0xdeadbeef 需要传 ef be ad de
