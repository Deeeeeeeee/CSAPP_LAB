################
CS:APP Shell Lab
################

Files:

Makefile	# Compiles your shell program and runs the tests
README		# This file
tsh.c		# The shell program that you will write and hand in
tshref		# The reference shell binary.

# The remaining files are used to test your shell
sdriver.pl	# The trace-driven shell driver
trace*.txt	# The 15 trace files that control the shell driver
tshref.out 	# Example output of the reference shell on all 15 traces

# Little C programs that are called by the trace files
myspin.c	# Takes argument <n> and spins for <n> seconds
mysplit.c	# Forks a child that spins for <n> seconds
mystop.c        # Spins for <n> seconds and sends SIGTSTP to itself
myint.c         # Spins for <n> seconds and sends SIGINT to itself

# 需要做的事情

tsh.c 中完成下面几个函数的功能，建议在 [n行] 完成
- eval: Main routine that parses and interprets the command line.主程序中解析和解释命令行 [70 lines]
- builtin cmd: Recognizes and interprets the built-in commands: quit, fg, bg, and jobs.识别和解释内建命令，quit,fg,bg和jobs [25 lines]
- do bgfg: Implements the bg and fg built-in commands.实现bg和fg内建命令 [50 lines]
- waitfg: Waits for a foreground job to complete.等待前台job完成 [20 lines]
- sigchld handler: Catches SIGCHILD signals.子进程结束handler [80 lines]
- sigint handler: Catches SIGINT (ctrl-c) signals. [15 lines]
- sigtstp handler: Catches SIGTSTP (ctrl-z) signals. [15 lines]

运行：
```
> make
> ./tsh
```

# 程序一些功能

- built-in 命令在当前进程中执行
- 其他命令 fork 一个子进程执行. 称为一个 job
- 如果命令行以 & 结尾，则 job 在后台执行
- jobs: List the running and stopped background jobs.列出jobs
- bg <job>: Change a stopped background job to a running background job.将一个 stopped 的后台 job 转换成一个running job 
- fg <job>: Change a stopped or running background job to a running in the foreground.将一个 stopped 或者 running 后台 job 转换成一个 running 前台 job
- kill <job>: Terminate a job.中止一个 job 

# 要求

- 提示应该以 `tsh> ` 开头
- 要考虑 name 和参数(0 个或者多个) 
- 要支持 | 管道和 < > IO 重定向
- ctrl-c, ctrl-z 要发信号给当前的前台 job 
- 以 & 结尾要后台运行
- 每个 job 要能被区分开来. %5 表示 JID5, 5 表示 PID5
- 支持以下 built-in 命令
  - quit 退出 shell 
  - jobs 列出所有后台 job 
  - bg <job> 发送 SIGCONT 来重启 job, 并在后台执行,<job> 可以用 JID 也可以用 PID 
  - fg <job> 发送 SIGCONT 来重启 job, 并在前台执行,<job> 可以用 JID 也可以用 PID 
- 要回收所有的僵尸进程 

# 检查作业的工具

- tshref 用来参照的
- sdriver.pl 用子进程的方式执行 shell。可以用 trace file 来发送命令和信号
 - `./sdriver.pl -t trace01.txt -s ./tsh -a "-p"` -a "-p" 参数表示不需要提示
 - 或者运行 make test01
 - 可以用这两个命令来参照对比 `./sdriver.pl -t trace01.txt -s ./tshref -a "-p"` 和 `make rtest01`

# 作业要点
- 阅读 csapp 第八章
- 使用 trace files 来指导你的开发
- 要是用 waitpid,kill,fork,execve,setpgid 和 sigprocmask. WUNTRACED 和 WNOHANG 两个 waitpid 参数会用到
- 处理 signal handlers 时，确认发送 SIGINT 和 SIGSTP 信号到整个前台进程组，-[pid] 表示 kill 进程组. sdriver.pl 会测试这个
- 有个棘手的事情，waitfg 和 sigchld_handler 函数分配 work 上的区别，推荐
  - waitfg 使用 busy loop 和 sleep
  - sigchld_handler 使用 waitpid
- 在 eval 中，父进程必须在 forks 子进程前，使用 sigprocmask 去阻塞 SIGCHILD 信号，然后在 add child job 后使用 sigprocmask 去 unblock。另外因为子进程继承了父进程的 block 向量，所以也要 unblock
- 因为 shell 在真实的 unix shell 执行，所以会在同一个进程组中，ctrl-c 的时候会发给进程组下的所有进程，避免这种情况，fork 后，execve 之前，使用 setpgid(0, 0) 来修改子进程的进程组

