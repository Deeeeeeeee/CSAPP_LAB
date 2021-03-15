#include "csapp.h"

static volatile long num_jobs;

void initjobs()
{
    num_jobs = 0;
}

void addjob(int pid)
{
    num_jobs ++;
    printf("Add: %d jobs\n", num_jobs);
}

void deletejob(int pid)
{
    num_jobs --;
    sio_puts("Delete: ");
    sio_putl(num_jobs);
    sio_puts(" jobs\n");
}

/* $begin procmask1 */
/* WARNING: This code is buggy! */
void handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;

    Sigfillset(&mask_all);
    while ((pid = waitpid(-1, NULL, 0)) > 0) { /* Reap a zombie child */
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        deletejob(pid); /* Delete the child from the job list */
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    if (errno != ECHILD)
        Sio_error("waitpid error");
    errno = olderrno;
}

#define N 5
// 这里有个问题，因为子进程有可能在父进程 addjob 前结束，而结束的时候会发信号 SIGCHLD 给父进程
// 上下文切换到父进程的时候，会先执行 signal_handler，意味着可能先 deletejob
// 然后父进程才 addjob，add 了一个已经结束的 job，从而产生 bug
int main(int argc, char **argv)
{
    int pid;
    sigset_t mask_all, prev_all;
    int n = N;

    Sigfillset(&mask_all);
    Signal(SIGCHLD, handler);
    initjobs(); /* Initialize the job list */

    while (n--) {
        if ((pid = Fork()) == 0) { /* Child process */
            Execve("/bin/date", argv, NULL);
        }
        // block 住所有的信号，保存之前的 blocked set
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* Parent process */  
        addjob(pid);  /* Add the child to the job list */
        // 恢复原来的 blocked set
        Sigprocmask(SIG_SETMASK, &prev_all, NULL);    
    }
    exit(0);
}
/* $end procmask1 */
